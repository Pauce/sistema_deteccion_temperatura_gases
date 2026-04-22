/*
 * mcp960x.c
 *
 *  Created on: 27/03/2026
 *      Author: plibreros
 */
#include "mcp960x.h"

static _status_mcp960x mcp960x_read_register(uint8_t idDevice,
		_registersPointers reg, uint8_t len_data, i2c_rtos_t *channel_i2c);
static _status_mcp960x write_single(uint8_t idDevice, _registersPointers reg,
		i2c_rtos_t *channel_i2c);
static _status_mcp960x read_single(uint8_t idDevice, _registersPointers reg,
		uint8_t len_data, i2c_rtos_t *channel_i2c);
static _status_mcp960x write_two(uint8_t idDevice, _registersPointers reg,
		uint8_t len_data, i2c_rtos_t *channel_i2c);

static _status_mcp960x mcp960x_thermo_sensor_config(_mcp960x_device *device,
		i2c_rtos_t *channel_i2c);
static _status_mcp960x mcp960x_dev_config_register(_mcp960x_device *device,
		i2c_rtos_t *channel_i2c);

void mcp960x_get_default_config(_mcp960x_device *device) {
	memset(device, 0, sizeof(_mcp960x_device));

	device->version = VERSION_MCP9601;
	device->type = Type_K;
	device->cold_resolution = cold_0_0625;
	device->burst_samples = burst_samples_1;
	device->operations = Normal_operation;
	device->filters = filter_IIR7;
	device->adc_resolution = MCP960x_ADCRESOLUTION_18;
}

_status_mcp960x mcp960x_channel_i2c_init(i2c_rtos_t *channel_i2c,
		I2C_Type *base) {
	status_t status = i2c_rtos_init(&channel_i2c->master_rtos_handle, base,
			&channel_i2c->masterConfig);
	if (status != kStatus_Success)
		return MCP960x_Error;

	return MCP960x_Success;
}

_status_mcp960x mcp960x_device_init(_mcp960x_device *device,
		i2c_rtos_t *channel_i2c) {

	if (device == NULL || channel_i2c == NULL)
		return MCP960x_Error;

	if (mcp960x_read_register(device->id, Device_IDRevision_register, 1,
			channel_i2c) != MCP960x_Success)
		return MCP960x_Error;

	if (channel_i2c->g_master_buff_rx[0] != device->version)
		return MCP960x_Error;

	if (mcp960x_thermo_sensor_config(device, channel_i2c) != MCP960x_Success)
		return MCP960x_Error;

	if (mcp960x_dev_config_register(device, channel_i2c) != MCP960x_Success)
		return MCP960x_Error;

	return MCP960x_Success;
}

_status_mcp960x mcp960x_read_thermocouple(_mcp960x_device *device,
		i2c_rtos_t *channel_i2c) {
	if (device == NULL || channel_i2c == NULL)
		return MCP960x_Error;

	if (mcp960x_read_register(device->id, Thermocouple_Hot_Junction_THOTR, 2,
			channel_i2c) != MCP960x_Success)
		return MCP960x_Error;

	device->hot_temp_upper_byte = channel_i2c->g_master_buff_rx[0];
	device->hot_temp_lower_byte = channel_i2c->g_master_buff_rx[1];

	return MCP960x_Success;
}

_status_mcp960x mcp960x_read_status(_mcp960x_device *device,
		i2c_rtos_t *channel_i2c) {
	if (device == NULL || channel_i2c == NULL)
		return MCP960x_Error;

	if (mcp960x_read_register(device->id, STATUS_register, 1, channel_i2c)
			!= MCP960x_Success)
		return MCP960x_Error;

	device->device_status = channel_i2c->g_master_buff_rx[0];

	return MCP960x_Success;
}

static _status_mcp960x mcp960x_thermo_sensor_config(_mcp960x_device *device,
		i2c_rtos_t *channel_i2c) {
	uint8_t data_register = (device->type << 4) | device->filters;
	memset(&channel_i2c->g_master_buff_tx, 0, I2C_DATA_LENGTH);
	channel_i2c->g_master_buff_tx[0] = data_register;

	if (write_two(device->id, Thermocouple_Sensor_Configuration_register, 1,
			channel_i2c) != MCP960x_Success)
		return MCP960x_Error;

	return MCP960x_Success;
}

static _status_mcp960x mcp960x_dev_config_register(_mcp960x_device *device,
		i2c_rtos_t *channel_i2c) {
	uint8_t register_config = (device->cold_resolution << 7)
			| (device->adc_resolution << 5) | (device->burst_samples << 2)
			| device->operations;

	if (write_two(device->id, register_config, 1, channel_i2c)
			!= MCP960x_Success)
		return MCP960x_Error;

	return MCP960x_Success;
}

static _status_mcp960x mcp960x_read_register(uint8_t idDevice,
		_registersPointers reg, uint8_t len_data, i2c_rtos_t *channel_i2c) {
	status_t status = write_single(idDevice, reg, channel_i2c);

	if (status != MCP960x_Success)
		return MCP960x_Error;

	status = read_single(idDevice, reg, len_data, channel_i2c);

	if (status != MCP960x_Success)
		return MCP960x_Error;

	return MCP960x_Success;
}

static _status_mcp960x write_single(uint8_t idDevice, _registersPointers reg,
		i2c_rtos_t *channel_i2c) {
	memset(&channel_i2c->masterXfer, 0, sizeof(i2c_master_transfer_t));

	channel_i2c->masterXfer.slaveAddress = idDevice;
	channel_i2c->masterXfer.direction = kI2C_Write;
	channel_i2c->masterXfer.subaddress = reg;
	channel_i2c->masterXfer.subaddressSize = 1;
	channel_i2c->masterXfer.data = 0;
	channel_i2c->masterXfer.dataSize = 0;
	channel_i2c->masterXfer.flags = kI2C_TransferDefaultFlag;

	status_t status = i2c_rtos_transfer(&channel_i2c->master_rtos_handle,
			&channel_i2c->masterXfer);

	if (status != kStatus_Success)
		return MCP960x_Error;

	return MCP960x_Success;
}

static _status_mcp960x read_single(uint8_t idDevice, _registersPointers reg,
		uint8_t len_data, i2c_rtos_t *channel_i2c) {
	memset(channel_i2c->g_master_buff_rx, 0, I2C_DATA_LENGTH);

	channel_i2c->masterXfer.slaveAddress = idDevice;
	channel_i2c->masterXfer.direction = kI2C_Read;
	channel_i2c->masterXfer.subaddress = 0;
	channel_i2c->masterXfer.subaddressSize = 0;
	channel_i2c->masterXfer.data = channel_i2c->g_master_buff_rx;
	channel_i2c->masterXfer.dataSize = len_data;
	channel_i2c->masterXfer.flags = kI2C_TransferRepeatedStartFlag;

	status_t status = i2c_rtos_transfer(&channel_i2c->master_rtos_handle,
			&channel_i2c->masterXfer);

	if (status != kStatus_Success)
		return MCP960x_Error;

	return MCP960x_Success;
}

static _status_mcp960x write_two(uint8_t idDevice, _registersPointers reg,
		uint8_t len_data, i2c_rtos_t *channel_i2c) {

	channel_i2c->masterXfer.slaveAddress = idDevice;
	channel_i2c->masterXfer.direction = kI2C_Write;
	channel_i2c->masterXfer.subaddress = reg;
	channel_i2c->masterXfer.subaddressSize = 1;
	channel_i2c->masterXfer.data = channel_i2c->g_master_buff_tx;
	channel_i2c->masterXfer.dataSize = len_data;
	channel_i2c->masterXfer.flags = kI2C_TransferDefaultFlag;

	status_t status = i2c_rtos_transfer(&channel_i2c->master_rtos_handle,
			&channel_i2c->masterXfer);

	if (status != kStatus_Success)
		return MCP960x_Error;

	return MCP960x_Success;
}
