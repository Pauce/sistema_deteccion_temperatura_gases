/*
 * rtc_device.h
 *
 *  Created on: 21/04/2026
 *      Author: plibreros
 */

#ifndef DEVICES_INC_RTC_DEVICE_H_
#define DEVICES_INC_RTC_DEVICE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "rtc_freertos.h"

typedef enum {
	RTC_Status_Ok, RTC_Status_Error
} _status_rtc;

_status_rtc rtc_device_init(rtc_rtos_handle_t *rtc_h,
		rtc_datetime_t *initial_dt);

_status_rtc rtc_device_update_date(rtc_rtos_handle_t *rtc_h,
		char *buff_date, uint8_t *len_date, rtc_datetime_t *dt);

_status_rtc rtc_device_update_time(rtc_rtos_handle_t *rtc_h,
		char *buff_time, uint8_t *len_time, rtc_datetime_t *dt);

#ifdef __cplusplus
}
#endif

#endif /* DEVICES_INC_RTC_DEVICE_H_ */
