/*
 * process_lcd.h
 *
 *  Created on: 21/04/2026
 *      Author: plibreros
 */

#ifndef APP_INC_PROCESS_LCD_H_
#define APP_INC_PROCESS_LCD_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "WL0F0007000A8GAAASA00.h"

void ao_process_lcd_init(void);
bool ao_process_lcd_queue_send(data_print_lcd_t *msg);

#ifdef __cplusplus
}
#endif

#endif /* APP_INC_PROCESS_LCD_H_ */
