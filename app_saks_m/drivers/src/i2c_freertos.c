/*
 * i2c_freertos.c
 *
 *  Created on: 26/03/2026
 *      Author: plibreros
 */

#include "i2c_freertos.h"

#define I2C_NVIC_PRIO 				2
#define I2C_BAUDRATE               (100000) /* 100K */

static void i2c_rtos_callback(I2C_Type *base, i2c_master_handle_t *drv_handle,
		status_t status, void *userData);

static void i2c_rtos_callback(I2C_Type *base, i2c_master_handle_t *drv_handle,
		status_t status, void *userData) {
	i2c_rtos_handle_t *handle = (i2c_rtos_handle_t*) userData;
	BaseType_t reschedule = pdFALSE;
	handle->async_status = status;
	(void) xSemaphoreGiveFromISR(handle->semaphore, &reschedule);
	portYIELD_FROM_ISR(reschedule);
}

/*!
 * brief Initializes I2C.
 *
 * This function initializes the I2C module and the related RTOS context.
 *
 * param handle The RTOS I2C handle, the pointer to an allocated space for RTOS context.
 * param base The pointer base address of the I2C instance to initialize.
 * param masterConfig Configuration structure to set-up I2C in master mode.
 * param srcClock_Hz Frequency of input clock of the I2C module.
 * return status of the operation.
 */
status_t i2c_rtos_init(i2c_rtos_handle_t *handle, I2C_Type *base,
		i2c_master_config_t *masterConfig) {
	if (handle == NULL)
		return kStatus_InvalidArgument;

	if (base == NULL)
		return kStatus_InvalidArgument;

	uint32_t time_wait = handle->ticks_to_wait_ms;

	(void) memset(handle, 0, sizeof(i2c_rtos_handle_t));

	handle->mutex = xSemaphoreCreateMutex();

	if (handle->mutex == NULL)
		return kStatus_Fail;

	handle->semaphore = xSemaphoreCreateBinary();

	if (handle->semaphore == NULL) {
		vSemaphoreDelete(handle->mutex);
		return kStatus_Fail;
	}

	handle->base = base;
	handle->ticks_to_wait_ms = time_wait;

	uint32_t instance = FLEXCOMM_GetInstance(base);
	IRQn_Type irqn = kFlexcommIrqs[instance];

	NVIC_SetPriority(irqn, (I2C_NVIC_PRIO + 1));
	EnableIRQ(irqn);

	I2C_MasterGetDefaultConfig(masterConfig);
	masterConfig->baudRate_Bps = I2C_BAUDRATE;

	uint32_t srcClk = CLOCK_GetFlexCommClkFreq(instance);

	I2C_MasterInit(handle->base, masterConfig, srcClk);
	I2C_MasterTransferCreateHandle(base, &handle->drv_handle, i2c_rtos_callback,
			(void*) handle);

	return kStatus_Success;
}

/*!
 * brief Deinitializes the I2C.
 *
 * This function deinitializes the I2C module and the related RTOS context.
 *
 * param handle The RTOS I2C handle.
 */
status_t i2c_rtos_deinit(i2c_rtos_handle_t *handle) {
	I2C_MasterDeinit(handle->base);

	vSemaphoreDelete(handle->semaphore);
	vSemaphoreDelete(handle->mutex);

	return kStatus_Success;
}

/*!
 * brief Performs I2C transfer.
 *
 * This function performs an I2C transfer according to data given in the transfer structure.
 *
 * param handle The RTOS I2C handle.
 * param transfer Structure specifying the transfer parameters.
 * return status of the operation.
 */
status_t i2c_rtos_transfer(i2c_rtos_handle_t *handle,
		i2c_master_transfer_t *transfer) {
	status_t status;

	/* Lock resource mutex */
	if (xSemaphoreTake(handle->mutex,
			pdMS_TO_TICKS(handle->ticks_to_wait_ms)) != pdTRUE)
		return kStatus_I2C_Busy;

	status = I2C_MasterTransferNonBlocking(handle->base, &handle->drv_handle,
			transfer);

	if (status != kStatus_Success) {
		(void) xSemaphoreGive(handle->mutex);
		return status;
	}

	/* Wait for transfer to finish */
	if (xSemaphoreTake(handle->semaphore,
			pdMS_TO_TICKS(handle->ticks_to_wait_ms)) != pdTRUE)
		return kStatus_Fail;

	/* Unlock resource mutex */
	(void) xSemaphoreGive(handle->mutex);

	/* Return status captured by callback function */
	return handle->async_status;
}
