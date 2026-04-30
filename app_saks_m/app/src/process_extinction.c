/*
 * process_extinction.c
 *
 *  Created on: 28/04/2026
 *      Author: plibreros
 */
#include "process_extinction.h"
#include "process_lcd.h"
#include "fsm_gas.h"

#include "FreeRTOS.h"
#include "portable.h"
#include "semphr.h"
#include "fsl_debug_console.h"

#define QUEUE_EXTINCTION_LENGTH 10
#define EXTINCTION_TASK_PRIORITY (tskIDLE_PRIORITY + 3)

typedef struct {
	TaskHandle_t id_extinction_task;
	char *task_name;
} task_extinction_params_t;

/* --- Variables globales --- */
static QueueHandle_t hqueue_extinction = NULL;
static task_extinction_params_t id_task = { 0 };
static gas_fsm_context_t co_sensor;
static gas_fsm_context_t ch4_sensor;

/* --- Prototipos --- */
static void task_extinction(void *args);

/* --- Implementación --- */
static void task_extinction(void *args) {

	data_sensors_t sensor;
	gas_fsm_init(&co_sensor, (uint16_t)(100U), TYPE_MONOXIDE);
	gas_fsm_init(&ch4_sensor, (uint16_t)(100U), TYPE_METHANE);

	for (;;) {
		if (xQueueReceive(hqueue_extinction, &sensor, portMAX_DELAY) == pdPASS) {
			switch (sensor.sensor) {
			case TYPE_MONOXIDE:
				gas_fsm_run(&co_sensor, sensor.value.gas_sensor);
				break;
			case TYPE_METHANE:
				gas_fsm_run(&ch4_sensor, sensor.value.gas_sensor);
				break;
			case TYPE_THERMOCOUPLE:
			default:
				break;
			}
		}
	}
}

void ao_process_extinction_init(void) {
	id_task.task_name = (char*) ("task_extinction");

	hqueue_extinction = xQueueCreate(QUEUE_EXTINCTION_LENGTH,
			sizeof(data_sensors_t));
	configASSERT(hqueue_extinction != NULL);

	BaseType_t res = xTaskCreate(task_extinction, id_task.task_name,
			(configMINIMAL_STACK_SIZE * 3),
			NULL,
			EXTINCTION_TASK_PRIORITY, &id_task.id_extinction_task);

	configASSERT(res == pdPASS);
}

bool ao_process_extinction_queue_send(data_sensors_t *msg) {
	return xQueueSend(hqueue_extinction, msg, 0) == pdPASS;
}
