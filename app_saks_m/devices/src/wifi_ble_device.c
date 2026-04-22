/*
 * wifi_ble_protocol.c
 *
 *  Created on: 20/04/2026
 *      Author: plibreros
 */
#include <wifi_ble_device.h>
#include "wifi_ble_uart_frame.h"

/* ---------------------------------------------------------------------------
 * Helper interno: ejecuta un intercambio básico y valida el frame recibido
 * ------------------------------------------------------------------------- */

/**
 * @brief Envía @p cmd, espera respuesta y verifica que el comando recibido
 *        coincida con @p expected_cmd.
 *
 * @param ctx           Contexto de transporte.
 * @param cmd           Comando a enviar.
 * @param expected_cmd  Comando que se espera recibir en la respuesta.
 * @param cmd_out       Si != NULL, se rellena con el comando parseado.
 * @return kProto_Ok, kProto_ErrTransport o kProto_ErrFrame.
 */
static proto_status_t priv_basic_exchange(mqtt_transport_ctx_t *ctx,
		mqtt_cmd_id_t cmd, mqtt_cmd_id_t expected_cmd, mqtt_cmd_id_t *cmd_out) {
	uint8_t frame[WIFI_BLE_FRAME_MAX_LEN];

	int32_t flen = mqtt_frame_build_basic(cmd, frame, sizeof(frame));
	if (flen <= 0) {
		return kProto_ErrFrame;
	}

	transport_status_t ts = mqtt_transport_transfer(ctx, frame, (size_t) flen);
	if (ts != kTransport_Ok) {
		return kProto_ErrTransport;
	}

	mqtt_cmd_id_t parsed = MQTT_CMD_UNKNOWN;
	frame_status_t fs = mqtt_frame_parse(ctx->rx_buf, ctx->rx_len, &parsed);
	if (fs != kFrame_Ok) {
		return kProto_ErrFrame;
	}

	if (parsed != expected_cmd) {
		return kProto_ErrFrame;
	}

	if (cmd_out != NULL) {
		*cmd_out = parsed;
	}

	return kProto_Ok;
}

/* ---------------------------------------------------------------------------
 * wifi_ble_proto_send_data
 * ------------------------------------------------------------------------- */

proto_status_t wifi_ble_proto_send_data(mqtt_transport_ctx_t *ctx,
		const mqtt_payload_t *payload) {
	if (ctx == NULL || payload == NULL) {
		return kProto_ErrParam;
	}

	uint8_t frame[WIFI_BLE_FRAME_MAX_LEN];

	int32_t flen = mqtt_frame_build_send(payload->flags,
			(uint8_t) WIFI_BLE_PROTO_FLAGS_LEN, payload->values,
			(uint8_t) WIFI_BLE_PROTO_VALUES_LEN, frame, sizeof(frame));
	if (flen <= 0) {
		return kProto_ErrFrame;
	}

	transport_status_t ts = mqtt_transport_transfer(ctx, frame, (size_t) flen);
	if (ts != kTransport_Ok) {
		return kProto_ErrTransport;
	}

	/* Validar CRC de la respuesta y comprobar ACK. */
	if (mqtt_frame_crc8(ctx->rx_buf, ctx->rx_len - 1U) != 0U) {
		return kProto_ErrFrame;
	}
	if (ctx->rx_buf[4] != WIFIBLEUART_PROC_OK) {
		return kProto_ErrNack;
	}

	return kProto_Ok;
}

/* ---------------------------------------------------------------------------
 * wifi_ble_proto_get_ntp_date
 * ------------------------------------------------------------------------- */

proto_status_t wifi_ble_proto_get_ntp_date(mqtt_transport_ctx_t *ctx,
		rtc_datetime_t *dt_out) {
	if (ctx == NULL || dt_out == NULL) {
		return kProto_ErrParam;
	}

	proto_status_t st = priv_basic_exchange(ctx, MQTT_CMD_GET_LOCAL_TIME,
			MQTT_CMD_GET_LOCAL_TIME,
			NULL);
	if (st != kProto_Ok) {
		return st;
	}

	const uint8_t *d = ctx->rx_buf;
	dt_out->year = ((uint16_t) d[5] << 8U) | d[4];
	dt_out->month = d[6] + 1U;
	dt_out->day = d[7];
	dt_out->hour = d[8];
	dt_out->minute = d[9];
	dt_out->second = d[10];

	return kProto_Ok;
}

/* ---------------------------------------------------------------------------
 * wifi_ble_proto_get_setpoints
 * ------------------------------------------------------------------------- */

proto_status_t wifi_ble_proto_get_setpoints(mqtt_transport_ctx_t *ctx,
		wifi_ble_setpoints_t *sp_out) {
	if (ctx == NULL || sp_out == NULL) {
		return kProto_ErrParam;
	}

	proto_status_t st = priv_basic_exchange(ctx, MQTT_CMD_GET_SETPOINTS,
			MQTT_CMD_GET_SETPOINTS,
			NULL);
	if (st != kProto_Ok) {
		return st;
	}

	const uint8_t *d = ctx->rx_buf;
	sp_out->num_bells = (uint8_t) (((uint16_t) d[5] << 8U) | d[4]);
	sp_out->um_temperature = (float) (((uint16_t) d[7] << 8U) | d[6]);
	sp_out->um_monoxide = (float) (((uint16_t) d[9] << 8U) | d[8]);
	sp_out->um_methane = (float) (((uint16_t) d[11] << 8U) | d[10]);

	return kProto_Ok;
}

/* ---------------------------------------------------------------------------
 * wifi_ble_proto_get_ssid
 * ------------------------------------------------------------------------- */

proto_status_t wifi_ble_proto_get_ssid(mqtt_transport_ctx_t *ctx,
		char *ssid_out, uint8_t *len_out) {
	if (ctx == NULL || ssid_out == NULL || len_out == NULL) {
		return kProto_ErrParam;
	}

	proto_status_t st = priv_basic_exchange(ctx, MQTT_CMD_GET_SETTINGS_W,
			MQTT_CMD_GET_SETTINGS_W,
			NULL);
	if (st != kProto_Ok) {
		return st;
	}

	/*
	 * Formato de la respuesta: STX | CMD[0] | CMD[1] | LEN_SSID | SSID... | CRC
	 * data[3] = longitud del SSID, data[4..] = contenido.
	 */
	const uint8_t *d = ctx->rx_buf;
	uint8_t ssid_len = d[3];

	if (ssid_len >= WIFI_BLE_FRAME_MAX_LEN) {
		return kProto_ErrFrame;
	}

	memset(ssid_out, 0, WIFI_BLE_FRAME_MAX_LEN);
	memcpy(ssid_out, &d[4], ssid_len);
	*len_out = ssid_len;

	return kProto_Ok;
}

/* ---------------------------------------------------------------------------
 * wifi_ble_proto_get_conn_status
 * ------------------------------------------------------------------------- */

proto_status_t wifi_ble_proto_get_conn_status(mqtt_transport_ctx_t *ctx,
		wifi_mqtt_conn_status_t *status) {
	if (ctx == NULL || status == NULL) {
		return kProto_ErrParam;
	}

	proto_status_t st = priv_basic_exchange(ctx, MQTT_CMD_IS_CONNECTED,
			MQTT_CMD_IS_CONNECTED,
			NULL);
	if (st != kProto_Ok) {
		return st;
	}

	/*
	 * data[4]: bits [2:0] = wl_status_t WiFi, bit [4] = estado MQTT.
	 */
	uint8_t flags = ctx->rx_buf[4];
	status->wifi_connected = ((flags & 0x07U) == (uint8_t) kWiFi_Connected);
	status->mqtt_connected = ((flags & 0x10U) >> 4U) == 0x01U;

	return kProto_Ok;
}

/* ---------------------------------------------------------------------------
 * wifi_ble_proto_get_ntp_status
 * ------------------------------------------------------------------------- */

proto_status_t wifi_ble_proto_get_ntp_status(mqtt_transport_ctx_t *ctx) {
	if (ctx == NULL) {
		return kProto_ErrParam;
	}

	proto_status_t st = priv_basic_exchange(ctx, MQTT_CMD_HAVE_NTP,
			MQTT_CMD_HAVE_NTP,
			NULL);
	if (st != kProto_Ok) {
		return st;
	}

	/*
	 * El ESP32 responde con PROC_OK (0xFD) en data[1] si NTP está listo.
	 */
	if (ctx->rx_buf[1] != WIFIBLEUART_PROC_OK) {
		return kProto_ErrNack;
	}

	return kProto_Ok;
}
