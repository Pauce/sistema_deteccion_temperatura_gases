/*
 * fsm_gas.c
 *
 *  Created on: 30/04/2026
 *      Author: plibreros
 */
//#include <string.h>
//#include "fsm_gas.h"
//#include "winsen.h"
//#include "process_lcd.h"
//#include "messages_lcd.h"
//
//#define GAS_PRE_ALARM_PORCENT		70U
//#define GAS_MAX_COUNT_ERROS	    	20
//#define GAS_MAX_COUNT_ALARMS		5
//#define GAS_CO_INDICATOR_BYTE		4U
//#define GAS_CH4_INDICATOR_BYTE		1U
//
///* --- Tabla de divisores enteros --- */
//static const uint32_t divisors_int[] = { 1U,       // 0 decimales
//		10U,      // 1 decimal
//		100U,     // 2 decimales
//		1000U,    // 3 decimales
//		10000U	  // 4 decimales
//		};
//
//static float gas_calculate_concentration(const winsen_data_t gas_data);
//static void gas_fsm_wait(gas_fsm_context_t *gas_context, winsen_data_t gas_data,
//		float concentration, float pre_alarm_value);
//static void gas_fsm_normal(gas_fsm_context_t *gas_context);
//static void gas_fsm_pre_alarm(gas_fsm_context_t *gas_context);
//static void gas_fsm_alarm(gas_fsm_context_t *gas_context);
//static void gas_fsm_cnt_alarm(gas_fsm_context_t *gas_context);
//static void gas_fsm_fault_error(gas_fsm_context_t *gas_context);
//static void gas_fsm_report_error(gas_fsm_context_t *gas_context,
//		winsen_data_t gas_data);
//static void gas_fsm_send_lcd_mqtt(gas_fsm_context_t *gas_context,
//		winsen_data_t gas_data, float concentration);
//
//static data_print_lcd_t data_lcd;
//static char msg_for_lcd[LCD_MAX_PRINT_BUFFER_SIZE];
//
//void gas_fsm_init(gas_fsm_context_t *gas_context, uint16_t umb,
//		type_sensors_t type) {
//
//	memset(gas_context, 0, sizeof(gas_fsm_context_t));
//	gas_context->state = GAS_FSM_STATE_WAIT;
//	gas_context->started = false;
//	gas_context->alarm_count = 0;
//	gas_context->fault_error_count = 0;
//	gas_context->umbral = umb;
//
//	memset(&data_lcd, 0, sizeof(data_print_lcd_t));
//	data_lcd._index =
//			(type == TYPE_METHANE) ?
//					display_7inch_Status_CH4 : display_7inch_Status_CO;
//	data_lcd._typ = DISPLAY_TYPE_VALUE_STRING;
//	strncpy(data_lcd.value.str, MESSAGE_INF_STAT_STOP_GAS,
//			strlen(MESSAGE_INF_STAT_STOP_GAS));
//
//	(void) ao_process_lcd_queue_send(&data_lcd);
//
//	memset(&data_lcd, 0, sizeof(data_print_lcd_t));
//	data_lcd._index =
//			(type == TYPE_METHANE) ?
//					display_7inch_CH4_Indicator : display_7inch_CO_Indicator;
//	data_lcd._typ = DISPLAY_TYPE_VALUE_INDICATOR;
//	data_lcd.value.u8 = 1U;
//
//	(void) ao_process_lcd_queue_send(&data_lcd);
//
//}
//
//void gas_fsm_run(gas_fsm_context_t *gas_context, winsen_data_t gas_data) {
//
//	float concentration = gas_calculate_concentration(gas_data);
//	float pre_alarm_value = (float) ((gas_context->umbral
//			* GAS_PRE_ALARM_PORCENT) / 100U);
//
//	switch (gas_context->state) {
//	case GAS_FSM_STATE_WAIT:
//		gas_fsm_wait(gas_context, gas_data, concentration, pre_alarm_value);
//		break;
//	case GAS_FSM_STATE_FAULT_ERROR:
//		gas_fsm_fault_error(gas_context);
//		break;
//	case GAS_FSM_STATE_NORMAL:
//		gas_fsm_normal(gas_context);
//		break;
//	case GAS_FSM_STATE_PRE_ALARM:
//		gas_fsm_pre_alarm(gas_context);
//		break;
//	case GAS_FSM_STATE_COUNT_ALARM:
//		gas_fsm_cnt_alarm(gas_context);
//		break;
//	case GAS_FSM_STATE_ALARM:
//		gas_fsm_alarm(gas_context);
//		break;
//	case GAS_FSM_STATE_SEND_LCD_MQTT:
//		gas_fsm_send_lcd_mqtt(gas_context, gas_data, concentration);
//		break;
//	case GAS_FSM_STATE_REPORT_ERROR:
//		gas_fsm_report_error(gas_context, gas_data);
//		break;
//	default:
//		break;
//	}
//
//}
//
//static float gas_calculate_concentration(const winsen_data_t gas_data) {
//
//	uint8_t d = gas_data.decimals;
//
//	if (d > WINSEN_MAX_DECIMALS) {
//		d = WINSEN_MAX_DECIMALS;
//	}
//
//	uint32_t divisor =
//			(d < (sizeof(divisors_int) / sizeof(divisors_int[0]))) ?
//					divisors_int[d] : divisors_int[WINSEN_MAX_DECIMALS];
//
//	return (float) gas_data.concentration_raw / (float) divisor;
//}
//
//static void gas_fsm_wait(gas_fsm_context_t *gas_context, winsen_data_t gas_data,
//		float concentration, float pre_alarm_value) {
//
//	if (gas_data.fault)
//		gas_context->state = GAS_FSM_STATE_FAULT_ERROR;
//	else if (concentration < pre_alarm_value)
//		gas_context->state = GAS_FSM_STATE_NORMAL;
//	else if (pre_alarm_value <= concentration
//			&& concentration < (float) (gas_context->umbral))
//		gas_context->state = GAS_FSM_STATE_PRE_ALARM;
//	else
//		gas_context->state = GAS_FSM_STATE_COUNT_ALARM;
//}
//
//static void gas_fsm_normal(gas_fsm_context_t *gas_context) {
//	gas_context->state = GAS_FSM_STATE_SEND_LCD_MQTT;
//}
//
//static void gas_fsm_fault_error(gas_fsm_context_t *gas_context) {
//
//	if (gas_context->fault_error_count++ > GAS_MAX_COUNT_ERROS)
//		gas_context->state = GAS_FSM_STATE_REPORT_ERROR;
//	else
//		gas_context->state = GAS_FSM_STATE_WAIT;
//}
//
//static void gas_fsm_pre_alarm(gas_fsm_context_t *gas_context) {
//	gas_context->state = GAS_FSM_STATE_SEND_LCD_MQTT;
//}
//
//static void gas_fsm_cnt_alarm(gas_fsm_context_t *gas_context) {
//
//	if (gas_context->alarm_count++ > GAS_MAX_COUNT_ALARMS)
//		gas_context->state = GAS_FSM_STATE_ALARM;
//	else
//		gas_context->state = GAS_FSM_STATE_SEND_LCD_MQTT;
//}
//
//static void gas_fsm_alarm(gas_fsm_context_t *gas_context) {
//	gas_context->state = GAS_FSM_STATE_SEND_LCD_MQTT;
//}
//
//static void gas_fsm_send_lcd_mqtt(gas_fsm_context_t *gas_context,
//		winsen_data_t gas_data, float concentration) {
//
//	memset(&data_lcd, 0, sizeof(data_print_lcd_t));
//	data_lcd._index =
//			(gas_data.gas_type == GAS_CH4_INDICATOR_BYTE) ?
//					display_7inch_CH4_Value : display_7inch_CO_Value;
//	data_lcd._typ = DISPLAY_TYPE_VALUE_FLOAT;
//	data_lcd.value.f = concentration;
//
//	(void) ao_process_lcd_queue_send(&data_lcd);
//
//	if (!gas_context->started) {
//		gas_context->started = true;
//		memset(&data_lcd, 0, sizeof(data_print_lcd_t));
//		data_lcd._index =
//				(gas_data.gas_type == GAS_CH4_INDICATOR_BYTE) ?
//						display_7inch_Status_CH4 : display_7inch_Status_CO;
//		data_lcd._typ = DISPLAY_TYPE_VALUE_STRING;
//		strncpy(data_lcd.value.str, MESSAGE_INF_STAT_START_GAS,
//				strlen(MESSAGE_INF_STAT_START_GAS));
//		(void) ao_process_lcd_queue_send(&data_lcd);
//	}
//
//	if (concentration > (float) (gas_context->umbral)) {
//		memset(&data_lcd, 0, sizeof(data_print_lcd_t));
//		data_lcd._index =
//				(gas_data.gas_type == GAS_CH4_INDICATOR_BYTE) ?
//						display_7inch_CH4_Indicator :
//						display_7inch_CO_Indicator;
//		data_lcd._typ = DISPLAY_TYPE_VALUE_INDICATOR;
//		data_lcd.value.u8 = 1U;
//
//		(void) ao_process_lcd_queue_send(&data_lcd);
//
//		memset(&data_lcd, 0, sizeof(data_print_lcd_t));
//		data_lcd._index =
//				(gas_data.gas_type == GAS_CH4_INDICATOR_BYTE) ?
//						display_7inch_Status_CH4 : display_7inch_Status_CO;
//		data_lcd._typ = DISPLAY_TYPE_VALUE_STRING;
//		strncpy(data_lcd.value.str, MESSAGE_INF_STAT_STOP_GAS,
//				strlen(MESSAGE_INF_STAT_STOP_GAS));
//
//		(void) ao_process_lcd_queue_send(&data_lcd);
//	}
//
//	/*Enviar datos via MQTT*/
//
//	gas_context->state = GAS_FSM_STATE_WAIT;
//}
//
//static void gas_fsm_report_error(gas_fsm_context_t *gas_context,
//		winsen_data_t gas_data) {
//
//	memset(msg_for_lcd, 0, sizeof(msg_for_lcd));
//	const char *sensor_name =
//			(gas_data.gas_type == GAS_CH4_INDICATOR_BYTE) ?
//					"ZC05.\0" : "ZE07.\0";
//	snprintf(msg_for_lcd, sizeof(msg_for_lcd), "%s%s", MESSAGE_ERROR_GAS,
//			sensor_name);
//
//	memset(&data_lcd, 0, sizeof(data_print_lcd_t));
//	data_lcd._index = display_7inch_Info;
//	data_lcd._typ = DISPLAY_TYPE_VALUE_STRING;
//	strncpy(data_lcd.value.str, msg_for_lcd, strlen(msg_for_lcd));
//
//	(void) ao_process_lcd_queue_send(&data_lcd);
//
//	/*Enviar a MQTT*/
//
//	gas_context->state = GAS_FSM_STATE_WAIT;
//}
/*
 * fsm_gas.c
 *
 *  Created on: 30/04/2026
 *      Author: plibreros
 */
#include <string.h>
#include "fsm_gas.h"
#include "winsen.h"
#include "process_lcd.h"
#include "messages_lcd.h"

#define GAS_PRE_ALARM_PORCENT       70U
#define GAS_MAX_COUNT_ERROS         20
#define GAS_MAX_COUNT_ALARMS        5
#define GAS_CO_INDICATOR_BYTE       4U
#define GAS_CH4_INDICATOR_BYTE      1U

/* --- Tabla de divisores enteros --- */
static const uint32_t divisors_int[] = { 1U, 10U, 100U, 1000U, 10000U };

static inline void lcd_send_float(_display_index_7inch idx, float val);
static inline void lcd_send_str(_display_index_7inch idx, const char *msg);
static inline void lcd_send_indicator(_display_index_7inch idx, uint8_t val);
static inline void lcd_send_range(_display_index_7inch idx, uint16_t range);

static data_print_lcd_t data_lcd;
static char msg_for_lcd[LCD_MAX_PRINT_BUFFER_SIZE];

/* --- Helper: envía un valor float al LCD ----------------------------- */
static inline void lcd_send_float(_display_index_7inch idx, float val) {
	memset(&data_lcd, 0, sizeof(data_print_lcd_t));
	data_lcd._index = idx;
	data_lcd._typ = DISPLAY_TYPE_VALUE_FLOAT;
	data_lcd.value.f = val;
	(void) ao_process_lcd_queue_send(&data_lcd);
}

/* --- Helper: envía un string al LCD ---------------------------------- */
static inline void lcd_send_str(_display_index_7inch idx, const char *msg) {
	memset(&data_lcd, 0, sizeof(data_print_lcd_t));
	data_lcd._index = idx;
	data_lcd._typ = DISPLAY_TYPE_VALUE_STRING;
	strncpy(data_lcd.value.str, msg, strlen(msg));
	(void) ao_process_lcd_queue_send(&data_lcd);
}

/* --- Helper: envía un indicador (u8) al LCD -------------------------- */
static inline void lcd_send_indicator(_display_index_7inch idx, uint8_t val) {
	memset(&data_lcd, 0, sizeof(data_print_lcd_t));
	data_lcd._index = idx;
	data_lcd._typ = DISPLAY_TYPE_VALUE_INDICATOR;
	data_lcd.value.u8 = val;
	(void) ao_process_lcd_queue_send(&data_lcd);
}

/* --- Helper: envía un range (u32) al LCD -------------------------- */
static inline void lcd_send_range(_display_index_7inch idx, uint16_t range) {
	memset(&data_lcd, 0, sizeof(data_print_lcd_t));
	data_lcd._index = idx;
	data_lcd._typ = DISPLAY_TYPE_VALUE_INTEGER;
	data_lcd.value.u32 = (uint32_t) range;
	(void) ao_process_lcd_queue_send(&data_lcd);
}

/* --- Selecciona índice LCD según tipo de gas (CH4 / CO) -------------- */
#define LCD_IDX(gas_type, idx_ch4, idx_co) \
    ((gas_type) == GAS_CH4_INDICATOR_BYTE ? (idx_ch4) : (idx_co))

/* ===================================================================== */

void gas_fsm_init(gas_fsm_context_t *gas_context, uint16_t umb,
		type_sensors_t type) {

	memset(gas_context, 0, sizeof(gas_fsm_context_t));
	gas_context->state = GAS_FSM_STATE_WAIT;
	gas_context->umbral = umb;

	lcd_send_str(
			(type == TYPE_METHANE) ?
					display_7inch_Status_CH4 : display_7inch_Status_CO,
			MESSAGE_INF_STAT_STOP_GAS);

	lcd_send_indicator(
			(type == TYPE_METHANE) ?
					display_7inch_CH4_Indicator : display_7inch_CO_Indicator,
			1U);
}

void gas_fsm_run(gas_fsm_context_t *gas_context, winsen_data_t gas_data) {

	/* Calcula concentración con clamping de decimales */
	uint8_t d = (gas_data.decimals > WINSEN_MAX_DECIMALS) ?
	WINSEN_MAX_DECIMALS :
															gas_data.decimals;
	float concentration = (float) gas_data.concentration_raw
			/ (float) divisors_int[d];
	float pre_alarm_value = (float) ((gas_context->umbral
			* GAS_PRE_ALARM_PORCENT) / 100U);

	uint8_t gas_type = gas_data.gas_type;

	switch (gas_context->state) {

	case GAS_FSM_STATE_WAIT:
		if (gas_data.fault)
			gas_context->state = GAS_FSM_STATE_FAULT_ERROR;
		else if (concentration < pre_alarm_value)
			gas_context->state = GAS_FSM_STATE_NORMAL;
		else if (concentration < (float) (gas_context->umbral))
			gas_context->state = GAS_FSM_STATE_PRE_ALARM;
		else
			gas_context->state = GAS_FSM_STATE_COUNT_ALARM;
		break;

	case GAS_FSM_STATE_FAULT_ERROR:
		gas_context->state =
				(gas_context->fault_error_count++ > GAS_MAX_COUNT_ERROS) ?
						GAS_FSM_STATE_REPORT_ERROR : GAS_FSM_STATE_WAIT;
		break;

	case GAS_FSM_STATE_NORMAL:
	case GAS_FSM_STATE_PRE_ALARM:
	case GAS_FSM_STATE_ALARM:
		gas_context->state = GAS_FSM_STATE_SEND_LCD_MQTT;
		break;

	case GAS_FSM_STATE_COUNT_ALARM:
		gas_context->state =
				(gas_context->alarm_count++ > GAS_MAX_COUNT_ALARMS) ?
						GAS_FSM_STATE_ALARM : GAS_FSM_STATE_SEND_LCD_MQTT;
		break;

	case GAS_FSM_STATE_SEND_LCD_MQTT:
		lcd_send_float(
				LCD_IDX(gas_type, display_7inch_CH4_Value,
						display_7inch_CO_Value), concentration);

		if (!gas_context->started) {
			gas_context->started = true;
			lcd_send_str(
					LCD_IDX(gas_type, display_7inch_Status_CH4,
							display_7inch_Status_CO),
					MESSAGE_INF_STAT_START_GAS);
			lcd_send_range(
					LCD_IDX(gas_type, display_7inch_FullRange_CH4,
							display_7inch_FullRange_CO), gas_data.full_range);
		}

		if (concentration > (float) (gas_context->umbral)) {
			lcd_send_indicator(
					LCD_IDX(gas_type, display_7inch_CH4_Indicator,
							display_7inch_CO_Indicator), 1U);
			lcd_send_str(
					LCD_IDX(gas_type, display_7inch_Status_CH4,
							display_7inch_Status_CO),
					MESSAGE_INF_STAT_STOP_GAS);
		}

		/*Enviar datos via MQTT*/

		gas_context->state = GAS_FSM_STATE_WAIT;
		break;

	case GAS_FSM_STATE_REPORT_ERROR:
		snprintf(msg_for_lcd, sizeof(msg_for_lcd), "%s%s",
		MESSAGE_ERROR_GAS,
				(gas_type == GAS_CH4_INDICATOR_BYTE) ? "ZC05." : "ZE07.");

		lcd_send_str(display_7inch_Info, msg_for_lcd);

		/*Enviar a MQTT*/

		gas_context->state = GAS_FSM_STATE_WAIT;
		break;

	default:
		break;
	}
}
