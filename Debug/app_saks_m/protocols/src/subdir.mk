################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../app_saks_m/protocols/src/wifi_ble_uart_frame.c \
../app_saks_m/protocols/src/wifi_ble_uart_transport.c \
../app_saks_m/protocols/src/winsen.c \
../app_saks_m/protocols/src/winstar_canopen.c 

C_DEPS += \
./app_saks_m/protocols/src/wifi_ble_uart_frame.d \
./app_saks_m/protocols/src/wifi_ble_uart_transport.d \
./app_saks_m/protocols/src/winsen.d \
./app_saks_m/protocols/src/winstar_canopen.d 

OBJS += \
./app_saks_m/protocols/src/wifi_ble_uart_frame.o \
./app_saks_m/protocols/src/wifi_ble_uart_transport.o \
./app_saks_m/protocols/src/winsen.o \
./app_saks_m/protocols/src/winstar_canopen.o 


# Each subdirectory must supply rules for building sources it contributes
app_saks_m/protocols/src/%.o: ../app_saks_m/protocols/src/%.c app_saks_m/protocols/src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DCPU_LPC55S16JBD100 -DCPU_LPC55S16JBD100_cm33 -DSERIAL_PORT_TYPE_UART=1 -DSDK_DEBUGCONSOLE=1 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -DSDK_OS_FREE_RTOS -DSDK_OS_BAREMETAL -I"C:\Users\plibreros\Documents\Desarrollo ASBA Proyecto N2\Firmware_LPC55S16\SAKs_Mod_V0\drivers" -I"C:\Users\plibreros\Documents\Desarrollo ASBA Proyecto N2\Firmware_LPC55S16\SAKs_Mod_V0\component\serial_manager" -I"C:\Users\plibreros\Documents\Desarrollo ASBA Proyecto N2\Firmware_LPC55S16\SAKs_Mod_V0\utilities\debug_console" -I"C:\Users\plibreros\Documents\Desarrollo ASBA Proyecto N2\Firmware_LPC55S16\SAKs_Mod_V0\utilities\debug_console\config" -I"C:\Users\plibreros\Documents\Desarrollo ASBA Proyecto N2\Firmware_LPC55S16\SAKs_Mod_V0\utilities\str" -I"C:\Users\plibreros\Documents\Desarrollo ASBA Proyecto N2\Firmware_LPC55S16\SAKs_Mod_V0\component\uart" -I"C:\Users\plibreros\Documents\Desarrollo ASBA Proyecto N2\Firmware_LPC55S16\SAKs_Mod_V0\device" -I"C:\Users\plibreros\Documents\Desarrollo ASBA Proyecto N2\Firmware_LPC55S16\SAKs_Mod_V0\device\periph2" -I"C:\Users\plibreros\Documents\Desarrollo ASBA Proyecto N2\Firmware_LPC55S16\SAKs_Mod_V0\CMSIS" -I"C:\Users\plibreros\Documents\Desarrollo ASBA Proyecto N2\Firmware_LPC55S16\SAKs_Mod_V0\CMSIS\m-profile" -I"C:\Users\plibreros\Documents\Desarrollo ASBA Proyecto N2\Firmware_LPC55S16\SAKs_Mod_V0\component\lists" -I"C:\Users\plibreros\Documents\Desarrollo ASBA Proyecto N2\Firmware_LPC55S16\SAKs_Mod_V0\utilities" -I"C:\Users\plibreros\Documents\Desarrollo ASBA Proyecto N2\Firmware_LPC55S16\SAKs_Mod_V0\freertos\freertos-kernel\include" -I"C:\Users\plibreros\Documents\Desarrollo ASBA Proyecto N2\Firmware_LPC55S16\SAKs_Mod_V0\freertos\freertos-kernel\portable\GCC\ARM_CM33_NTZ\non_secure" -I"C:\Users\plibreros\Documents\Desarrollo ASBA Proyecto N2\Firmware_LPC55S16\SAKs_Mod_V0\board" -I"C:\Users\plibreros\Documents\Desarrollo ASBA Proyecto N2\Firmware_LPC55S16\SAKs_Mod_V0\freertos\freertos-kernel\template" -I"C:\Users\plibreros\Documents\Desarrollo ASBA Proyecto N2\Firmware_LPC55S16\SAKs_Mod_V0\freertos\freertos-kernel\template\ARM_CM33_3_priority_bits" -I"C:\Users\plibreros\Documents\Desarrollo ASBA Proyecto N2\Firmware_LPC55S16\SAKs_Mod_V0\source" -I"C:\Users\plibreros\Documents\Desarrollo ASBA Proyecto N2\Firmware_LPC55S16\SAKs_Mod_V0\app_saks_m" -I"C:\Users\plibreros\Documents\Desarrollo ASBA Proyecto N2\Firmware_LPC55S16\SAKs_Mod_V0\app_saks_m\drivers\src" -I"C:\Users\plibreros\Documents\Desarrollo ASBA Proyecto N2\Firmware_LPC55S16\SAKs_Mod_V0\app_saks_m\drivers\inc" -I"C:\Users\plibreros\Documents\Desarrollo ASBA Proyecto N2\Firmware_LPC55S16\SAKs_Mod_V0\app_saks_m\app\inc" -I"C:\Users\plibreros\Documents\Desarrollo ASBA Proyecto N2\Firmware_LPC55S16\SAKs_Mod_V0\app_saks_m\app\src" -I"C:\Users\plibreros\Documents\Desarrollo ASBA Proyecto N2\Firmware_LPC55S16\SAKs_Mod_V0\app_saks_m\devices\src" -I"C:\Users\plibreros\Documents\Desarrollo ASBA Proyecto N2\Firmware_LPC55S16\SAKs_Mod_V0\app_saks_m\devices\inc" -I"C:\Users\plibreros\Documents\Desarrollo ASBA Proyecto N2\Firmware_LPC55S16\SAKs_Mod_V0\app_saks_m\protocols\inc" -I"C:\Users\plibreros\Documents\Desarrollo ASBA Proyecto N2\Firmware_LPC55S16\SAKs_Mod_V0\app_saks_m\protocols\src" -I"C:\Users\plibreros\Documents\Desarrollo ASBA Proyecto N2\Firmware_LPC55S16\SAKs_Mod_V0\app_saks_m\utilities\src" -I"C:\Users\plibreros\Documents\Desarrollo ASBA Proyecto N2\Firmware_LPC55S16\SAKs_Mod_V0\app_saks_m\utilities\inc" -O0 -fno-common -g3 -gdwarf-4 -Wall -c -ffunction-sections -fdata-sections -fno-builtin -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m33 -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-app_saks_m-2f-protocols-2f-src

clean-app_saks_m-2f-protocols-2f-src:
	-$(RM) ./app_saks_m/protocols/src/wifi_ble_uart_frame.d ./app_saks_m/protocols/src/wifi_ble_uart_frame.o ./app_saks_m/protocols/src/wifi_ble_uart_transport.d ./app_saks_m/protocols/src/wifi_ble_uart_transport.o ./app_saks_m/protocols/src/winsen.d ./app_saks_m/protocols/src/winsen.o ./app_saks_m/protocols/src/winstar_canopen.d ./app_saks_m/protocols/src/winstar_canopen.o

.PHONY: clean-app_saks_m-2f-protocols-2f-src

