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

	status_t st = rtc_rtos_init(rtc_h, pdMS_TO_TICKS(RTC_DEVICE_TICKS_MS));

	if (st != kStatus_Success)
		return RTC_Status_Error;

	rtc_rtos_set_datetime(rtc_h, initial_dt);

	return RTC_Status_Ok;
}

_status_rtc rtc_device_update_date(rtc_rtos_handle_t *rtc_h,
		char *buff_date, uint8_t *len_date, rtc_datetime_t *dt) {

	if (rtc_rtos_get_datetime(rtc_h, dt) != kStatus_Success)
		return RTC_Status_Error;

	if (rtc_rtos_update_date(rtc_h, buff_date, len_date, dt)
			!= kStatus_Success)
		return RTC_Status_Error;

	return RTC_Status_Ok;
}

_status_rtc rtc_device_update_time(rtc_rtos_handle_t *rtc_h,
		char *buff_time, uint8_t *len_time, rtc_datetime_t *dt){

	if(rtc_rtos_update_time(rtc_h, buff_time, len_time, dt) != kStatus_Success)
		return RTC_Status_Error;

	return RTC_Status_Ok;
}

