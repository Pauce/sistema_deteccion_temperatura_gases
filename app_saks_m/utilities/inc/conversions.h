/*
 * conversions.h
 *
 *  Created on: 31/03/2026
 *      Author: plibreros
 */

#ifndef UTILITIES_INC_CONVERSIONS_H_
#define UTILITIES_INC_CONVERSIONS_H_

#include <string.h>
#include <stdint.h>
#include <stdbool.h>

void float_to_str_1decimal(float value, char* buffer);
void uint32_to_str(uint32_t value, char* buffer, size_t buf_size);

#endif /* UTILITIES_INC_CONVERSIONS_H_ */
