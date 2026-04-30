/*
 * process_lcd.c
 *
 *  Created on: 21/04/2026
 *      Author: plibreros
 */
#include "process_lcd.h"
#include "WL0F0007000A8GAAASA00.h"

#include "FreeRTOS.h"
#include "portable.h"
#include "semphr.h"
#include "fsl_debug_console.h"

#define QUEUE_LCD_LENGTH			10
#define LCD_TASK_PRIORITY			(tskIDLE_PRIORITY + 2)
#define LCD_WAIT_IDLE				1000

typedef struct {
	TaskHandle_t id_lcd_task;
	char *task_name;
} task_lcd_params_t;

static QueueHandle_t hqueue_lcd = NULL;
static task_lcd_params_t id_task;

/*-------------------------------- PRIVATE PROTOT -------------------------------*/
static void task_lcd(void *args);
/*-------------------------------- PRIVATE METHODS -------------------------------*/
static void task_lcd(void *args) {
	if (!display_7inch_can_init())
		vTaskSuspend(NULL);

	vTaskDelay(pdMS_TO_TICKS((TickType_t)LCD_WAIT_IDLE));

	data_print_lcd_t data_print = { 0 };

	for (;;) {
		if (xQueueReceive(hqueue_lcd, &data_print, portMAX_DELAY) == pdPASS) {
			display_7inch_print(&data_print);
		}
	}
}

/*-------------------------------- PUBLIC METHODS -------------------------------*/
void ao_process_lcd_init(void) {

	memset(&id_task, 0, sizeof(task_lcd_params_t));
	id_task.task_name = (char*) ("task_lcd");

	hqueue_lcd = xQueueCreate(QUEUE_LCD_LENGTH, sizeof(data_print_lcd_t));
	configASSERT(hqueue_lcd != NULL);

	BaseType_t res = xTaskCreate(task_lcd, id_task.task_name,
			(configMINIMAL_STACK_SIZE * 3),
			NULL,
			LCD_TASK_PRIORITY, &id_task.id_lcd_task);

	configASSERT(res == pdPASS);
}

bool ao_process_lcd_queue_send(data_print_lcd_t *msg) {
	return xQueueSend(hqueue_lcd, msg, 0) == pdPASS;
}
