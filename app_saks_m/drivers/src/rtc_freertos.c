///*
// * rtc_freertos.c
// *
// *  Created on: 21/04/2026
// *      Author: plibreros
// */
#include "rtc_freertos.h"
/*******************************************************************************
 * Private – ISR linkage
 ******************************************************************************/

/*! Pointer set by rtc_rtos_init; used only inside the ISR. */
static rtc_rtos_handle_t *rtc_rtos_active_handle = NULL;

/*******************************************************************************
 * ISR
 ******************************************************************************/

void RTC_IRQHandler(void) {
	if (RTC_GetStatusFlags(RTC) & kRTC_AlarmFlag) {

		RTC_ClearStatusFlags(RTC, kRTC_AlarmFlag);

		if((rtc_rtos_active_handle != NULL) && (rtc_rtos_active_handle->semaphore != NULL))
		{
			BaseType_t higher_task_woken = pdFALSE;
			(void) xSemaphoreGiveFromISR(rtc_rtos_active_handle->semaphore,
					&higher_task_woken);
			portYIELD_FROM_ISR(higher_task_woken);
		}
	}

	SDK_ISR_EXIT_BARRIER;
}

/*******************************************************************************
 * Private helpers – string formatting (no heap, no sprintf)
 ******************************************************************************/

static void append_two_digit(uint8_t value, char *str) {
	size_t len = strlen(str);
	str[len++] = '0' + (value / 10);
	str[len++] = '0' + (value % 10);
	str[len] = '\0';
}

static void append_four_digit(uint16_t value, char *str) {
	size_t len = strlen(str);
	str[len++] = '0' + (value / 1000) % 10;
	str[len++] = '0' + (value / 100) % 10;
	str[len++] = '0' + (value / 10) % 10;
	str[len++] = '0' + (value % 10);
	str[len] = '\0';
}

static void format_date(char *str, uint8_t *len, const rtc_datetime_t *dt) {
	const char sep = '/';
	str[0] = '\0';

	append_two_digit(dt->day, str);
	strncat(str, &sep, 1);
	append_two_digit(dt->month, str);
	strncat(str, &sep, 1);
	append_four_digit(dt->year, str);

	*len = (uint8_t) strlen(str);
}

static void format_time(char *str, uint8_t *len, const rtc_datetime_t *dt) {
	const char sep[2] = { ':', '\0' };

	memset(str, 0x00, *len);

	append_two_digit(dt->hour, str);
	strcat(str, sep);
	append_two_digit(dt->minute, str);
	strcat(str, sep);
	append_two_digit(dt->second, str);

	*len = (uint8_t) strlen(str);
}

/*******************************************************************************
 * Public API
 ******************************************************************************/

status_t rtc_rtos_init(rtc_rtos_handle_t *handle, TickType_t ticks_to_wait) {
	if (handle == NULL) {
		return kStatus_InvalidArgument;
	}

	(void) memset(handle, 0, sizeof(rtc_rtos_handle_t));
	handle->ticks_to_wait = ticks_to_wait;
	handle->cached_day = 0xFF; /* sentinel – forces first-call date update */

	/* --- OS objects -------------------------------------------------------- */
	handle->mutex = xSemaphoreCreateMutex();
	if (handle->mutex == NULL) {
		return kStatus_Fail;
	}

	handle->semaphore = xSemaphoreCreateBinary();
	if (handle->semaphore == NULL) {
		vSemaphoreDelete(handle->mutex);
		handle->mutex = NULL;
		return kStatus_Fail;
	}

	/* Publish handle so the ISR can reach the semaphore */
	rtc_rtos_active_handle = handle;

	/* --- Hardware ---------------------------------------------------------- */
	RTC_Init(RTC);
	RTC_EnableTimer(RTC, false); /* Must stop counter before writing TSR  */
	RTC_ClearStatusFlags(RTC, kRTC_AlarmFlag);
	NVIC_SetPriority(RTC_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1);
	EnableIRQ(RTC_IRQn);
	RTC_EnableTimer(RTC, true);

	return kStatus_Success;
}

/* -------------------------------------------------------------------------- */

status_t rtc_rtos_deinit(rtc_rtos_handle_t *handle) {
	if (handle == NULL) {
		return kStatus_InvalidArgument;
	}

	DisableIRQ(RTC_IRQn);
	rtc_rtos_active_handle = NULL;

	RTC_EnableTimer(RTC, false);
	RTC_Deinit(RTC);

	vSemaphoreDelete(handle->semaphore);
	vSemaphoreDelete(handle->mutex);

	handle->semaphore = NULL;
	handle->mutex = NULL;

	return kStatus_Success;
}

/* -------------------------------------------------------------------------- */

status_t rtc_rtos_set_datetime(rtc_rtos_handle_t *handle,
		const rtc_datetime_t *dt) {
	if (handle == NULL || dt == NULL) {
		return kStatus_InvalidArgument;
	}

	if (xSemaphoreTake(handle->mutex, handle->ticks_to_wait) != pdTRUE) {
		return kStatus_Fail;
	}

	status_t status = RTC_SetDatetime(RTC, dt);

	(void) xSemaphoreGive(handle->mutex);

	return status;
}

/* -------------------------------------------------------------------------- */

status_t rtc_rtos_get_datetime(rtc_rtos_handle_t *handle, rtc_datetime_t *dt) {
	if (handle == NULL || dt == NULL) {
		return kStatus_InvalidArgument;
	}

	if (xSemaphoreTake(handle->mutex, handle->ticks_to_wait) != pdTRUE) {
		return kStatus_Fail;
	}

	RTC_GetDatetime(RTC, dt);

	(void) xSemaphoreGive(handle->mutex);

	return kStatus_Success;
}

/* -------------------------------------------------------------------------- */

status_t rtc_rtos_arm_alarm_1s(rtc_rtos_handle_t *handle) {
	if (handle == NULL) {
		return kStatus_InvalidArgument;
	}

	if (xSemaphoreTake(handle->mutex, handle->ticks_to_wait) != pdTRUE) {
		return kStatus_Fail;
	}

	uint32_t next_sec = RTC_GetSecondsTimerCount(RTC) + 1U;
	RTC_SetSecondsTimerMatch(RTC, next_sec);

	(void) xSemaphoreGive(handle->mutex);

	return kStatus_Success;
}

/* -------------------------------------------------------------------------- */

status_t rtc_rtos_wait_alarm(rtc_rtos_handle_t *handle) {
	if (handle == NULL) {
		return kStatus_InvalidArgument;
	}

	/* Block the calling task until the ISR gives the semaphore */
	if (xSemaphoreTake(handle->semaphore, handle->ticks_to_wait) != pdTRUE) {
		return kStatus_Fail;
	}

	return kStatus_Success;
}

/* -------------------------------------------------------------------------- */

status_t rtc_rtos_update_time(rtc_rtos_handle_t *handle, char *str,
		uint8_t *len, rtc_datetime_t *dt) {
	if (handle == NULL || str == NULL || len == NULL || dt == NULL) {
		return kStatus_InvalidArgument;
	}

	/* Arm the 1-second alarm, then block until it fires */
	status_t st = rtc_rtos_arm_alarm_1s(handle);
	if (st != kStatus_Success) {
		return st;
	}

	st = rtc_rtos_wait_alarm(handle);
	if (st != kStatus_Success) {
		return st;
	}

	/* Read and format the time */
	st = rtc_rtos_get_datetime(handle, dt);
	if (st != kStatus_Success) {
		return st;
	}

	format_time(str, len, dt);

	return kStatus_Success;
}

/* -------------------------------------------------------------------------- */

status_t rtc_rtos_update_date(rtc_rtos_handle_t *handle, char *str,
		uint8_t *len, const rtc_datetime_t *dt) {
	if (handle == NULL || str == NULL || len == NULL || dt == NULL) {
		return kStatus_InvalidArgument;
	}

	if (xSemaphoreTake(handle->mutex, handle->ticks_to_wait) != pdTRUE) {
		return kStatus_Fail;
	}

	status_t result;

	if (handle->cached_day != dt->day) {
		handle->cached_day = dt->day;
		(void) xSemaphoreGive(handle->mutex);

		format_date(str, len, dt);
		result = kStatus_Success;
	} else {
		(void) xSemaphoreGive(handle->mutex);
		result = kStatus_NoData; /* Day unchanged – no new string produced */
	}

	return result;
}

/* -------------------------------------------------------------------------- */

void rtc_rtos_format_next_year(char *str, uint8_t *len,
		const rtc_datetime_t *dt) {
	rtc_datetime_t next = *dt; /* local copy – do not modify caller's struct */
	next.year += 1U;

	/* Edge case: 29 Feb on a non-leap year */
	if (next.month == 2U && next.day == 29U) {
		bool leap = ((next.year % 4U == 0U && next.year % 100U != 0U)
				|| (next.year % 400U == 0U));
		if (!leap) {
			next.day = 28U;
		}
	}

	format_date(str, len, &next);
}

