/*
 * winsen.h
 *
 *  Created on: 16/04/2026
 *      Author: plibreros
 */

#ifndef PROTOCOLS_INC_WINSEN_H_
#define PROTOCOLS_INC_WINSEN_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#define WINSEN_FRAME_SIZE   9
#define WINSEN_START_BYTE   0xFF

/* Sensor types */
typedef enum {
    WINSEN_SENSOR_ZC05,
    WINSEN_SENSOR_ZE07
} winsen_sensor_t;

/* Parsed data */
typedef struct {
    uint16_t concentration;
    uint16_t full_range;
    uint8_t  gas_type;
    bool     fault;
} winsen_data_t;

/* Status */
typedef enum {
    WINSEN_OK = 0,
    WINSEN_ERROR_CHECKSUM,
    WINSEN_ERROR_FORMAT
} winsen_status_t;

/* API */
uint8_t winsen_checksum(const uint8_t *frame);
winsen_status_t winsen_validate_frame(const uint8_t *frame);

winsen_status_t winsen_parse_frame(winsen_sensor_t type, const uint8_t *frame, winsen_data_t *out);

void winsen_build_read_cmd(uint8_t *frame);
void winsen_build_set_qa_mode(uint8_t *frame);
void winsen_build_set_stream_mode(uint8_t *frame);


#ifdef __cplusplus
}
#endif


#endif /* PROTOCOLS_INC_WINSEN_H_ */
