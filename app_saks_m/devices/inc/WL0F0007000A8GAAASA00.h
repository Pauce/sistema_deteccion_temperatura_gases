/*
 * WL0F0007000A8GAAASA00.h
 *
 *  Created on: 17/04/2026
 *      Author: plibreros
 */

#ifndef DEVICES_INC_WL0F0007000A8GAAASA00_H_
#define DEVICES_INC_WL0F0007000A8GAAASA00_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

typedef enum {
	DISPLAY_TYPE_VALUE_INDICATOR,
	DISPLAY_TYPE_VALUE_FLOAT,
	DISPLAY_TYPE_VALUE_STRING,
	DISPLAY_TYPE_VALUE_INTEGER,
	DISPLAY_TYPE_VALUE_BUZZER
} _arg_display_entry;

typedef enum {
	display_7inch_Status_Temp,
	display_7inch_Temp_Value1,
	display_7inch_Temp_Value2,
	display_7inch_Temp_Value3,
	display_7inch_FullRange_Temp,
	display_7inch_Temp_Indicator,
	display_7inch_Status_CO,
	display_7inch_CO_Value,
	display_7inch_FullRange_CO,
	display_7inch_CO_Indicator,
	display_7inch_Status_CH4,
	display_7inch_CH4_Value,
	display_7inch_FullRange_CH4,
	display_7inch_CH4_Indicator,
	display_7inch_SSID_Name,
	display_7inch_NumberTempeSensors,
	display_7inch_Next_Revision,
	display_7inch_Time,
	display_7inch_Date,
	display_7inch_Info,

	display_7inch_count_fields
} _display_index_7inch;

bool display_7inch_can_init(void);
void display_7inch_print(_display_index_7inch index, _arg_display_entry type_var, void *var);

#ifdef __cplusplus
}
#endif

#endif /* DEVICES_INC_WL0F0007000A8GAAASA00_H_ */
