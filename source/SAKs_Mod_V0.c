/*
 * Copyright 2016-2026 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file    SAKs_Mod_V0.c
 * @brief   Application entry point.
 */
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_debug_console.h"
/* TODO: insert other include files here. */
#include "app_saksm.h"
/* TODO: insert other definitions and declarations here. */

/*
 * @brief   Application entry point.
 */
int main(void) {

	/* Init board hardware. */
	/*For All UART's*/
	CLOCK_AttachClk(kFRO12M_to_FLEXCOMM1);
	CLOCK_AttachClk(kFRO12M_to_FLEXCOMM2);
	CLOCK_AttachClk(kFRO12M_to_FLEXCOMM3);
//	/* For I2C master */
	CLOCK_AttachClk(kFRO12M_to_FLEXCOMM4);
//	/* reset FLEXCOMM for I2C */
	RESET_PeripheralReset(kFC4_RST_SHIFT_RSTn);
	/*For CAN peripheral*/
	CLOCK_SetClkDiv(kCLOCK_DivCanClk, 5U, true);
    CLOCK_AttachClk(kMCAN_DIV_to_MCAN);

    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
    /* Init FSL debug console. */
    BOARD_InitDebugConsole();
	/* Clear screen*/
	PRINTF("%c[2J", 27);
	/* Set cursor location at [0,0] */
	PRINTF("%c[0;0H", 27);
#endif

    app_saks_mod_init();

    while(1) {

    }

    return 0 ;
}
