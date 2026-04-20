/*
 * can_freertos.h
 *
 *  Created on: 26/03/2026
 *      Author: plibreros
 */

#ifndef DRIVERS_INC_CAN_FREERTOS_H_
#define DRIVERS_INC_CAN_FREERTOS_H_

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------
 * FreeRTOS Kernel Includes
 *----------------------------------------*/
#include <stdbool.h>
#include "FreeRTOS.h"
#include "portable.h"
#include "semphr.h"
#include "fsl_debug_console.h"

#include "fsl_mcan.h"

#define CAN_DATA_SIZE 				(8U)

typedef struct {
	CAN_Type *base;
	mcan_rx_buffer_frame_t rxFrame;
	mcan_tx_buffer_frame_t txFrame;

	mcan_handle_t mcanHandle;
	mcan_config_t mcanConfig;
	mcan_buffer_transfer_t txXfer;
	mcan_fifo_transfer_t rxXfer;
	mcan_memory_config_t memoryConfig;
	mcan_frame_filter_config_t rxFilter;
	mcan_std_filter_element_config_t stdFilter;
	mcan_rx_fifo_config_t rxFifo0;
	mcan_tx_buffer_config_t txBuffer;

    volatile status_t async_status_tx;    		/*!< Status of last TX transfer */
    volatile status_t async_status_rx;    		/*!< Status of last RX transfer */
    SemaphoreHandle_t mutex;        			/*!< A mutex to lock the handle during a transfer */
    SemaphoreHandle_t semaphore_tx;    			/*!< Semaphore signaling the end of TX */
    SemaphoreHandle_t semaphore_rx;    			/*!< Semaphore signaling the end of RX */
    TickType_t	ticks_to_wait_tick;
} mcan_rtos_handle_t;

status_t mcan_rtos_init(mcan_rtos_handle_t *can_open, CAN_Type *base, TickType_t delay_ticks);
status_t mcan_rtos_deinit(mcan_rtos_handle_t *can_open);
status_t mcan_rtos_transfer_send(mcan_rtos_handle_t *can_open, uint8_t *data, size_t len_data, uint32_t id);
status_t mcan_rtos_transfer_receive(mcan_rtos_handle_t *can_open, uint8_t *data_received, size_t len_data_received);
status_t mcan_rtos_transfer_send_receive(mcan_rtos_handle_t *can_open, uint8_t *data_send, size_t len_send, uint32_t id, uint8_t *data_recv, size_t len_recv);

#ifdef __cplusplus
}
#endif

#endif /* DRIVERS_INC_CAN_FREERTOS_H_ */
