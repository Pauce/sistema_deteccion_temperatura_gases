/*
 * process_extinction.h
 *
 *  Created on: 28/04/2026
 *      Author: plibreros
 */

#ifndef APP_INC_PROCESS_EXTINCTION_H_
#define APP_INC_PROCESS_EXTINCTION_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "winsen.h"
#include "mcp960x.h"

typedef enum {
	TYPE_THERMOCOUPLE,
	TYPE_MONOXIDE,
	TYPE_METHANE
} type_sensors_t;

typedef struct {
	type_sensors_t sensor;
	union {
		winsen_data_t gas_sensor;
		_mcp960x_device temp_sensor;
	} value;
} data_sensors_t;

void ao_process_extinction_init(void);
bool ao_process_extinction_queue_send(data_sensors_t * sensor);

#ifdef __cplusplus
}
#endif

#endif /* APP_INC_PROCESS_EXTINCTION_H_ */
