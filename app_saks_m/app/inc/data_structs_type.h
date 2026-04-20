/*
 * data_structs_type.h
 *
 *  Created on: 19/03/2026
 *      Author: plibreros
 */

#ifndef INC_DATA_STRUCTS_TYPE_H_
#define INC_DATA_STRUCTS_TYPE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct {
	uint32_t flags;
	uint32_t val1;
	uint32_t val2;
	uint32_t val3;
} _mqtt_sender;

typedef  struct {
	uint8_t item_index;
	uint8_t value_to_print;
} _lcd_sender;

typedef struct {
	uint8_t item_index;
	uint8_t value_to_print;
} _rtc_sender;

typedef struct {
	uint8_t item_index;
	uint8_t value_to_print;
} _sensors_sender;

#ifdef __cplusplus
}
#endif

#endif /* INC_DATA_STRUCTS_TYPE_H_ */
