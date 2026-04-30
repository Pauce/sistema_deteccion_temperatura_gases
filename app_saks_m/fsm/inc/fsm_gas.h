/*
 * fsm_gas.h
 *
 *  Created on: 30/04/2026
 *      Author: plibreros
 */

#ifndef FSM_INC_FSM_GAS_H_
#define FSM_INC_FSM_GAS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "winsen.h"
#include "process_extinction.h"

typedef enum{
    GAS_FSM_STATE_WAIT,
    GAS_FSM_STATE_FAULT_ERROR,
    GAS_FSM_STATE_NORMAL,
    GAS_FSM_STATE_PRE_ALARM,
    GAS_FSM_STATE_COUNT_ALARM,
    GAS_FSM_STATE_ALARM,
    GAS_FSM_STATE_SEND_LCD_MQTT,
    GAS_FSM_STATE_REPORT_ERROR
} states_fsm_gas_t;

typedef struct {
    states_fsm_gas_t state;
    uint8_t fault_error_count;
    uint8_t alarm_count;
    uint16_t umbral;
    bool started;
} gas_fsm_context_t;

void gas_fsm_init(gas_fsm_context_t *gas_context, uint16_t umb, type_sensors_t type);
void gas_fsm_run(gas_fsm_context_t *gas_context, winsen_data_t gas_data);

#ifdef __cplusplus
}
#endif

#endif /* FSM_INC_FSM_GAS_H_ */
