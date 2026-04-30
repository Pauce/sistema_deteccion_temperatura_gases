/*
 * process_rtc.c
 *
 *  Created on: 21/04/2026
 *      Author: plibreros
 */
#include "process_rtc.h"
#include "rtc_device.h"
#include "process_lcd.h"

#include "FreeRTOS.h"
#include "portable.h"
#include "semphr.h"
#include "fsl_debug_console.h"

#define TIME_DATE_SIZE_BUFFER   30
#define QUEUE_RTC_LENGTH        10
#define MAX_WAIT_DELAY_RTC      1000
#define RTC_TASK_PRIORITY       (tskIDLE_PRIORITY + 2)

typedef struct {
	TaskHandle_t id_rtc_task;
	char *task_name;
} _task_rtc_params_t;

static data_print_lcd_t lcd_printer_time;
static data_print_lcd_t lcd_printer_date;

//static data_rtc_t ntp_data = { 0 };
static rtc_rtos_handle_t g_rtc;
static rtc_datetime_t dt;

static char time_str[TIME_DATE_SIZE_BUFFER];
static char date_str[TIME_DATE_SIZE_BUFFER];
uint8_t time_str_len = TIME_DATE_SIZE_BUFFER;
uint8_t date_str_len = TIME_DATE_SIZE_BUFFER;

static QueueHandle_t hqueue_rtc = NULL;
static _task_rtc_params_t id_task;

static void task_rtc(void *args);

static void task_rtc(void *args) {
	(void) args;

	memset(&lcd_printer_time, 0, sizeof(data_print_lcd_t));
	memset(&lcd_printer_date, 0, sizeof(data_print_lcd_t));
	memset(time_str, 0, sizeof(time_str));
	memset(date_str, 0, sizeof(date_str));

	rtc_datetime_t initial_dt = { .year = 2026, .month = 4, .day = 21,
			.hour = 8, .minute = 0, .second = 0, };

	_status_rtc status = rtc_device_init(&g_rtc, &initial_dt);

	if (status != RTC_Status_Ok) {
		PRINTF("[task_rtc] ERROR: rtc_device_init falló (status=%d)\r\n",
				(int) status);
		vTaskSuspend(NULL);
	}

	TickType_t xLastWakeTime = xTaskGetTickCount();
	const TickType_t xFrequency = pdMS_TO_TICKS(MAX_WAIT_DELAY_RTC);

	for (;;) {

		bool new_time = rtc_device_update_time(&g_rtc, time_str, &time_str_len, &dt)
						== RTC_Status_Ok;
		bool new_date = rtc_device_update_date(&g_rtc, date_str, &date_str_len, &dt)
						== RTC_Status_Ok;
		if (new_time) {
			lcd_printer_time._index = display_7inch_Time;
			lcd_printer_time._typ = DISPLAY_TYPE_VALUE_STRING;
			strncpy(lcd_printer_time.value.str, time_str, time_str_len);

            if (!ao_process_lcd_queue_send(&lcd_printer_time)) {
                PRINTF("[task_rtc] WARN: queue LCD llena (time)\r\n");
            }
		}

		if (new_date) {
			lcd_printer_date._index = display_7inch_Date;
			lcd_printer_date._typ = DISPLAY_TYPE_VALUE_STRING;
			strncpy(lcd_printer_date.value.str, date_str, date_str_len);

            if (!ao_process_lcd_queue_send(&lcd_printer_date)) {
                PRINTF("[task_rtc] WARN: queue LCD llena (date)\r\n");
            }
		}

		vTaskDelayUntil(&xLastWakeTime, xFrequency);
	}
}

void ao_process_rtc_init(void) {

	memset(&id_task, 0, sizeof(_task_rtc_params_t));
	id_task.task_name = (char*) ("task_rtc");

	hqueue_rtc = xQueueCreate(QUEUE_RTC_LENGTH, sizeof(data_rtc_t));
	configASSERT(hqueue_rtc != NULL);

	BaseType_t res = xTaskCreate(task_rtc, id_task.task_name,
			(configMINIMAL_STACK_SIZE * 3),
			NULL,
			RTC_TASK_PRIORITY, &id_task.id_rtc_task);
	configASSERT(res == pdPASS);
}

bool ao_process_rtc_queue_send(data_rtc_t *msg) {
	return xQueueSend(hqueue_rtc, msg, 0) == pdPASS;
}
