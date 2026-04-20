/*
 * ZE07.h
 *
 *  Created on: 17/04/2026
 *      Author: plibreros
 */

#ifndef DEVICES_INC_ZE07_H_
#define DEVICES_INC_ZE07_H_

#ifdef __cplusplus
extern "C" {
#endif

void ze07_init(TaskHandle_t notify_task_handle);
winsen_data_t * ze07_get_data(uint32_t notif_val);

#ifdef __cplusplus
}
#endif

#endif /* DEVICES_INC_ZE07_H_ */
