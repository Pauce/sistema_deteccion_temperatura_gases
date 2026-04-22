/*
 * ZE07.c
 *
 *  Created on: 17/04/2026
 *      Author: plibreros
 */
#include "ZC05.h"
#include "FreeRTOS.h"

#define UART2_THRESHOLD		9U
#define UART2_BAUD			9600U
#define UART_IRQ_PRIORITY   5U
#define ZE07_NOTIFY			UART_DRV_NOTIFY_BIT_2

static uart_drv_handle_t g_uart2;
static winsen_data_t ze07_data;

/*Method for handle ISR FLEXCOMM*/
void FLEXCOMM2_IRQHandler(void) {
	uart_drv_isr_handler(&g_uart2);
}

void ze07_init(TaskHandle_t notify_task_handle) {
	uart_drv_status_t status;
	/* --- Instancia 1: USART1 / FLEXCOMM1 --- */
	uart_drv_config_t cfg1 = { .base = USART2, .src_clk_hz =
			CLOCK_GetFlexCommClkFreq(2U), .baud_rate = UART2_BAUD, .task =
			notify_task_handle, .notify_bit = UART_DRV_NOTIFY_BIT_2,
			.threshold = UART2_THRESHOLD, .irq = FLEXCOMM2_IRQn, .irq_prio =
					UART_IRQ_PRIORITY, };
	status = uart_drv_init(&g_uart2, &cfg1);
	configASSERT(status == kUartDrv_Ok);
}

winsen_data_t* ze07_get_data(uint32_t notif_val) {
	if (!(notif_val & ZE07_NOTIFY) || (uart_drv_available(&g_uart2) == 0))
		return NULL;

	static uint8_t data_raw[UART_DRV_BUF_SIZE] = { 0 };

	size_t len_data_raw = uart_drv_read(&g_uart2, data_raw, sizeof(data_raw));

	if (data_raw[(len_data_raw - 1)] != winsen_checksum(data_raw))
		return NULL;

	memset(&ze07_data, 0, sizeof(winsen_data_t));
	winsen_status_t status = winsen_parse_frame(WINSEN_SENSOR_ZE07, data_raw,
			&ze07_data);

	if (status != WINSEN_OK)
		return NULL;

	return &ze07_data;
}

