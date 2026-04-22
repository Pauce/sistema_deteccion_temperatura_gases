/*
 * wifi_ble_uart_frame.c
 *
 *  Created on: 21/04/2026
 *      Author: plibreros
 */
#include "wifi_ble_uart_frame.h"
/* ---------------------------------------------------------------------------
 * Tabla de comandos (cmd_id → string de 2 chars)
 * ------------------------------------------------------------------------- */

static const char s_cmd_table[WIFI_BLE_CMD_COUNT][2] = {
/* MQTT_CMD_RESET          */{ 'R', 'S' },
/* MQTT_CMD_VERSION        */{ 'V', 'S' },
/* MQTT_CMD_IS_CONNECTED   */{ 'I', 'C' },
/* MQTT_CMD_HAVE_NTP       */{ 'I', 'N' },
/* MQTT_CMD_SEND_MQTT      */{ 'S', 'M' },
/* MQTT_CMD_GET_SETTINGS_W */{ 'G', 'W' },
/* MQTT_CMD_SET_SETTINGS_W */{ 'S', 'W' },
/* MQTT_CMD_GET_SETTINGS_M */{ 'G', 'M' },
/* MQTT_CMD_SET_SETTINGS_M */{ 'S', 'M' },
/* MQTT_CMD_GET_LOCAL_TIME */{ 'G', 'T' },
/* MQTT_CMD_GET_SETPOINTS  */{ 'S', 'P' }, };

/* ---------------------------------------------------------------------------
 * CRC-8 (polinomio Dallas/Maxim 0x31, init 0xFF)
 * ------------------------------------------------------------------------- */

uint8_t mqtt_frame_crc8(const uint8_t *data, size_t len) {
	uint8_t crc = 0xFFU;

	for (size_t i = 0U; i < len; i++) {
		crc ^= data[i];
		for (uint8_t j = 0U; j < 8U; j++) {
			if ((crc & 0x80U) != 0U) {
				crc = (uint8_t) ((crc << 1U) ^ 0x31U);
			} else {
				crc <<= 1U;
			}
		}
	}
	return crc;
}

/* ---------------------------------------------------------------------------
 * Byte stuffing (ESC)
 * ------------------------------------------------------------------------- */

int32_t mqtt_frame_stuff(uint8_t *dst, size_t dst_max, const uint8_t *src,
		size_t src_len) {
	if (dst == NULL || src == NULL || src_len == 0U) {
		return (int32_t) kFrame_ErrParam;
	}

	size_t out = 0U;

	/* El primer byte (STX) se copia tal cual, sin ESC. */
	if (out >= dst_max) {
		return (int32_t) kFrame_ErrOverflow;
	}
	dst[out++] = src[0];

	for (size_t i = 1U; i < src_len; i++) {
		uint8_t b = src[i];

		if (b == WIFIBLEUART_ESC || b == WIFIBLEUART_STX || b == WIFIBLEUART_ETX) {
			if ((out + 2U) > dst_max) {
				return (int32_t) kFrame_ErrOverflow;
			}
			dst[out++] = WIFIBLEUART_ESC;
			dst[out++] = b;
		} else {
			if (out >= dst_max) {
				return (int32_t) kFrame_ErrOverflow;
			}
			dst[out++] = b;
		}
	}

	/* Cierre del frame. */
	if (out >= dst_max) {
		return (int32_t) kFrame_ErrOverflow;
	}
	dst[out++] = WIFIBLEUART_ETX;

	return (int32_t) out;
}

/* ---------------------------------------------------------------------------
 * Helper interno: copia un bloque precedido de su longitud (TLV simple)
 * ------------------------------------------------------------------------- */

static inline void priv_add_block(uint8_t **ptr, const uint8_t *data,
		uint8_t len) {
	*(*ptr)++ = len;
	memcpy(*ptr, data, len);
	*ptr += len;
}

/* ---------------------------------------------------------------------------
 * Constructores de frames
 * ------------------------------------------------------------------------- */

int32_t mqtt_frame_build_basic(mqtt_cmd_id_t cmd, uint8_t *out, size_t out_max) {
	if (out == NULL || out_max < WIFI_BLE_FRAME_MAX_LEN) {
		return (int32_t) kFrame_ErrParam;
	}
	if ((uint8_t) cmd >= WIFI_BLE_CMD_COUNT) {
		return (int32_t) kFrame_ErrParam;
	}

	uint8_t tmp[WIFI_BLE_FRAME_MAX_LEN];
	uint8_t *p = tmp;

	*p++ = WIFIBLEUART_STX;
	*p++ = (uint8_t) s_cmd_table[cmd][0];
	*p++ = (uint8_t) s_cmd_table[cmd][1];
	*p++ = 0x00U; /* Sin payload */

	size_t hdr_len = (size_t) (p - tmp);
	*p++ = mqtt_frame_crc8(tmp, hdr_len);

	return mqtt_frame_stuff(out, out_max, tmp, (size_t) (p - tmp));
}

int32_t mqtt_frame_build_send(const uint8_t *flags, uint8_t flags_len,
		const uint8_t *values, uint8_t values_len, uint8_t *out, size_t out_max) {
	if (flags == NULL || values == NULL || out == NULL
			|| out_max < WIFI_BLE_FRAME_MAX_LEN) {
		return (int32_t) kFrame_ErrParam;
	}

	uint8_t tmp[WIFI_BLE_FRAME_MAX_LEN];
	uint8_t *p = tmp;

	*p++ = WIFIBLEUART_STX;
	*p++ = (uint8_t) s_cmd_table[MQTT_CMD_SEND_MQTT][0];
	*p++ = (uint8_t) s_cmd_table[MQTT_CMD_SEND_MQTT][1];

	priv_add_block(&p, flags, flags_len);
	priv_add_block(&p, values, values_len);

	size_t hdr_len = (size_t) (p - tmp);
	*p++ = mqtt_frame_crc8(tmp, hdr_len);

	return mqtt_frame_stuff(out, out_max, tmp, (size_t) (p - tmp));
}

/* ---------------------------------------------------------------------------
 * Parser
 * ------------------------------------------------------------------------- */

frame_status_t mqtt_frame_parse(const uint8_t *data, size_t len,
		mqtt_cmd_id_t *cmd_out) {
	if (data == NULL || len < 2U || cmd_out == NULL) {
		return kFrame_ErrParam;
	}

	*cmd_out = MQTT_CMD_UNKNOWN;

	/* Verificar CRC: el frame completo incluido el CRC debe dar 0x00. */
	if (mqtt_frame_crc8(data, len - 1U) != 0U) {
		return kFrame_ErrCRC;
	}

	/* Buscar el comando por los dos bytes de identificación. */
	char c0 = (char) data[1];
	char c1 = (char) data[2];

	for (uint8_t i = 0U; i < WIFI_BLE_CMD_COUNT; i++) {
		if (s_cmd_table[i][0] == c0 && s_cmd_table[i][1] == c1) {
			*cmd_out = (mqtt_cmd_id_t) i;
			return kFrame_Ok;
		}
	}

	return kFrame_ErrNoCmd;
}
