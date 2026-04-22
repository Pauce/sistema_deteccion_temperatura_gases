/**
 * @file wifi_ble_uart_transport.c
 * @brief Implementación de la capa de transporte MQTT-UART.
 *
 * Diseño de recepción:
 *  El driver uart_drv notifica a la tarea cuando recibe >= threshold bytes.
 *  Esta capa NO hace polling: llama a xTaskNotifyWait y luego drena el ring
 *  buffer byte a byte construyendo el frame de respuesta.
 *
 *  Máquina de estados de recepción (privada, por llamada):
 *    IDLE       → espera STX
 *    IN_FRAME   → acumula bytes hasta ETX
 *    ESC_NEXT   → el siguiente byte está escapado (precede ESC)
 */

#include "wifi_ble_uart_transport.h"

/* ---------------------------------------------------------------------------
 * Estados del receptor
 * ------------------------------------------------------------------------- */

typedef enum {
	RX_IDLE, RX_IN_FRAME, RX_ESC_NEXT, RX_DONE, RX_ERROR,
} rx_state_t;

/* ---------------------------------------------------------------------------
 * API pública
 * ------------------------------------------------------------------------- */

transport_status_t mqtt_transport_init(mqtt_transport_ctx_t *ctx,
		uart_drv_handle_t *drv, uint32_t notify_bit) {
	if (ctx == NULL || drv == NULL || notify_bit == 0U) {
		return kTransport_ErrParam;
	}

	ctx->drv = drv;
	ctx->notify_bit = notify_bit;
	ctx->rx_len = 0U;

	return kTransport_Ok;
}

transport_status_t mqtt_transport_transfer(mqtt_transport_ctx_t *ctx,
		const uint8_t *frame, size_t frame_len) {
	if (ctx == NULL || frame == NULL || frame_len == 0U) {
		return kTransport_ErrParam;
	}

	/* --- 1. Limpiar buffer de RX y enviar -------------------------------- */
	uart_drv_flush(ctx->drv);
	ctx->rx_len = 0U;

	uart_drv_status_t tx_st = uart_drv_write(ctx->drv, frame, frame_len);
	if (tx_st != kUartDrv_Ok) {
		return kTransport_ErrTx;
	}

	/* --- 2. Esperar notificación del driver (ISR → xTaskNotifyFromISR) --- */
	uint32_t notified_val = 0U;
	BaseType_t got_notify = xTaskNotifyWait(0U, /* No limpiar bits al entrar.  */
	ctx->notify_bit, /* Limpiar nuestro bit al salir.*/
	&notified_val, MQTT_TRANSPORT_RX_TIMEOUT_TICKS);

	if (got_notify == pdFALSE || (notified_val & ctx->notify_bit) == 0U) {
		return kTransport_ErrTimeout;
	}

	/* --- 3. Drena el ring buffer y construye el frame desescapado -------- */
	rx_state_t state = RX_IDLE;
	size_t wr_idx = 0U;
	uint8_t byte;

	/*
	 * Leemos byte a byte. Si el driver notificó con threshold = 1 (valor
	 * recomendado para este protocolo), todos los bytes ya están disponibles.
	 * Si el threshold fuera mayor, el bucle agota lo disponible y vuelve a
	 * esperar otra notificación antes de declarar timeout.
	 *
	 * Para mantener esta capa simple, se asume threshold = 1 o que el frame
	 * completo llegó en la ventana de timeout. Si se necesita recepción
	 * incremental, ampliar con un segundo xTaskNotifyWait interno.
	 */
	while (uart_drv_read(ctx->drv, &byte, 1U) == 1U) {

		switch (state) {

		case RX_IDLE:
			if (byte == WIFIBLEUART_STX) {
				if (wr_idx < WIFI_BLE_FRAME_MAX_LEN) {
					ctx->rx_buf[wr_idx++] = byte;
				}
				state = RX_IN_FRAME;
			}
			break;

		case RX_IN_FRAME:
			if (byte == WIFIBLEUART_ETX) {
				/* Fin del frame: no se almacena el ETX. */
				state = RX_DONE;
			} else if (byte == WIFIBLEUART_ESC) {
				state = RX_ESC_NEXT;
			} else {
				if (wr_idx >= WIFI_BLE_FRAME_MAX_LEN) {
					state = RX_ERROR;
					break;
				}
				ctx->rx_buf[wr_idx++] = byte;
			}
			break;

		case RX_ESC_NEXT:
			/* El byte precedido por ESC se almacena tal cual. */
			if (wr_idx >= WIFI_BLE_FRAME_MAX_LEN) {
				state = RX_ERROR;
				break;
			}
			ctx->rx_buf[wr_idx++] = byte;
			state = RX_IN_FRAME;
			break;

		default:
			break;
		}

		if (state == RX_DONE || state == RX_ERROR) {
			break;
		}
	}

	if (state != RX_DONE || wr_idx == 0U) {
		ctx->rx_len = 0U;
		return kTransport_ErrFrame;
	}

	ctx->rx_len = wr_idx;
	return kTransport_Ok;
}
