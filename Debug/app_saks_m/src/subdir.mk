################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../app_saks_m/src/ao_app.c \
../app_saks_m/src/app_saksm.c \
../app_saks_m/src/process_extinction.c \
../app_saks_m/src/process_gas.c \
../app_saks_m/src/process_lcd.c \
../app_saks_m/src/process_rtc.c \
../app_saks_m/src/process_temperature.c \
../app_saks_m/src/process_wifible.c \
../app_saks_m/src/task_app.c \
../app_saks_m/src/utilitirs_saks.c 

C_DEPS += \
./app_saks_m/src/ao_app.d \
./app_saks_m/src/app_saksm.d \
./app_saks_m/src/process_extinction.d \
./app_saks_m/src/process_gas.d \
./app_saks_m/src/process_lcd.d \
./app_saks_m/src/process_rtc.d \
./app_saks_m/src/process_temperature.d \
./app_saks_m/src/process_wifible.d \
./app_saks_m/src/task_app.d \
./app_saks_m/src/utilitirs_saks.d 

OBJS += \
./app_saks_m/src/ao_app.o \
./app_saks_m/src/app_saksm.o \
./app_saks_m/src/process_extinction.o \
./app_saks_m/src/process_gas.o \
./app_saks_m/src/process_lcd.o \
./app_saks_m/src/process_rtc.o \
./app_saks_m/src/process_temperature.o \
./app_saks_m/src/process_wifible.o \
./app_saks_m/src/task_app.o \
./app_saks_m/src/utilitirs_saks.o 


# Each subdirectory must supply rules for building sources it contributes
app_saks_m/src/%.o: ../app_saks_m/src/%.c app_saks_m/src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DCPU_LPC55S16JBD100 -DCPU_LPC55S16JBD100_cm33 -DSDK_OS_BAREMETAL -DSERIAL_PORT_TYPE_UART=1 -DSDK_DEBUGCONSOLE=1 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -DSDK_OS_FREE_RTOS -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -I"C:\Users\plibreros\Documents\Desarrollo ASBA Proyecto N2\Firmware_LPC55S16\SAKs_Mod_V0\drivers" -I"C:\Users\plibreros\Documents\Desarrollo ASBA Proyecto N2\Firmware_LPC55S16\SAKs_Mod_V0\component\serial_manager" -I"C:\Users\plibreros\Documents\Desarrollo ASBA Proyecto N2\Firmware_LPC55S16\SAKs_Mod_V0\utilities\debug_console" -I"C:\Users\plibreros\Documents\Desarrollo ASBA Proyecto N2\Firmware_LPC55S16\SAKs_Mod_V0\utilities\debug_console\config" -I"C:\Users\plibreros\Documents\Desarrollo ASBA Proyecto N2\Firmware_LPC55S16\SAKs_Mod_V0\utilities\str" -I"C:\Users\plibreros\Documents\Desarrollo ASBA Proyecto N2\Firmware_LPC55S16\SAKs_Mod_V0\component\uart" -I"C:\Users\plibreros\Documents\Desarrollo ASBA Proyecto N2\Firmware_LPC55S16\SAKs_Mod_V0\device" -I"C:\Users\plibreros\Documents\Desarrollo ASBA Proyecto N2\Firmware_LPC55S16\SAKs_Mod_V0\device\periph2" -I"C:\Users\plibreros\Documents\Desarrollo ASBA Proyecto N2\Firmware_LPC55S16\SAKs_Mod_V0\CMSIS" -I"C:\Users\plibreros\Documents\Desarrollo ASBA Proyecto N2\Firmware_LPC55S16\SAKs_Mod_V0\CMSIS\m-profile" -I"C:\Users\plibreros\Documents\Desarrollo ASBA Proyecto N2\Firmware_LPC55S16\SAKs_Mod_V0\component\lists" -I"C:\Users\plibreros\Documents\Desarrollo ASBA Proyecto N2\Firmware_LPC55S16\SAKs_Mod_V0\utilities" -I"C:\Users\plibreros\Documents\Desarrollo ASBA Proyecto N2\Firmware_LPC55S16\SAKs_Mod_V0\freertos\freertos-kernel\include" -I"C:\Users\plibreros\Documents\Desarrollo ASBA Proyecto N2\Firmware_LPC55S16\SAKs_Mod_V0\freertos\freertos-kernel\portable\GCC\ARM_CM33_NTZ\non_secure" -I"C:\Users\plibreros\Documents\Desarrollo ASBA Proyecto N2\Firmware_LPC55S16\SAKs_Mod_V0\board" -I"C:\Users\plibreros\Documents\Desarrollo ASBA Proyecto N2\Firmware_LPC55S16\SAKs_Mod_V0\freertos\freertos-kernel\template" -I"C:\Users\plibreros\Documents\Desarrollo ASBA Proyecto N2\Firmware_LPC55S16\SAKs_Mod_V0\freertos\freertos-kernel\template\ARM_CM33_3_priority_bits" -I"C:\Users\plibreros\Documents\Desarrollo ASBA Proyecto N2\Firmware_LPC55S16\SAKs_Mod_V0\source" -I"C:\Users\plibreros\Documents\Desarrollo ASBA Proyecto N2\Firmware_LPC55S16\SAKs_Mod_V0\app_saks_m" -I"C:\Users\plibreros\Documents\Desarrollo ASBA Proyecto N2\Firmware_LPC55S16\SAKs_Mod_V0\app_saks_m\inc" -I"C:\Users\plibreros\Documents\Desarrollo ASBA Proyecto N2\Firmware_LPC55S16\SAKs_Mod_V0\app_saks_m\src" -I"C:\Users\plibreros\Documents\Desarrollo ASBA Proyecto N2\Firmware_LPC55S16\SAKs_Mod_V0\app_saks_m\drivers\src" -I"C:\Users\plibreros\Documents\Desarrollo ASBA Proyecto N2\Firmware_LPC55S16\SAKs_Mod_V0\app_saks_m\drivers\inc" -O0 -fno-common -g3 -gdwarf-4 -Wall -c -ffunction-sections -fdata-sections -fno-builtin -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m33 -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-app_saks_m-2f-src

clean-app_saks_m-2f-src:
	-$(RM) ./app_saks_m/src/ao_app.d ./app_saks_m/src/ao_app.o ./app_saks_m/src/app_saksm.d ./app_saks_m/src/app_saksm.o ./app_saks_m/src/process_extinction.d ./app_saks_m/src/process_extinction.o ./app_saks_m/src/process_gas.d ./app_saks_m/src/process_gas.o ./app_saks_m/src/process_lcd.d ./app_saks_m/src/process_lcd.o ./app_saks_m/src/process_rtc.d ./app_saks_m/src/process_rtc.o ./app_saks_m/src/process_temperature.d ./app_saks_m/src/process_temperature.o ./app_saks_m/src/process_wifible.d ./app_saks_m/src/process_wifible.o ./app_saks_m/src/task_app.d ./app_saks_m/src/task_app.o ./app_saks_m/src/utilitirs_saks.d ./app_saks_m/src/utilitirs_saks.o

.PHONY: clean-app_saks_m-2f-src

