/*
 * app_saksm.c
 *
 *  Created on: 19/03/2026
 *      Author: plibreros
 */
#include "app_saksm.h"
#include "process_extinction.h"
#include "process_uart.h"
#include "process_lcd.h"
#include "process_rtc.h"

#include "FreeRTOS.h"
#include "task.h"

/////////////////////////PRUEBA//////////////////////////////////////
//#include "mcp960x.h"

//static void init_dummie(void);
//static void task_dummie(void *pvParameters);

//void init_dummie(void) {
//	BaseType_t res = xTaskCreate(task_dummie, "task_dummie",
//			(configMINIMAL_STACK_SIZE * 2),
//			NULL, (tskIDLE_PRIORITY + 3),
//			NULL);
//	configASSERT(pdPASS == res);

//}
//
//static void task_dummie(void *pvParameters) {
//#ifdef DUMMIE_TASK_MCP
//	_mcp960x_device device;
//	i2c_rtos_t channel_i2c;
//	float Temperature = 0.0f;
//	char temp_value_str[5] = {0};
//	char msg[50] = {0};
//
//	mcp960x_get_default_config(&device);
//	device.id = SENSOR067;
//	channel_i2c.master_rtos_handle.ticks_to_wait_ms = 10;
//
//	if(mcp960x_channel_i2c_init(&channel_i2c, I2C4) != MCP960x_Success){
//		PRINTF("\r\n[task_dummie] mcp960x_channel_i2c_init fail.");
//		vTaskSuspend(NULL);
//	}
//
//	if(mcp960x_device_init(&device, &channel_i2c) != MCP960x_Success){
//		PRINTF("\r\n[task_dummie] mcp960x_device_init fail.");
//		vTaskSuspend(NULL);
//	}
//
//	for(;;){
//		if(mcp960x_read_thermocouple(&device, &channel_i2c) != MCP960x_Success){
//			PRINTF("\r\n[task_dummie] mcp960x_read_thermocouple fail.");
//		}else{
//			(void)memset(temp_value_str, 0, sizeof(temp_value_str));
//			(void)memset(msg, 0, sizeof(msg));
//			if((device.hot_temp_upper_byte & 0x80) == 0x80){										/*When the temperature is lower than 0°*/
//				Temperature = (float)((device.hot_temp_upper_byte * 16.0f) + (device.hot_temp_lower_byte / 16.0f) - 4096.0f);
//			}else{																	/*Otherwise if the temperature is greater than 0°*/
//				Temperature = (float)((device.hot_temp_upper_byte * 16.0f) + (device.hot_temp_lower_byte / 16.0f));
//			}
//			float_to_str_1decimal(Temperature, temp_value_str);
//			snprintf(msg, sizeof(msg), "\r\n[task_dummie] Temperature : %s", temp_value_str);
//			PRINTF("%s", msg);
//		}
//		vTaskDelay(100);
//	}
//#endif
//}
/////////////////////////PRUEBA//////////////////////////////////////
void app_saks_mod_init(void) {
//	init_dummie();

	ao_process_lcd_init();
	ao_process_uart_init();
	ao_process_rtc_init();
	ao_process_extinction_init();
	vTaskStartScheduler();
}
