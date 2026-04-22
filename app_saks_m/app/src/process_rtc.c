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

#define QUEUE_RTC_LENGTH			10
#define RTC_TASK_PRIORITY			(tskIDLE_PRIORITY + 2)

typedef struct {
	TaskHandle_t id_rtc_task;
	char *task_name;
} _task_rtc_params_t;

static QueueHandle_t hqueue_rtc = NULL;
static _task_rtc_params_t id_task;

static void task_rtc(void *args);

static void task_rtc(void *args) {
	data_rtc_t ntp_data = { 0 };
	rtc_rtos_handle_t g_rtc = NULL;

	if (xQueueReceive(hqueue_rtc, &ntp_data, portMAX_DELAY) != pdPASS)
		vTaskSuspend(NULL);

	_status_rtc status = rtc_device_init(&g_rtc, &ntp_data.data_time);

	if (status != RTC_Status_Ok)
		vTaskSuspend(NULL);
//	char          time_str[RTC_TIME_STR_LEN];
//	    uint8_t       time_len = RTC_TIME_STR_LEN;

	/*Inicializar rtc interno*/

	for (;;) {
		/*Actualizar hora y enviar a lcd*/
	}
}

void ao_process_rtc_init(void) {
	memset(&id_task, 0, sizeof(_task_rtc_params_t));
	id_task.task_name = (char*) ("task_rtc");

	hqueue_rtc = xQueueCreate(QUEUE_RTC_LENGTH, sizeof(data_rtc_t));
	configASSERT(hqueue_rtc != NULL);

	BaseType_t res = xTaskCreate(task_rtc, id_task.task_name,
			(configMINIMAL_STACK_SIZE * 2),
			NULL,
			RTC_TASK_PRIORITY, &id_task.id_rtc_task);
	configASSERT(res == pdPASS);
}

bool ao_process_rtc_queue_send(data_rtc_t *msg) {
	return xQueueSend(hqueue_rtc, msg, 0) == pdPASS;
}
