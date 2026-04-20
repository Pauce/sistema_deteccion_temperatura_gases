/*
 * i2c_freertos.h
 *
 *  Created on: 26/03/2026
 *      Author: plibreros
 */

#ifndef DRIVERS_INC_I2C_FREERTOS_H_
#define DRIVERS_INC_I2C_FREERTOS_H_

#include "FreeRTOS.h"
#include "portable.h"
#include "semphr.h"

#include "fsl_i2c.h"
#include "fsl_flexcomm.h"

#define I2C_DATA_LENGTH            (32)     /* MAX is 256 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*! @brief I2C FreeRTOS handle */
typedef struct {
    I2C_Type *base;                 /*!< I2C base address */
    i2c_master_handle_t drv_handle; /*!< A handle of the underlying driver, treated as opaque by the RTOS layer */
    status_t async_status;          /*!< Transactional state of the underlying driver */
    SemaphoreHandle_t mutex;        /*!< A mutex to lock the handle during a transfer */
    SemaphoreHandle_t semaphore;    /*!< A semaphore to notify and unblock task when the transfer ends */
    TickType_t	ticks_to_wait_ms;
} i2c_rtos_handle_t;

typedef struct {
	i2c_master_config_t masterConfig;
	i2c_master_transfer_t masterXfer;
	i2c_rtos_handle_t master_rtos_handle;
	uint8_t g_master_buff_rx[I2C_DATA_LENGTH];
	uint8_t g_master_buff_tx[I2C_DATA_LENGTH];
} i2c_rtos_t;



#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @name I2C RTOS Operation
 * @{
 */

/*!
 * @brief Initializes I2C.
 *
 * This function initializes the I2C module and the related RTOS context.
 *
 * @param handle The RTOS I2C handle, the pointer to an allocated space for RTOS context.
 * @param base The pointer base address of the I2C instance to initialize.
 * @param masterConfig Configuration structure to set-up I2C in master mode.
 * @param srcClock_Hz Frequency of input clock of the I2C module.
 * @return status of the operation.
 */
status_t i2c_rtos_init(i2c_rtos_handle_t *handle,
                       I2C_Type *base,
                       i2c_master_config_t *masterConfig);

/*!
 * @brief Deinitializes the I2C.
 *
 * This function deinitializes the I2C module and the related RTOS context.
 *
 * @param handle The RTOS I2C handle.
 */
status_t i2c_rtos_deinit(i2c_rtos_handle_t *handle);

/*!
 * @brief Performs I2C transfer.
 *
 * This function performs an I2C transfer according to data given in the transfer structure.
 *
 * @param handle The RTOS I2C handle.
 * @param transfer Structure specifying the transfer parameters.
 * @return status of the operation.
 */
status_t i2c_rtos_transfer(i2c_rtos_handle_t *handle, i2c_master_transfer_t *transfer);

/*!
 * @}
 */

#if defined(__cplusplus)
}
#endif

#endif /* DRIVERS_INC_I2C_FREERTOS_H_ */
