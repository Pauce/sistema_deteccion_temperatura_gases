/*
 * winstar_canopen.h
 *
 *  Created on: 31/03/2026
 *      Author: plibreros
 */

#ifndef PROTOCOLS_INC_WINSTAR_CANOPEN_H_
#define PROTOCOLS_INC_WINSTAR_CANOPEN_H_

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdint.h>
#include "can_freertos.h"
#include "conversions.h"
#include "fsl_debug_console.h"


typedef enum {
	Winstar_Status_Ok,
	Winstar_Status_Error
} _winstar_status;

_winstar_status winstar_init_peripheral(mcan_rtos_handle_t *can_open, CAN_Type *base);
_winstar_status winstar_set_ind_value(mcan_rtos_handle_t *can_open, uint8_t indexIndicator, uint8_t value);
_winstar_status winstar_set_text_float(mcan_rtos_handle_t *can_open, uint8_t textIndicator, float value);
_winstar_status winstar_set_text_string(mcan_rtos_handle_t *can_open, uint8_t textIndicator, char* info);
_winstar_status winstar_set_text_integer(mcan_rtos_handle_t *can_open, uint8_t textIndicator, uint32_t value);
_winstar_status winstar_activation_buzzer(mcan_rtos_handle_t *can_open, uint8_t repeat, uint8_t period);

#ifdef __cplusplus
}
#endif

#endif /* PROTOCOLS_INC_WINSTAR_CANOPEN_H_ */
