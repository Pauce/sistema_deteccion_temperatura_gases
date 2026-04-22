/*
 * rtc_freertos.h
 *
 *  Created on: 21/04/2026
 *      Author: plibreros
 */

#ifndef DRIVERS_INC_RTC_FREERTOS_H_
#define DRIVERS_INC_RTC_FREERTOS_H_

#ifdef __cplusplus
extern "C" {
#endif

/* FreeRTOS */
#include "FreeRTOS.h"
#include "semphr.h"
#include "portable.h"
#include "fsl_common.h"
#include "fsl_rtc.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @brief String buffer length required by the format helpers (DD/MM/YYYY\0). */
#define RTC_DATE_STR_LEN    (11U)

/*! @brief String buffer length required by the format helpers (HH:MM:SS\0). */
#define RTC_TIME_STR_LEN    (9U)

/*!
 * @brief FreeRTOS RTC handle.
 *
 * Allocate one instance per application; pass its address to every API call.
 * Initialise with rtc_rtos_init() before use.
 */
typedef struct {
	SemaphoreHandle_t mutex; /*!< Serialises concurrent API callers. */
	SemaphoreHandle_t semaphore; /*!< Signalled from the RTC alarm ISR.  */
	TickType_t ticks_to_wait; /*!< Max ticks to block on mutex / ISR. */
	uint8_t cached_day; /*!< Last known day – drives date-change detection. */
} rtc_rtos_handle_t;

/*******************************************************************************
 * API
 ******************************************************************************/
/*!
 * @brief Initialises the RTC peripheral and its FreeRTOS handle.
 *
 * Creates the mutex and binary semaphore, configures and starts the RTC
 * hardware, and enables the alarm IRQ through the NVIC.
 *
 * @param handle        Pointer to an allocated (but uninitialised) handle.
 * @param ticks_to_wait Maximum tick count for all blocking operations.
 *                      Pass portMAX_DELAY to block indefinitely.
 * @return kStatus_Success or kStatus_Fail if OS object creation failed.
 */
status_t rtc_rtos_init(rtc_rtos_handle_t *handle, TickType_t ticks_to_wait);

/*!
 * @brief Deinitialises the RTC peripheral and releases OS objects.
 *
 * @param handle Pointer to an initialised handle.
 * @return kStatus_Success.
 */
status_t rtc_rtos_deinit(rtc_rtos_handle_t *handle);

/*!
 * @brief Sets the RTC date and time.
 *
 * Acquires the mutex, programs the hardware, then releases the mutex.
 *
 * @param handle Pointer to an initialised handle.
 * @param dt     Date/time to write.
 * @return kStatus_Success, kStatus_InvalidArgument, or kStatus_I2C_Busy
 *         (mutex timeout).
 */
status_t rtc_rtos_set_datetime(rtc_rtos_handle_t *handle,
		const rtc_datetime_t *dt);

/*!
 * @brief Reads the current RTC date and time.
 *
 * Acquires the mutex, reads the hardware, then releases the mutex.
 *
 * @param handle Pointer to an initialised handle.
 * @param dt     Output: populated with the current date/time.
 * @return kStatus_Success or kStatus_Fail (mutex timeout).
 */
status_t rtc_rtos_get_datetime(rtc_rtos_handle_t *handle, rtc_datetime_t *dt);

/*!
 * @brief Arms a one-second RTC alarm.
 *
 * The alarm ISR will signal the internal semaphore one second later.
 * Call rtc_rtos_wait_alarm() to block until that event fires.
 *
 * @param handle Pointer to an initialised handle.
 * @return kStatus_Success or kStatus_Fail (mutex timeout).
 */
status_t rtc_rtos_arm_alarm_1s(rtc_rtos_handle_t *handle);

/*!
 * @brief Blocks until the previously armed alarm fires.
 *
 * The calling task is suspended (not spinning) while waiting.
 *
 * @param handle Pointer to an initialised handle.
 * @return kStatus_Success when the alarm fired, or kStatus_Fail on timeout.
 */
status_t rtc_rtos_wait_alarm(rtc_rtos_handle_t *handle);

/*!
 * @brief Reads the RTC time and formats it as "HH:MM:SS".
 *
 * This is a convenience wrapper: it arms the alarm, waits for it, reads the
 * time, and writes the formatted string. Typical use inside a 1-Hz task loop.
 *
 * @param handle     Pointer to an initialised handle.
 * @param str        Output buffer – must be at least RTC_TIME_STR_LEN bytes.
 * @param len        Output: length of the formatted string.
 * @param dt         Output: raw date/time struct.
 * @return kStatus_Success or kStatus_Fail.
 */
status_t rtc_rtos_update_time(rtc_rtos_handle_t *handle, char *str,
		uint8_t *len, rtc_datetime_t *dt);

/*!
 * @brief Returns a formatted date string "DD/MM/YYYY" only when the day changes.
 *
 * Compares the current day against a cached value.  Returns kStatus_Success
 * and populates @p str only on a day transition; returns kStatus_NoData
 * otherwise.
 *
 * @param handle     Pointer to an initialised handle.
 * @param str        Output buffer – must be at least RTC_DATE_STR_LEN bytes.
 * @param len        Output: length of the formatted string.
 * @param dt         Current date/time (obtained from rtc_rtos_get_datetime).
 * @return kStatus_Success on day-change, kStatus_NoData if unchanged, or
 *         kStatus_Fail on mutex timeout.
 */
status_t rtc_rtos_update_date(rtc_rtos_handle_t *handle, char *str,
		uint8_t *len, const rtc_datetime_t *dt);

/*!
 * @brief Formats the date one year ahead as "DD/MM/YYYY".
 *
 * Handles the 29-Feb edge case: if the computed year is not a leap year the
 * day is clamped to 28.
 *
 * @param str  Output buffer – must be at least RTC_DATE_STR_LEN bytes.
 * @param len  Output: length of the formatted string.
 * @param dt   Source date (not modified).
 */
void rtc_rtos_format_next_year(char *str, uint8_t *len,
		const rtc_datetime_t *dt);

#ifdef __cplusplus
}
#endif

#endif /* DRIVERS_INC_RTC_FREERTOS_H_ */
