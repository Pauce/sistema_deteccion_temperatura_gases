/*
 * rtc_device.c
 *
 *  Created on: 21/04/2026
 *      Author: plibreros
 */
#include "rtc_device.h"

#include "FreeRTOS.h"
#include "task.h"

#define RTC_DEVICE_TICKS_MS		1000

_status_rtc rtc_device_init(rtc_rtos_handle_t *rtc_h,
		rtc_datetime_t *initial_dt) {
//	rtc_datetime_t initial_dt = {
//			.year 	= 2026,
//			.month	= 4,
//			.day	= 21,
//			.hour	= 0,
//			.minute = 0,
//			.second	= 0
//	};

	status_t st = rtc_rtos_init(rtc_h, pdMS_TO_TICKS(RTC_DEVICE_TICKS_MS));

	if (st != kStatus_Success)
		return RTC_Status_Error;

	rtc_rtos_set_datetime(rtc_h, initial_dt);

	return RTC_Status_Ok;
}

