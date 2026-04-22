/*
 * winsen.c
 *
 *  Created on: 16/04/2026
 *      Author: plibreros
 */

/*------------------ Initiative Upload Data Format ------------------------*/
/*| Byte0 | Byte1 | Byte2 | Byte3 | Byte4 | Byte5 |	Byte6 | Byte7 | Byte8 |*/
/*-------------------------------------------------------------------------*/
/*| Start |  Gas  | Unit  |   N°  |Concent|Concent| Full R| Full R| Check |*/
/*| Byte  | Type  | ppm   |decimal| High  |  Low  | High  |  Low  |  sum  |*/

/*------------------ Question & Answer Data Format ------------------------*/
/*| Byte0 | Byte1 | Byte2 | Byte3 | Byte4 | Byte5 |	Byte6 | Byte7 | Byte8 |*/
/*-------------------------------------------------------------------------*/
/*| Start |  Gas  |Concent|Concent| Reser | Reser |Concent|Concent| Check |*/
/*| Byte  | Type  | High  | Low   | ved   | ved   | High  | Low   | sum   |*/

#include "winsen.h"
#define WINSEN_LEN_FRAME	9U

static void winsen_clear_payload(uint8_t *frame);

static void winsen_clear_payload(uint8_t *frame) {
	for (int i = 3; i < 8; i++) {
		frame[i] = 0x00;
	}
}

uint8_t winsen_checksum(const uint8_t *frame) {
	uint8_t sum = 0;

	for (int i = 1; i < 8; i++) {
		sum += frame[i];
	}

	return (uint8_t) (~sum + 1);
}

winsen_status_t winsen_validate_frame(const uint8_t *frame) {
	if (frame == 0) {
		return WINSEN_ERROR_FORMAT;
	}

	if (frame[0] != WINSEN_START_BYTE) {
		return WINSEN_ERROR_FORMAT;
	}

	uint8_t checksum = winsen_checksum(frame);

	if (checksum != frame[8]) {
		return WINSEN_ERROR_CHECKSUM;
	}

	return WINSEN_OK;
}

winsen_status_t winsen_parse_frame(winsen_sensor_t type, const uint8_t *frame,
		winsen_data_t *out) {
	if ((frame == 0) || (out == 0)) {
		return WINSEN_ERROR_FORMAT;
	}

	winsen_status_t st = winsen_validate_frame(frame);
	if (st != WINSEN_OK) {
		return st;
	}

	switch (type) {

	case WINSEN_SENSOR_ZC05: {
		uint8_t high = frame[4];
		uint8_t low = frame[5];

		out->fault = (high & 0x80U) ? true : false;

		uint16_t value = ((high & 0x3FU) << 8) | low;
		out->concentration = value;

		out->full_range = ((uint16_t) frame[6] << 8) | frame[7];
		out->gas_type = frame[1];
	}
		break;

	case WINSEN_SENSOR_ZE07: {
		uint16_t raw = ((uint16_t) frame[4] << 8) | frame[5];

		out->concentration = raw / 10U;
		out->full_range = ((uint16_t) frame[6] << 8) | frame[7];
		out->gas_type = frame[1];
		out->fault = false;
	}
		break;

	default:
		return WINSEN_ERROR_FORMAT;
	}

	return WINSEN_OK;
}

void winsen_build_read_cmd(uint8_t *frame) {
	if (frame == 0)
		return;

	frame[0] = 0xFF;
	frame[1] = 0x01;
	frame[2] = 0x86;

	winsen_clear_payload(frame);

	frame[8] = winsen_checksum(frame);
}

void winsen_build_set_qa_mode(uint8_t *frame) {
	if (frame == 0)
		return;

	frame[0] = 0xFF;
	frame[1] = 0x01;
	frame[2] = 0x78;
	frame[3] = 0x41;

	for (int i = 4; i < 8; i++) {
		frame[i] = 0x00;
	}

	frame[8] = winsen_checksum(frame);
}

void winsen_build_set_stream_mode(uint8_t *frame) {
	if (frame == 0)
		return;

	frame[0] = 0xFF;
	frame[1] = 0x01;
	frame[2] = 0x78;
	frame[3] = 0x40;

	for (int i = 4; i < 8; i++) {
		frame[i] = 0x00;
	}

	frame[8] = winsen_checksum(frame);
}
