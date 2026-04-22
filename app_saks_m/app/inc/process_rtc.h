/*
 * process_rtc.h
 *
 *  Created on: 21/04/2026
 *      Author: plibreros
 */

#ifndef APP_INC_PROCESS_RTC_H_
#define APP_INC_PROCESS_RTC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "fsl_rtc.h"

typedef struct {
	rtc_datetime_t data_time;
} data_rtc_t;

void ao_process_rtc_init(void);
bool ao_process_rtc_queue_send(data_rtc_t *msg);

#ifdef __cplusplus
}
#endif

#endif /* APP_INC_PROCESS_RTC_H_ */
