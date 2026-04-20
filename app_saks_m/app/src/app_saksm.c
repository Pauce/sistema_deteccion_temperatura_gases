/*
 * app_saksm.c
 *
 *  Created on: 19/03/2026
 *      Author: plibreros
 */
#include "app_saksm.h"

#include "fsl_debug_console.h"
#include "FreeRTOS.h"
#include "task.h"

//#define DUMMIE_TASK_UART
//#define DUMMIE_TASK_MCP
#define DUMMIE_TASK_CAN

/////////////////////////PRUEBA//////////////////////////////////////
//#include "winstar_canopen.h"
//#include "mcp960x.h"
//#include "i2c_freertos.h"
//#include "uart_freertos.h"
//#include "ZC05.h"
//#include "ZE07.h"
//#include "winsen.h"
#include "WL0F0007000A8GAAASA00.h"

static void init_dummie(void);
static void task_dummie(void * pvParameters);

#ifdef DUMMIE_TASK_UART

static TaskHandle_t dummie_task_handle;

#endif


void init_dummie(void){

#ifndef DUMMIE_TASK_UART
	BaseType_t res = xTaskCreate(
						task_dummie,
						"task_dummie",
						(configMINIMAL_STACK_SIZE * 2),
						NULL,
						(tskIDLE_PRIORITY + 3),
						NULL
					);
#endif
#ifdef DUMMIE_TASK_UART
	BaseType_t res = xTaskCreate(
						task_dummie,
						"task_dummie",
						(configMINIMAL_STACK_SIZE + 80),
						NULL,
						(tskIDLE_PRIORITY + 3),
						&dummie_task_handle
					);
#endif
	configASSERT(pdPASS == res);
#ifdef DUMMIE_TASK_UART
	zc05_init(dummie_task_handle);
	ze07_init(dummie_task_handle);
#endif
}

static void task_dummie(void * pvParameters){

#ifdef DUMMIE_TASK_UART

	(void)pvParameters;

	for (;;) {

		uint32_t notif_val = 0U;

	    xTaskNotifyWait(
	            0x00000000U,            /* No limpiar bits a la entrada        */
	            UART_DRV_NOTIFY_BIT_0 |
	            UART_DRV_NOTIFY_BIT_1,  /* Limpiar ambos bits al salir         */
	            &notif_val,
	            pdMS_TO_TICKS(500)
	    );

	    winsen_data_t *zc05 = zc05_get_data(notif_val);

        if(zc05 != NULL){
   	        PRINTF("Gas type: %d\n\r", zc05->gas_type);
   	        PRINTF("Concentration: %d ppm\n\r", zc05->concentration);
   	        PRINTF("Full range: %d ppm\n\r", zc05->full_range);
   	        PRINTF("Fault: %s\n\r", zc05->fault ? "YES" : "NO");
   	        PRINTF("\r\n");
        }

        winsen_data_t *ze07 = ze07_get_data(notif_val);

        if(ze07 != NULL){
            PRINTF("Gas type: %d\n\r", ze07->gas_type);
            PRINTF("Concentration: %d ppm\n\r", ze07->concentration);
            PRINTF("Full range: %d ppm\n\r", ze07->full_range);
            PRINTF("Fault: %s\n\r", ze07->fault ? "YES" : "NO");
            PRINTF("\r\n");
        }
	}

#endif

#ifdef DUMMIE_TASK_MCP
	_mcp960x_device device;
	i2c_rtos_t channel_i2c;
	float Temperature = 0.0f;
	char temp_value_str[5] = {0};
	char msg[50] = {0};

	mcp960x_get_default_config(&device);
	device.id = SENSOR067;
	channel_i2c.master_rtos_handle.ticks_to_wait_ms = 10;

	if(mcp960x_channel_i2c_init(&channel_i2c, I2C4) != MCP960x_Success){
		PRINTF("\r\n[task_dummie] mcp960x_channel_i2c_init fail.");
		vTaskSuspend(NULL);
	}

	if(mcp960x_device_init(&device, &channel_i2c) != MCP960x_Success){
		PRINTF("\r\n[task_dummie] mcp960x_device_init fail.");
		vTaskSuspend(NULL);
	}

	for(;;){
		if(mcp960x_read_thermocouple(&device, &channel_i2c) != MCP960x_Success){
			PRINTF("\r\n[task_dummie] mcp960x_read_thermocouple fail.");
		}else{
			(void)memset(temp_value_str, 0, sizeof(temp_value_str));
			(void)memset(msg, 0, sizeof(msg));
			if((device.hot_temp_upper_byte & 0x80) == 0x80){										/*When the temperature is lower than 0°*/
				Temperature = (float)((device.hot_temp_upper_byte * 16.0f) + (device.hot_temp_lower_byte / 16.0f) - 4096.0f);
			}else{																	/*Otherwise if the temperature is greater than 0°*/
				Temperature = (float)((device.hot_temp_upper_byte * 16.0f) + (device.hot_temp_lower_byte / 16.0f));
			}
			float_to_str_1decimal(Temperature, temp_value_str);
			snprintf(msg, sizeof(msg), "\r\n[task_dummie] Temperature : %s", temp_value_str);
			PRINTF("%s", msg);
		}
		vTaskDelay(100);
	}
#endif
#ifdef DUMMIE_TASK_CAN
	if(!display_7inch_can_init()){
		PRINTF("\r\n[task_dummie] Display init error.");
		vTaskSuspend(NULL);
	}else{
		PRINTF("\r\n[task_dummie] Display init ok.");
	}

	vTaskDelay(1000);
	const uint32_t init_value = 0;
	uint32_t counter = init_value;

	for(;;){

		display_7inch_print(display_7inch_Temp_Value1, DISPLAY_TYPE_VALUE_INTEGER, &counter);
		counter++;

		vTaskDelay(pdMS_TO_TICKS(300));
	}
#endif
}
/////////////////////////PRUEBA//////////////////////////////////////
void app_saks_mod_init(void){
	init_dummie();
	vTaskStartScheduler();
}
