/*
 * mcp960x.h
 *
 *  Created on: 27/03/2026
 *      Author: plibreros
 */



#ifndef DEVICES_INC_MCP960X_H_
#define DEVICES_INC_MCP960X_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#include "i2c_freertos.h"

#define ERROR_READ_TEMPERATURE		((float)(-100.0f))
#define THERMO_TEMPER_LEN_REGISTER	2U

typedef enum {
	VERSION_MCP9601	= 0x41,
	VERSION_MCP9600	= 0x40
} _MCP960xVersion;

typedef enum {
	MCP960x_Success,
	MCP960x_Error
} _status_mcp960x;

typedef enum {
	Type_K 			= 0x00,
	Type_J 			= 0x01,
	Type_T 			= 0x02,
	Type_N 			= 0x03,
	Type_S 			= 0x04,
	Type_E 			= 0x05,
	Type_B 			= 0x06,
	Type_R 			= 0x07,

	Type_Error		= 0xFF
} _thermoType;

typedef enum {
	cold_0_25		= 0x01,
	cold_0_0625		= 0x00
} _coldResolutions;

typedef enum {
	burst_samples_1 = 0x00,
	burst_samples_2,
	burst_samples_4,
	burst_samples_8,
	burst_samples_16,
	burst_samples_32,
	burst_samples_64,
	burst_samples_128
} _burstSamples;

typedef enum {
	Normal_operation,
	Shutdown_mode,
	Burst_mode
} _shutDowns;

typedef enum {
	Filter_Off 		= 0x00,
	filter_IIR1,
	filter_IIR2,
	filter_IIR3,
	filter_IIR4,
	filter_IIR5,
	filter_IIR6,
	filter_IIR7
} _filters;

typedef enum {
	Thermocouple_Hot_Junction_THOTR				= 0x00,
	Junctions_Temperature_TDELTA				= 0x01,
	Cold_Junction_Temperature_TC				= 0x02,
	Raw_ADC_Data_register						= 0x03,
	STATUS_register								= 0x04,
	Thermocouple_Sensor_Configuration_register	= 0x05,
	Device_Configuration_register				= 0x06,
	Alert_1_Configuration_register				= 0x08,
	Alert_2_Configuration_register				= 0x09,
	Alert_3_Configuration_register				= 0x0A,
	Alert_4_Configuration_register				= 0x0B,
	Alert_1_Hysteresis_register_THYST1			= 0x0C,
	Alert_2_Hysteresis_register_THYST2			= 0x0D,
	Alert_3_Hysteresis_register_THYST3			= 0x0E,
	Alert_4_Hysteresis_register_THYST4			= 0x0F,
	Temperature_Alert_1_Limit_register_TALERT1	= 0x10,
	Temperature_Alert_2_Limit_register_TALERT2	= 0x11,
	Temperature_Alert_3_Limit_register_TALERT3	= 0x12,
	Temperature_Alert_4_Limit_register_TALERT4	= 0x13,
	Device_IDRevision_register					= 0x20
} _registersPointers;

/*! The possible ADC resolution settings */
typedef enum {
	MCP960x_ADCRESOLUTION_18,
	MCP960x_ADCRESOLUTION_16,
	MCP960x_ADCRESOLUTION_14,
	MCP960x_ADCRESOLUTION_12
} _hotResolution;

typedef enum {
	MCP960x_Status_Alert1						= 0x01, ///< Bit flag for alert 1 status
	MCP960x_Status_Alert2 						= 0x02, ///< Bit flag for alert 2 status
	MCP960x_Status_Alert3 						= 0x04, ///< Bit flag for alert 3 status
	MCP960x_Status_Alert4						= 0x08, ///< Bit flag for alert 4 status
	MCP960x_Status_InputRange 					= 0x10, ///< Bit flag for input range
	MCP960x_Status_THUpdate 					= 0x40, ///< Bit flag for TH update
	MCP960x_Status_Burst 						= 0x80, ///< Bit flag for burst complete
	MCP9601_Status_OpenCircuit 					= 0x10, ///< Bit flag for open circuit
	MCP9601_Status_ShortCircuit 				= 0x20	///< Bit flag for short circuit
} _flagsStatus;

typedef enum {
	SENSOR060 	= 0x60,
	SENSOR061	= 0x61,
	SENSOR062	= 0x62,
	SENSOR063	= 0x63,
	SENSOR064	= 0x64,
	SENSOR065	= 0x65,
	SENSOR066	= 0x66,
	SENSOR067	= 0x67
} _idSensors;

typedef struct {
	_MCP960xVersion version;
	_thermoType type;
	_coldResolutions cold_resolution;
	_burstSamples burst_samples;
	_shutDowns operations;
	_filters filters;
	_hotResolution adc_resolution;
	_idSensors id;
	uint8_t hot_temp_upper_byte;
	uint8_t hot_temp_lower_byte;
	uint8_t cold_temp_upper_byte;
	uint8_t cold_temp_lower_byte;
	uint8_t device_status;
} _mcp960x_device;

_status_mcp960x mcp960x_channel_i2c_init(i2c_rtos_t* channel_i2c, I2C_Type *base);
void mcp960x_get_default_config(_mcp960x_device* device);
_status_mcp960x mcp960x_device_init(_mcp960x_device* device, i2c_rtos_t* channel_i2c);
_status_mcp960x mcp960x_read_thermocouple(_mcp960x_device* device, i2c_rtos_t* channel_i2c);
_status_mcp960x mcp960x_read_status(_mcp960x_device* device, i2c_rtos_t* channel_i2c);

#ifdef __cplusplus
}
#endif

#endif /* DEVICES_INC_MCP960X_H_ */
