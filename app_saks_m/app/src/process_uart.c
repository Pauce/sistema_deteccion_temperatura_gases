/*
 * process_uart.c
 *
 *  Created on: 29/04/2026
 *      Author: plibreros
 */
#include "process_uart.h"
#include "process_extinction.h"
#include "ZC05.h"
#include "ZE07.h"

#define UART_TASK_PRIORITY			(tskIDLE_PRIORITY + 3)

typedef struct {
	TaskHandle_t id_uart_task;
	char *task_name;
} task_uart_params_t;

data_sensors_t gas_sensor;

static void task_uart(void *args);

static void task_uart(void *args) {

	memset(&gas_sensor, 0, sizeof(data_sensors_t));

	for (;;) {

		uint32_t notif_val = 0;

		xTaskNotifyWait(0x00000000U, 					/* No limpiar bits a la entrada*/
		UART_DRV_NOTIFY_BIT_0 | UART_DRV_NOTIFY_BIT_1, 	/* Limpiar ambos bits al salir*/
		&notif_val, pdMS_TO_TICKS(500));

		winsen_data_t *zc05 = zc05_get_data(notif_val);

		if(zc05 != NULL){
			gas_sensor.sensor = TYPE_METHANE;
			gas_sensor.value.gas_sensor = *zc05;
			(void)ao_process_extinction_queue_send(&gas_sensor);
		}

		winsen_data_t *ze07 = ze07_get_data(notif_val);

		if(ze07 != NULL){
			gas_sensor.sensor = TYPE_MONOXIDE;
			gas_sensor.value.gas_sensor = *ze07;
			(void)ao_process_extinction_queue_send(&gas_sensor);
		}
	}
}

void ao_process_uart_init(void) {

	task_uart_params_t id_task;
	memset(&id_task, 0, sizeof(task_uart_params_t));
	id_task.task_name = (char*) ("task_uart");

	BaseType_t res = xTaskCreate(task_uart, id_task.task_name,
			(configMINIMAL_STACK_SIZE * 4),
			NULL,
			UART_TASK_PRIORITY, &id_task.id_uart_task);

	configASSERT(res == pdPASS);

	zc05_init(id_task.id_uart_task);
	ze07_init(id_task.id_uart_task);
}
