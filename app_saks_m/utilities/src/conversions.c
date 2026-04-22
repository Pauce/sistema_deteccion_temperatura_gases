/*
 * conversions.c
 *
 *  Created on: 31/03/2026
 *      Author: plibreros
 */
#include "conversions.h"

/*!
 * @brief floatToStr_1Decimal: Method to convert a Double to char*.
 * @param Double: value = value to convert.
 * @param char*: buffer = a pointer to save the result.
 */
void float_to_str_1decimal(float value, char *buffer) {
	int intVal = (int) (value * 10.0);

	bool isNegative = false;
	if (intVal < 0) {
		isNegative = true;
		intVal = -intVal;
	}

	int intPart = intVal / 10;
	int decimalPart = intVal % 10;

	char *ptr = buffer;

	if (isNegative) {
		*ptr++ = '-';
	}

	if (intPart == 0) {
		*ptr++ = '0';
	} else {
		char temp[10];
		int i = 0;
		while (intPart > 0) {
			temp[i++] = '0' + (intPart % 10);
			intPart /= 10;
		}
		while (i-- > 0) {
			*ptr++ = temp[i];
		}
	}

	*ptr++ = '.';
	*ptr++ = '0' + decimalPart;
	*ptr = '\0';
}
/*!
 * @brief uint32ToStr: Method to convert a uint32_t to char*.
 * @param uint32_t: value = value to convert.
 * @param char*: buffer = a pointer to save the result.
 * @param buf_size = size of buffer.
 */
void uint32_to_str(uint32_t value, char *buffer, size_t buf_size) {

	if (buffer == NULL || buf_size < 2)
		return;

	char temp[11];
	int i = 0;

	if (value == 0) {
		buffer[0] = '0';
		buffer[1] = '\0';
		return;
	}

	while (value > 0) {
		temp[i++] = '0' + (value % 10);
		value /= 10;
	}

	if ((size_t) i >= buf_size)
		return; // no cabe

	int j = 0;
	while (i-- > 0) {
		buffer[j++] = temp[i];
	}
	buffer[j] = '\0';
}
