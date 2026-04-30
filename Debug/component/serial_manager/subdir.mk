################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../component/serial_manager/fsl_component_serial_manager.c \
../component/serial_manager/fsl_component_serial_port_uart.c 

C_DEPS += \
./component/serial_manager/fsl_component_serial_manager.d \
./component/serial_manager/fsl_component_serial_port_uart.d 

OBJS += \
./component/serial_manager/fsl_component_serial_manager.o \
./component/serial_manager/fsl_component_serial_port_uart.o 


# Each subdirectory must supply rules for building sources it contributes
component/serial_manager/%.o: ../component/serial_manager/%.c component/serial_manager/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DCPU_LPC55S16JBD100 -DCPU_LPC55S16JBD100_cm33 -DSERIAL_PORT_TYPE_UART=1 -DSDK_DEBUGCONSOLE=1 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -DSDK_OS_FREE_RTOS -DSDK_OS_BAREMETAL -D__NEWLIB__ -I"C:\Users\plibreros\Documents\Desarrollo ASBA Proyecto N2\Firmware_LPC55S16\SAKs_Mod_V0\drivers" -I"C:\Users\plibreros\Documents\Desarrollo ASBA Proyecto N2\Firmware_LPC55S16\SAKs_Mod_V0\component\serial_manager" -I"C:\Users\plibreros\Documents\Desarrollo ASBA Proyecto N2\Firmware_LPC55S16\SAKs_Mod_V0\utilities\debug_console" -I"C:\Users\plibreros\Documents\Desarrollo ASBA Proyecto N2\Firmware_LPC55S16\SAKs_Mod_V0\utilities\debug_console\config" -I"C:\Users\plibreros\Documents\Desarrollo ASBA Proyecto N2\Firmware_LPC55S16\SAKs_Mod_V0\utilities\str" -I"C:\Users\plibreros\Documents\Desarrollo ASBA Proyecto N2\Firmware_LPC55S16\SAKs_Mod_V0\component\uart" -I"C:\Users\plibreros\Documents\Desarrollo ASBA Proyecto N2\Firmware_LPC55S16\SAKs_Mod_V0\device" -I"C:\Users\plibreros\Documents\Desarrollo ASBA Proyecto N2\Firmware_LPC55S16\SAKs_Mod_V0\device\periph2" -I"C:\Users\plibreros\Documents\Desarrollo ASBA Proyecto N2\Firmware_LPC55S16\SAKs_Mod_V0\CMSIS" -I"C:\Users\plibreros\Documents\Desarrollo ASBA Proyecto N2\Firmware_LPC55S16\SAKs_Mod_V0\CMSIS\m-profile" -I"C:\Users\plibreros\Documents\Desarrollo ASBA Proyecto N2\Firmware_LPC55S16\SAKs_Mod_V0\component\lists" -I"C:\Users\plibreros\Documents\Desarrollo ASBA Proyecto N2\Firmware_LPC55S16\SAKs_Mod_V0\utilities" -I"C:\Users\plibreros\Documents\Desarrollo ASBA Proyecto N2\Firmware_LPC55S16\SAKs_Mod_V0\freertos\freertos-kernel\include" -I"C:\Users\plibreros\Documents\Desarrollo ASBA Proyecto N2\Firmware_LPC55S16\SAKs_Mod_V0\freertos\freertos-kernel\portable\GCC\ARM_CM33_NTZ\non_secure" -I"C:\Users\plibreros\Documents\Desarrollo ASBA Proyecto N2\Firmware_LPC55S16\SAKs_Mod_V0\board" -I"C:\Users\plibreros\Documents\Desarrollo ASBA Proyecto N2\Firmware_LPC55S16\SAKs_Mod_V0\freertos\freertos-kernel\template" -I"C:\Users\plibreros\Documents\Desarrollo ASBA Proyecto N2\Firmware_LPC55S16\SAKs_Mod_V0\freertos\freertos-kernel\template\ARM_CM33_3_priority_bits" -I"C:\Users\plibreros\Documents\Desarrollo ASBA Proyecto N2\Firmware_LPC55S16\SAKs_Mod_V0\source" -I"C:\Users\plibreros\Documents\Desarrollo ASBA Proyecto N2\Firmware_LPC55S16\SAKs_Mod_V0\app_saks_m" -I"C:\Users\plibreros\Documents\Desarrollo ASBA Proyecto N2\Firmware_LPC55S16\SAKs_Mod_V0\app_saks_m\drivers\src" -I"C:\Users\plibreros\Documents\Desarrollo ASBA Proyecto N2\Firmware_LPC55S16\SAKs_Mod_V0\app_saks_m\drivers\inc" -I"C:\Users\plibreros\Documents\Desarrollo ASBA Proyecto N2\Firmware_LPC55S16\SAKs_Mod_V0\app_saks_m\app\inc" -I"C:\Users\plibreros\Documents\Desarrollo ASBA Proyecto N2\Firmware_LPC55S16\SAKs_Mod_V0\app_saks_m\app\src" -I"C:\Users\plibreros\Documents\Desarrollo ASBA Proyecto N2\Firmware_LPC55S16\SAKs_Mod_V0\app_saks_m\devices\src" -I"C:\Users\plibreros\Documents\Desarrollo ASBA Proyecto N2\Firmware_LPC55S16\SAKs_Mod_V0\app_saks_m\devices\inc" -I"C:\Users\plibreros\Documents\Desarrollo ASBA Proyecto N2\Firmware_LPC55S16\SAKs_Mod_V0\app_saks_m\protocols\inc" -I"C:\Users\plibreros\Documents\Desarrollo ASBA Proyecto N2\Firmware_LPC55S16\SAKs_Mod_V0\app_saks_m\protocols\src" -I"C:\Users\plibreros\Documents\Desarrollo ASBA Proyecto N2\Firmware_LPC55S16\SAKs_Mod_V0\app_saks_m\utilities\src" -I"C:\Users\plibreros\Documents\Desarrollo ASBA Proyecto N2\Firmware_LPC55S16\SAKs_Mod_V0\app_saks_m\utilities\inc" -I"C:\Users\plibreros\Documents\Desarrollo ASBA Proyecto N2\Firmware_LPC55S16\SAKs_Mod_V0\app_saks_m\fsm\inc" -I"C:\Users\plibreros\Documents\Desarrollo ASBA Proyecto N2\Firmware_LPC55S16\SAKs_Mod_V0\app_saks_m\fsm\src" -O0 -fno-common -g3 -gdwarf-4 -Wall -c -ffunction-sections -fdata-sections -fno-builtin -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m33 -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -D__NEWLIB__ -fstack-usage -specs=nano.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-component-2f-serial_manager

clean-component-2f-serial_manager:
	-$(RM) ./component/serial_manager/fsl_component_serial_manager.d ./component/serial_manager/fsl_component_serial_manager.o ./component/serial_manager/fsl_component_serial_port_uart.d ./component/serial_manager/fsl_component_serial_port_uart.o

.PHONY: clean-component-2f-serial_manager

