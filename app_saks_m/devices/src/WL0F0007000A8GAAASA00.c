/*
 * WL0F0007000A8GAAASA00.c
 *
 *  Created on: 17/04/2026
 *      Author: plibreros
 */
#include "WL0F0007000A8GAAASA00.h"
#include "can_freertos.h"
#include "winstar_canopen.h"

#define BUZZ_REPEAT 		80

static mcan_rtos_handle_t can_open;
static CAN_Type *can_peripheral = CAN0;

bool display_7inch_can_init(void) {
	return winstar_init_peripheral(&can_open, can_peripheral)
			== Winstar_Status_Ok;
}

void display_7inch_print(_display_index_7inch index_f,
		_arg_display_entry type_var, void *var) {
	if (var == NULL)
		return;

	switch (type_var) {
	case DISPLAY_TYPE_VALUE_INDICATOR: {
		uint8_t value = *(uint8_t*) var;
		(void) winstar_set_ind_value(&can_open, index_f, value);
		break;
	}
	case DISPLAY_TYPE_VALUE_FLOAT: {
		float value = *(float*) var;
		(void) winstar_set_text_float(&can_open, index_f, value);
		break;
	}
	case DISPLAY_TYPE_VALUE_STRING: {
		char *value = (char*) var;
		(void) winstar_set_text_string(&can_open, index_f, value);
		break;
	}
	case DISPLAY_TYPE_VALUE_INTEGER: {
		uint32_t value = *(uint32_t*) var;
		(void) winstar_set_text_integer(&can_open, index_f, value);
		break;
	}
	case DISPLAY_TYPE_VALUE_BUZZER: {
		uint8_t value = *(uint8_t*) var;
		(void) winstar_activation_buzzer(&can_open, (uint8_t) BUZZ_REPEAT,
				value);
		break;
	}
	default:
		break;
	}
}
