/*
 * wifi_ble_uart_frame.h
 *
 *  Created on: 21/04/2026
 *      Author: plibreros
 */

#ifndef PROTOCOLS_INC_WIFI_BLE_UART_FRAME_H_
#define PROTOCOLS_INC_WIFI_BLE_UART_FRAME_H_

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

/* ---------------------------------------------------------------------------
 * Constantes del protocolo
 * ------------------------------------------------------------------------- */

#define WIFIBLEUART_STX            0xFFU
#define WIFIBLEUART_ESC            0x1BU
#define WIFIBLEUART_ETX            0xFEU
#define WIFIBLEUART_PROC_OK        0xFDU
#define WIFIBLEUART_PROC_ERR       0xFCU

/** Tamaño máximo del buffer de un frame (con ESC expandido). */
#define WIFI_BLE_FRAME_MAX_LEN      50U

/** Número de comandos definidos en el protocolo. */
#define WIFI_BLE_CMD_COUNT          11U

/* ---------------------------------------------------------------------------
 * Tabla de comandos (índice = CmdWiFi)
 * ------------------------------------------------------------------------- */

/** Identificadores de comando (coinciden con el enum original CmdWiFi). */
typedef enum {
	MQTT_CMD_RESET = 0,
	MQTT_CMD_VERSION,
	MQTT_CMD_IS_CONNECTED,
	MQTT_CMD_HAVE_NTP,
	MQTT_CMD_SEND_MQTT,
	MQTT_CMD_GET_SETTINGS_W,
	MQTT_CMD_SET_SETTINGS_W,
	MQTT_CMD_GET_SETTINGS_M,
	MQTT_CMD_SET_SETTINGS_M,
	MQTT_CMD_GET_LOCAL_TIME,
	MQTT_CMD_GET_SETPOINTS,

	MQTT_CMD_UNKNOWN = 0xFFU
} mqtt_cmd_id_t;

/* ---------------------------------------------------------------------------
 * Resultado de operaciones de framing
 * ------------------------------------------------------------------------- */

typedef enum {
	kFrame_Ok = 0, kFrame_ErrParam = -1, /**< Argumento NULL o buffer insuficiente.  */
	kFrame_ErrCRC = -2, /**< CRC incorrecto al parsear.             */
	kFrame_ErrNoCmd = -3, /**< Comando desconocido en la respuesta.   */
	kFrame_ErrOverflow = -4, /**< Frame demasiado largo para el buffer.  */
} frame_status_t;

/* ---------------------------------------------------------------------------
 * Primitivas de CRC y ESC
 * ------------------------------------------------------------------------- */

/**
 * @brief Calcula CRC-8 (polinomio 0x31, init 0xFF).
 *
 * Para verificar: pasar el frame completo (incluyendo byte de CRC al final);
 * el resultado debe ser 0x00.
 */
uint8_t mqtt_frame_crc8(const uint8_t *data, size_t len);

/**
 * @brief Aplica byte-stuffing ESC sobre @p src y escribe en @p dst.
 *
 * El primer byte (STX) se copia sin ESC. Cada byte igual a STX, ESC o ETX
 * en el payload se precede con ESC. Agrega ETX al final.
 *
 * @param[out] dst          Buffer de salida.
 * @param[in]  dst_max      Capacidad del buffer de salida.
 * @param[in]  src          Frame sin ESC (debe comenzar con STX).
 * @param[in]  src_len      Longitud de @p src.
 * @return Longitud del frame con ESC, o negativo si @p dst_max insuficiente.
 */
int32_t mqtt_frame_stuff(uint8_t *dst, size_t dst_max, const uint8_t *src,
		size_t src_len);

/* ---------------------------------------------------------------------------
 * Constructores de frames
 * ------------------------------------------------------------------------- */

/**
 * @brief Construye un frame de comando básico (sin payload de datos).
 *
 * Formato interno: STX | CMD[0] | CMD[1] | 0x00 | CRC
 * Luego se aplica ESC y se añade ETX.
 *
 * @param[in]  cmd          Identificador del comando.
 * @param[out] out          Buffer de salida (mín. WIFI_BLE_FRAME_MAX_LEN bytes).
 * @param[in]  out_max      Capacidad del buffer de salida.
 * @return Longitud del frame listo para enviar, o negativo si error.
 */
int32_t mqtt_frame_build_basic(mqtt_cmd_id_t cmd, uint8_t *out, size_t out_max);

/**
 * @brief Construye el frame de envío de datos MQTT (SM).
 *
 * Formato interno: STX | CMD | LEN_FLAGS | FLAGS[] | LEN_VALUES | VALUES[] | CRC
 *
 * @param[in]  flags        Array de bytes de flags.
 * @param[in]  flags_len    Longitud del array de flags.
 * @param[in]  values       Array de bytes de valores.
 * @param[in]  values_len   Longitud del array de valores.
 * @param[out] out          Buffer de salida.
 * @param[in]  out_max      Capacidad del buffer de salida.
 * @return Longitud del frame listo para enviar, o negativo si error.
 */
int32_t mqtt_frame_build_send(const uint8_t *flags, uint8_t flags_len,
		const uint8_t *values, uint8_t values_len, uint8_t *out, size_t out_max);

/* ---------------------------------------------------------------------------
 * Parser de frames recibidos
 * ------------------------------------------------------------------------- */

/**
 * @brief Valida CRC e identifica el comando de un frame recibido.
 *
 * @param[in]  data     Frame recibido (ya sin ESC, si el transporte lo removió).
 * @param[in]  len      Longitud del frame.
 * @param[out] cmd_out  Comando identificado; MQTT_CMD_UNKNOWN si no encontrado.
 * @return kFrame_Ok, kFrame_ErrCRC o kFrame_ErrNoCmd.
 */
frame_status_t mqtt_frame_parse(const uint8_t *data, size_t len,
		mqtt_cmd_id_t *cmd_out);
#ifdef __cplusplus
}
#endif

#endif /* PROTOCOLS_INC_WIFI_BLE_UART_FRAME_H_ */
