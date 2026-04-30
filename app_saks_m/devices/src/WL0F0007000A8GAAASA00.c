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

void display_7inch_print(data_print_lcd_t * data) {
	if (data == NULL)
		return;

	switch (data->_typ) {
	case DISPLAY_TYPE_VALUE_INDICATOR: {
		(void) winstar_set_ind_value(&can_open, data->_index, data->value.u8);
		break;
	}
	case DISPLAY_TYPE_VALUE_FLOAT: {
		(void) winstar_set_text_float(&can_open, data->_index, data->value.f);
		break;
	}
	case DISPLAY_TYPE_VALUE_STRING: {
		(void) winstar_set_text_string(&can_open, data->_index, data->value.str);
		break;
	}
	case DISPLAY_TYPE_VALUE_INTEGER: {
		(void) winstar_set_text_integer(&can_open, data->_index, data->value.u32);
		break;
	}
	case DISPLAY_TYPE_VALUE_BUZZER: {
		(void) winstar_activation_buzzer(&can_open, (uint8_t) BUZZ_REPEAT,
				data->value.u8);
		break;
	}
	default:
		break;
	}
}
