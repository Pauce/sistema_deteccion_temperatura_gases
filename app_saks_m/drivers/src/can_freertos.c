/*
 * can_freertos.c
 *
 *  Created on: 26/03/2026
 *      Author: plibreros
 */
#include "can_freertos.h"

#define	DEFAULT_BAUDRATE_CAN		250000U
#define MSG_RAM_BASE            	0x04000000U
#define USE_IMPROVED_TIMING_CONFIG 	(1U)
#define STDID_OFFSET            	(18U)
#define msgRam 						MSG_RAM_BASE
#define STD_FILTER_OFS 				0x0
#define RX_FIFO0_OFS 				0x10U
#define TX_BUFFER_OFS 				0x20U
#define MSG_RAM_SIZE 				(TX_BUFFER_OFS + 8 + CAN_DATA_SIZE)
#define NVI_CAN_PRIORITY			2

static void mcan_callback(CAN_Type *base, mcan_handle_t *handle,
		status_t status, uint32_t result, void *userData);
static inline void notify_status(mcan_rtos_handle_t *can_open, status_t status,
		BaseType_t *reschedule);

/*
 * Single callback invoked from ISR.
 * Discriminates between TX and RX based on the 'status' value and signals
 * the corresponding semaphore. This eliminates ambiguity
 * even if TX and RX fire very close together.
 */
static void mcan_callback(CAN_Type *base, mcan_handle_t *handle,
		status_t status, uint32_t result, void *userData) {
	if (userData == NULL)
		return;

	mcan_rtos_handle_t *can_open = (mcan_rtos_handle_t*) userData;

	if (can_open->base != base)
		return;
	BaseType_t reschedule = pdFALSE;

	notify_status(can_open, status, &reschedule);

	portYIELD_FROM_ISR(reschedule);
}

static inline void notify_status(mcan_rtos_handle_t *can_open, status_t status,
		BaseType_t *reschedule) {
	switch (status) {
	case kStatus_MCAN_TxIdle:
	case kStatus_MCAN_TxBusy:
		can_open->async_status_tx = status;
		(void) xSemaphoreGiveFromISR(can_open->semaphore_tx, reschedule);
		break;
	case kStatus_MCAN_RxFifo0Idle:
	case kStatus_MCAN_RxFifo0Busy:
	case kStatus_MCAN_RxFifo0Full:
		can_open->async_status_rx = status;
		(void) xSemaphoreGiveFromISR(can_open->semaphore_rx, reschedule);
		break;
	case kStatus_MCAN_RxIdle:
	case kStatus_MCAN_RxBusy:
		can_open->async_status_rx = status;
		(void) xSemaphoreGiveFromISR(can_open->semaphore_rx, reschedule);
		break;

	default:
		break;
	}
}

status_t mcan_rtos_init(mcan_rtos_handle_t *can_open, CAN_Type *base,
		TickType_t delay_ticks) {

	if (can_open == NULL || base == NULL)
		return kStatus_InvalidArgument;

	(void) memset(can_open, 0, sizeof(mcan_rtos_handle_t));

	can_open->mutex = xSemaphoreCreateMutex();
	if (can_open->mutex == NULL)
		return kStatus_Fail;

	can_open->semaphore_tx = xSemaphoreCreateBinary();

	if (can_open->semaphore_tx == NULL) {
		vSemaphoreDelete(can_open->mutex);
		return kStatus_Fail;
	}

	can_open->semaphore_rx = xSemaphoreCreateBinary();
	if (can_open->semaphore_rx == NULL) {
		vSemaphoreDelete(can_open->semaphore_tx);
		vSemaphoreDelete(can_open->mutex);
		return kStatus_Fail;
	}

	can_open->base = base;

	MCAN_GetDefaultConfig(&can_open->mcanConfig);
	can_open->mcanConfig.baudRateA = DEFAULT_BAUDRATE_CAN;
	can_open->mcanConfig.baudRateD = DEFAULT_BAUDRATE_CAN;

#if (defined(USE_IMPROVED_TIMING_CONFIG) && USE_IMPROVED_TIMING_CONFIG)
	mcan_timing_config_t timing_config;
	memset(&timing_config, 0, sizeof(timing_config));
	if (MCAN_CalculateImprovedTimingValues(can_open->mcanConfig.baudRateA,
			CLOCK_GetMCanClkFreq(), &timing_config)) {
		/* Update the improved timing configuration*/
		memcpy(&(can_open->mcanConfig.timingConfig), &timing_config,
				sizeof(mcan_timing_config_t));
	}
#endif

	MCAN_Init(can_open->base, &can_open->mcanConfig, CLOCK_GetMCanClkFreq());

	can_open->memoryConfig.baseAddr = (uint32_t) (msgRam);
	/* STD filter config. */
	can_open->rxFilter.address = STD_FILTER_OFS;
	can_open->rxFilter.idFormat = kMCAN_FrameIDStandard;
	can_open->rxFilter.listSize = 1U;
	can_open->rxFilter.nmFrame = kMCAN_reject0;
	can_open->rxFilter.remFrame = kMCAN_rejectFrame;
	can_open->memoryConfig.stdFilterCfg = &can_open->rxFilter;
	/* RX fifo0 config. */
	can_open->rxFifo0.address = RX_FIFO0_OFS;
	can_open->rxFifo0.elementSize = 1U;
	can_open->rxFifo0.watermark = 0;
	can_open->rxFifo0.opmode = kMCAN_FifoBlocking;
	can_open->rxFifo0.datafieldSize = kMCAN_8ByteDatafield;
	can_open->memoryConfig.rxFifo0Cfg = &can_open->rxFifo0;
	/* TX buffer config. */
	can_open->txBuffer.address = TX_BUFFER_OFS;
	can_open->txBuffer.dedicatedSize = 1U;
	can_open->txBuffer.fqSize = 0;
	can_open->txBuffer.datafieldSize = kMCAN_8ByteDatafield;
	can_open->memoryConfig.txBufferCfg = &can_open->txBuffer;
	/* Set Message RAM config and clear memory to avoid BEU/BEC error. */
	memset((void*) msgRam, 0, MSG_RAM_SIZE * sizeof(uint8_t));

	if (kStatus_Success
			!= MCAN_SetMessageRamConfig(can_open->base,
					&can_open->memoryConfig)) {
		return kStatus_Fail;
	}
	/* Filling Standard ID filter element with Classic filter mode, which only filter matching ID. */
	can_open->stdFilter.sfec = kMCAN_storeinFifo0;
	can_open->stdFilter.sft = kMCAN_classic;
	can_open->stdFilter.sfid1 = 0x5FBU;
	can_open->stdFilter.sfid2 = 0x7FFU;
	MCAN_SetSTDFilterElement(can_open->base, &can_open->rxFilter,
			&can_open->stdFilter, 0);

	MCAN_TransferCreateHandle(can_open->base, &can_open->mcanHandle,
			mcan_callback, can_open);

	NVIC_SetPriority(CAN0_IRQ0_IRQn, NVI_CAN_PRIORITY);
	NVIC_SetPriority(CAN0_IRQ1_IRQn, NVI_CAN_PRIORITY);
	can_open->ticks_to_wait_tick = delay_ticks;

	return kStatus_Success;
}

status_t mcan_rtos_deinit(mcan_rtos_handle_t *can_open) {

	if (can_open == NULL)
		return kStatus_Fail;

	MCAN_Deinit(can_open->base);

	vSemaphoreDelete(can_open->semaphore_tx);
	vSemaphoreDelete(can_open->semaphore_rx);
	vSemaphoreDelete(can_open->mutex);

	return kStatus_Success;
}

status_t mcan_rtos_transfer_send(mcan_rtos_handle_t *can_open, uint8_t *data,
		size_t len_data, uint32_t id) {

	if (can_open == NULL || data == NULL)
		return kStatus_InvalidArgument;
	if (len_data > CAN_DATA_SIZE)
		return kStatus_InvalidArgument;

	if (xSemaphoreTake(can_open->mutex, can_open->ticks_to_wait_tick) == pdFALSE)
		return kStatus_MCAN_TxBusy;

	can_open->txFrame.xtd = kMCAN_FrameIDStandard;
	can_open->txFrame.rtr = kMCAN_FrameTypeData;
	can_open->txFrame.fdf = 0U;
	can_open->txFrame.brs = 0U;
	can_open->txFrame.dlc = CAN_DATA_SIZE;
	can_open->txFrame.id = id << STDID_OFFSET;
	can_open->txFrame.data = data;
	can_open->txFrame.size = (uint8_t) (len_data);
	can_open->txXfer.frame = &can_open->txFrame;
	can_open->txXfer.bufferIdx = 0;

	status_t status = MCAN_TransferSendNonBlocking(can_open->base,
			&can_open->mcanHandle, &can_open->txXfer);

	if (status != kStatus_Success) {
		(void) xSemaphoreGive(can_open->mutex);
		return status;
	}

	/* Wait for transfer to finish */
	if (xSemaphoreTake(can_open->semaphore_tx,
			can_open->ticks_to_wait_tick) == pdFALSE) {
		(void) xSemaphoreGive(can_open->mutex);
		return kStatus_Fail;
	}

	/* Unlock resource mutex */
	(void) xSemaphoreGive(can_open->mutex);

	if (can_open->async_status_tx != kStatus_MCAN_TxIdle)
		return kStatus_Fail;

	return kStatus_Success;
}

status_t mcan_rtos_transfer_receive(mcan_rtos_handle_t *can_open,
		uint8_t *data_received, size_t len_data_received) {
	if (can_open == NULL || data_received == NULL)
		return kStatus_InvalidArgument;

	if (len_data_received > CAN_DATA_SIZE)
		return kStatus_InvalidArgument;

	if (xSemaphoreTake(can_open->mutex, can_open->ticks_to_wait_tick) == pdFALSE)
		return kStatus_MCAN_RxBusy;

	can_open->rxFrame.size = len_data_received;
	can_open->rxXfer.frame = &can_open->rxFrame;

	status_t status = MCAN_TransferReceiveFifoNonBlocking(can_open->base, 0,
			&can_open->mcanHandle, &can_open->rxXfer);

	if (status != kStatus_Success) {
		(void) xSemaphoreGive(can_open->mutex);
		return status;
	}

	if (xSemaphoreTake(can_open->semaphore_rx,
			can_open->ticks_to_wait_tick) == pdFALSE) {
		(void) xSemaphoreGive(can_open->mutex);
		return kStatus_Fail;
	}

	/* Unlock resource mutex */
	(void) xSemaphoreGive(can_open->mutex);

	if (can_open->async_status_rx != kStatus_MCAN_RxFifo0Idle)
		return kStatus_Fail;

	(void) memset(data_received, 0, len_data_received);
	(void) memcpy(data_received, can_open->rxFrame.data,
			can_open->rxFrame.size);

	return kStatus_Success;
}

status_t mcan_rtos_transfer_send_receive(mcan_rtos_handle_t *can_open,
		uint8_t *data_send, size_t len_send, uint32_t id, uint8_t *data_recv,
		size_t len_recv) {
	if (can_open == NULL || data_send == NULL || data_recv == NULL)
		return kStatus_InvalidArgument;

	if (xSemaphoreTake(can_open->mutex, can_open->ticks_to_wait_tick) == pdFALSE)
		return kStatus_MCAN_TxBusy;

	/* --- Configurar TX --- */
	can_open->txFrame.xtd = kMCAN_FrameIDStandard;
	can_open->txFrame.rtr = kMCAN_FrameTypeData;
	can_open->txFrame.fdf = 0U;
	can_open->txFrame.brs = 0U;
	can_open->txFrame.dlc = CAN_DATA_SIZE;
	can_open->txFrame.id = id << STDID_OFFSET;
	can_open->txFrame.data = data_send;
	can_open->txFrame.size = (uint8_t) (len_send);
	can_open->txXfer.frame = &can_open->txFrame;
	can_open->txXfer.bufferIdx = 0;

	/* --- Configurar RX (antes de enviar para no perder la respuesta) --- */
	can_open->rxFrame.size = len_recv;
	can_open->rxXfer.frame = &can_open->rxFrame;

	/*
	 * Armar la recepción PRIMERO para que el handler interno del SDK
	 * ya esté registrado cuando llegue la respuesta del nodo remoto.
	 * Esto elimina la condición de carrera.
	 */
	status_t status = MCAN_TransferReceiveFifoNonBlocking(can_open->base, 0,
			&can_open->mcanHandle, &can_open->rxXfer);

	if (status != kStatus_Success) {
		(void) xSemaphoreGive(can_open->mutex);
		return status;
	}

	/* --- Enviar --- */
	status = MCAN_TransferSendNonBlocking(can_open->base, &can_open->mcanHandle,
			&can_open->txXfer);

	if (status != kStatus_Success) {
		/* Cancelar recepción pendiente si el envío falla */
		MCAN_TransferAbortReceiveFifo(can_open->base, 0, &can_open->mcanHandle);
		(void) xSemaphoreGive(can_open->mutex);
		return status;
	}

	/* --- Esperar TX done (semaphore_tx) --- */
	if (xSemaphoreTake(can_open->semaphore_tx,
			can_open->ticks_to_wait_tick) == pdFALSE) {
		MCAN_TransferAbortReceiveFifo(can_open->base, 0, &can_open->mcanHandle);
		(void) xSemaphoreGive(can_open->mutex);
		return kStatus_Fail;
	}

	if (can_open->async_status_tx != kStatus_MCAN_TxIdle) {
		MCAN_TransferAbortReceiveFifo(can_open->base, 0, &can_open->mcanHandle);
		(void) xSemaphoreGive(can_open->mutex);
		return kStatus_Fail;
	}

	/* --- Esperar RX done (semaphore_rx) --- */
	if (xSemaphoreTake(can_open->semaphore_rx,
			can_open->ticks_to_wait_tick) == pdFALSE) {
		(void) xSemaphoreGive(can_open->mutex);
		return kStatus_Fail;
	}

	(void) xSemaphoreGive(can_open->mutex); /* único punto de liberación */

	if (can_open->async_status_rx != kStatus_MCAN_RxFifo0Idle)
		return kStatus_Fail;

	if (can_open->rxFrame.size == 0U)
		return kStatus_Fail;

	(void) memset(data_recv, 0, len_recv);
	(void) memcpy(data_recv, can_open->rxFrame.data, can_open->rxFrame.size);

	return kStatus_Success;
}
