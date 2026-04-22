/*
 * wifi_ble_protocol.h
 *
 *  Created on: 20/04/2026
 *      Author: plibreros
 */

#ifndef PROTOCOLS_INC_WIFI_BLE_DEVICE_H_
#define PROTOCOLS_INC_WIFI_BLE_DEVICE_H_

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#include "fsl_rtc.h"                    /* rtc_datetime_t                    */
#include "wifi_ble_uart_transport.h"

/* ---------------------------------------------------------------------------
 * Estructuras de dominio
 * ------------------------------------------------------------------------- */

/** Estado de la conexión WiFi (igual al enum wl_status_t original). */
typedef enum {
	kWiFi_Idle = 0,
	kWiFi_NoSSID = 1,
	kWiFi_ScanDone = 2,
	kWiFi_Connected = 3,
	kWiFi_ConnFailed = 4,
	kWiFi_ConnLost = 5,
	kWiFi_Disconnected = 6,
} wifi_status_t;

/** Flags de conexión retornados por MQTTUARTGetStatusWiFiMQTT. */
typedef struct {
	bool wifi_connected; /**< true si wl_status == kWiFi_Connected.       */
	bool mqtt_connected; /**< true si el broker MQTT está conectado.       */
} wifi_mqtt_conn_status_t;

/** Parámetros de configuración / set points. */
typedef struct {
	uint8_t num_bells;
	float um_temperature;
	float um_monoxide;
	float um_methane;
} wifi_ble_setpoints_t;

/**
 * @brief Payload de datos a publicar vía MQTT.
 *
 * Se mantiene desacoplado de _mqttSender para no depender de saks_common.h
 * en esta capa. El llamante mapea sus datos aquí.
 */
#define WIFI_BLE_PROTO_FLAGS_LEN    6U
#define WIFI_BLE_PROTO_VALUES_LEN   12U

typedef struct {
	uint8_t flags[WIFI_BLE_PROTO_FLAGS_LEN];
	uint8_t values[WIFI_BLE_PROTO_VALUES_LEN];
} mqtt_payload_t;

/* ---------------------------------------------------------------------------
 * Códigos de retorno
 * ------------------------------------------------------------------------- */

typedef enum {
	kProto_Ok = 0, kProto_ErrParam = -1, kProto_ErrTransport = -2, /**< Error en TX o timeout RX.               */
	kProto_ErrFrame = -3, /**< CRC incorrecto o comando inesperado.    */
	kProto_ErrNack = -4, /**< ESP32 respondió con processError.       */
} proto_status_t;

/* ---------------------------------------------------------------------------
 * API de alto nivel
 * ------------------------------------------------------------------------- */

/**
 * @brief Publica datos de sensores en el broker MQTT.
 *
 * @param ctx       Contexto de transporte inicializado.
 * @param payload   Datos a enviar.
 * @return kProto_Ok o código de error.
 */
proto_status_t wifi_ble_proto_send_data(mqtt_transport_ctx_t *ctx,
		const mqtt_payload_t *payload);

/**
 * @brief Consulta fecha/hora NTP al ESP32.
 *
 * @param ctx       Contexto de transporte.
 * @param dt_out    Estructura donde se escribe la fecha/hora.
 */
proto_status_t wifi_ble_proto_get_ntp_date(mqtt_transport_ctx_t *ctx,
		rtc_datetime_t *dt_out);

/**
 * @brief Lee los set points almacenados en el ESP32.
 *
 * @param ctx       Contexto de transporte.
 * @param sp_out    Estructura donde se escriben los set points.
 */
proto_status_t wifi_ble_proto_get_setpoints(mqtt_transport_ctx_t *ctx,
		wifi_ble_setpoints_t *sp_out);

/**
 * @brief Lee el nombre de la red WiFi (SSID) configurada en el ESP32.
 *
 * @param ctx       Contexto de transporte.
 * @param ssid_out  Buffer de destino (mín. WIFI_BLE_FRAME_MAX_LEN bytes).
 * @param len_out   Longitud de la cadena copiada.
 */
proto_status_t wifi_ble_proto_get_ssid(mqtt_transport_ctx_t *ctx,
		char *ssid_out, uint8_t *len_out);

/**
 * @brief Consulta el estado de conexión WiFi y MQTT.
 *
 * @param ctx       Contexto de transporte.
 * @param status    Estructura donde se escribe el estado.
 */
proto_status_t wifi_ble_proto_get_conn_status(mqtt_transport_ctx_t *ctx,
		wifi_mqtt_conn_status_t *status);

/**
 * @brief Verifica si el servicio NTP está activo en el ESP32.
 *
 * @param ctx   Contexto de transporte.
 * @return kProto_Ok si NTP listo, kProto_ErrNack si no.
 */
proto_status_t wifi_ble_proto_get_ntp_status(mqtt_transport_ctx_t *ctx);

#ifdef __cplusplus
}
#endif

#endif /* PROTOCOLS_INC_WIFI_BLE_DEVICE_H_ */
