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

#ifdef __cplusplus
}
#endif

#endif /* DEVICES_INC_RTC_DEVICE_H_ */
