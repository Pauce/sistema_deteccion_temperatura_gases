/*
 * wifible_uart_transport.h
 *
 *  Created on: 21/04/2026
 *      Author: plibreros
 */

#ifndef PROTOCOLS_INC_WIFI_BLE_UART_TRANSPORT_H_
#define PROTOCOLS_INC_WIFI_BLE_UART_TRANSPORT_H_

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "FreeRTOS.h"
#include "task.h"

#include "uart_freertos.h"
#include "wifi_ble_uart_frame.h"    /* solo para WIFI_BLE_FRAME_MAX_LEN y bytes especiales */

/* ---------------------------------------------------------------------------
 * Configuración de timeouts (en ticks FreeRTOS)
 * ------------------------------------------------------------------------- */

/**
 * Ticks máximos esperando la notificación de la ISR tras enviar el frame.
 * A 115200 bps un frame de ~50 bytes demora ~4 ms; 20 ms es generoso.
 */
#ifndef MQTT_TRANSPORT_RX_TIMEOUT_TICKS
#define MQTT_TRANSPORT_RX_TIMEOUT_TICKS     pdMS_TO_TICKS(20U)
#endif

/* ---------------------------------------------------------------------------
 * Contexto de transporte
 * ------------------------------------------------------------------------- */

/**
 * @brief Contexto de una sesión de transporte MQTT-UART.
 *
 * Instanciar en RAM estática; inicializar con mqtt_transport_init().
 */
typedef struct {
	uart_drv_handle_t *drv; /**< Handle del driver UART.         */
	uint32_t notify_bit; /**< Bit de notificación del driver. */

	/* Buffer de recepción desescapado. */
	uint8_t rx_buf[WIFI_BLE_FRAME_MAX_LEN];
	size_t rx_len; /**< Bytes válidos en rx_buf.        */
} mqtt_transport_ctx_t;

/* ---------------------------------------------------------------------------
 * Códigos de retorno
 * ------------------------------------------------------------------------- */

typedef enum {
	kTransport_Ok = 0, kTransport_ErrParam = -1, kTransport_ErrTx = -2, /**< uart_drv_write falló.               */
	kTransport_ErrTimeout = -3, /**< No llegó respuesta a tiempo.        */
	kTransport_ErrFrame = -4, /**< Frame recibido mal formado.         */
} transport_status_t;

/* ---------------------------------------------------------------------------
 * API
 * ------------------------------------------------------------------------- */

/**
 * @brief Inicializa el contexto de transporte.
 *
 * @param ctx           Contexto a inicializar.
 * @param drv           Handle del driver UART ya inicializado.
 * @param notify_bit    Bit de notificación configurado en el driver.
 */
transport_status_t mqtt_transport_init(mqtt_transport_ctx_t *ctx,
		uart_drv_handle_t *drv, uint32_t notify_bit);

/**
 * @brief Envía @p frame y espera una respuesta completa (STX…ETX).
 *
 * El frame recibido se desescapa y queda en ctx->rx_buf / ctx->rx_len.
 * Usa xTaskNotifyWait(): la tarea llamante se bloquea hasta recibir datos
 * o hasta que expire MQTT_TRANSPORT_RX_TIMEOUT_TICKS.
 *
 * @param ctx           Contexto de transporte.
 * @param frame         Frame listo para enviar (salida de mqtt_frame_build_*).
 * @param frame_len     Longitud del frame.
 * @return kTransport_Ok o código de error.
 */
transport_status_t mqtt_transport_transfer(mqtt_transport_ctx_t *ctx,
		const uint8_t *frame, size_t frame_len);

#ifdef __cplusplus
}
#endif

#endif /* PROTOCOLS_INC_WIFI_BLE_UART_TRANSPORT_H_ */
