/*
 * ZC05.h
 *
 *  Created on: 17/04/2026
 *      Author: plibreros
 */

#ifndef DEVICES_INC_ZC05_H_
#define DEVICES_INC_ZC05_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "fsl_debug_console.h"
#include "task.h"
#include "uart_freertos.h"
#include "winsen.h"

void zc05_init(TaskHandle_t notify_task_handle);
winsen_data_t * zc05_get_data(uint32_t notif_val);

#ifdef __cplusplus
}
#endif

#endif /* DEVICES_INC_ZC05_H_ */
