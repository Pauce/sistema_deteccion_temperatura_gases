/*
 * winstar_canopen.c
 *
 *  Created on: 31/03/2026
 *      Author: plibreros
 */
#include "winstar_canopen.h"

#define UTF16_BUFFER_LEN			255U
#define MAX_LEN_LCD_DATA			50U
#define DEFAULT_NODE				0x7B
#define BUZZER_INDEX				0x102
#define SIZE_TO_SWITCH_MODE			4U
#define LED_INDICATOR_SUBINX		0x07
#define	TEXT_INDICATOR_SUBINX		0x09
#define WINSTAR_CAN_RECEIVE_SUCCESS 0x60
#define WINSTAR_CAN_RECEIVE_IND1	0x30
#define WINSTAR_CAN_RECEIVE_IND2	0x20
#define MAX_WAIT_CAN_RESP_TICKS		10U

typedef struct {
	uint32_t id;
	uint8_t data[CAN_DATA_SIZE];
} _canMsg_t;

static uint8_t buzzerToggle = 0;

/*Funciones privadas*/
static uint8_t sdo_get_exped_cmd(uint8_t len);
static _winstar_status sdo_explicit_write(mcan_rtos_handle_t *can_open, uint8_t node_id, uint16_t _index, uint8_t subindex, const uint8_t data);
static _winstar_status sdo_expedited_write(mcan_rtos_handle_t *can_open,uint8_t node_id, uint8_t _index, uint8_t subindex, const uint8_t *data, uint8_t len);
static _winstar_status sdo_segment_write(mcan_rtos_handle_t *can_open,uint8_t node_id, uint16_t _index, uint8_t subindex, const uint8_t *text, uint8_t length);
static _winstar_status winstar_send_receive_msg(mcan_rtos_handle_t *can_open, _canMsg_t* myMsg, uint8_t _index);
static _winstar_status winstar_evaluate_response(uint8_t *data_received, uint8_t indexIndicator);
static uint8_t convert_to_utf16(const uint8_t* data, uint8_t* utf16_buffer, uint8_t datLen);
static _winstar_status winstar_send_to_lcd(mcan_rtos_handle_t *can_open, char* stringVal, uint8_t lenString, uint8_t _index, uint8_t subindex);
/*********************************************************************
 * Funciones auxiliares
 *********************************************************************/
static inline uint32_t cob_id_request(uint8_t node_id)  { return 0x600 + node_id; }
static inline uint16_t sdo_get_index(uint16_t _index) { return (uint16_t)(0x2000 + (uint16_t)(_index)); }

_winstar_status winstar_init_peripheral(mcan_rtos_handle_t *can_open, CAN_Type *base){

	if(mcan_rtos_init(can_open, base, (TickType_t)(MAX_WAIT_CAN_RESP_TICKS)) != kStatus_Success)
		return Winstar_Status_Error;

	return Winstar_Status_Ok;
}

_winstar_status winstar_set_ind_value(
	mcan_rtos_handle_t *can_open,
	uint8_t indexIndicator,
	uint8_t value
){

	char stringVal = (char)(value);

	if(winstar_send_to_lcd(can_open, &stringVal, (uint8_t)(1U), indexIndicator, LED_INDICATOR_SUBINX) != Winstar_Status_Ok)
		return Winstar_Status_Error;

	return Winstar_Status_Ok;
}

_winstar_status winstar_set_text_float(
	mcan_rtos_handle_t *can_open,
	uint8_t textIndicator,
	float value
){

	char stringVal[MAX_LEN_LCD_DATA];
	memset(stringVal, 0x00, sizeof(stringVal));

	float_to_str_1decimal(value, stringVal);
	uint8_t lenString = (uint8_t)(strlen(stringVal));

	if(winstar_send_to_lcd(can_open, stringVal, lenString, textIndicator, TEXT_INDICATOR_SUBINX) != Winstar_Status_Ok)
		return Winstar_Status_Error;

	return Winstar_Status_Ok;
}

_winstar_status winstar_set_text_string(
	mcan_rtos_handle_t *can_open,
	uint8_t textIndicator,
	char* info
){

	uint8_t lenString = (uint8_t)(strlen(info));

	if(winstar_send_to_lcd(can_open, info, lenString, textIndicator, TEXT_INDICATOR_SUBINX) != Winstar_Status_Ok)
		return Winstar_Status_Error;

	return Winstar_Status_Ok;
}

_winstar_status winstar_set_text_integer(
	mcan_rtos_handle_t *can_open,
	uint8_t textIndicator,
	uint32_t value
){
	char stringVal[MAX_LEN_LCD_DATA];
	memset(stringVal, 0x00, sizeof(stringVal));

	uint32_to_str(value, stringVal, sizeof(stringVal));
	uint8_t lenString = (uint8_t)(strlen(stringVal));

	if(winstar_send_to_lcd(can_open, stringVal, lenString, textIndicator, TEXT_INDICATOR_SUBINX) != Winstar_Status_Ok)
		return Winstar_Status_Error;

	return Winstar_Status_Ok;
}

_winstar_status winstar_activation_buzzer(mcan_rtos_handle_t *can_open, uint8_t repeat, uint8_t period){

	uint8_t data[4] = {repeat, period, period, buzzerToggle};
	_winstar_status status;

	for(uint8_t i = 0; i < 4; i++){
		if((status = sdo_explicit_write(can_open, (uint8_t)(DEFAULT_NODE), (uint32_t)(BUZZER_INDEX), (uint8_t)(i + 1), (const uint8_t)(data[i]))) != Winstar_Status_Ok) break;
	}

	buzzerToggle = ~buzzerToggle;
	return status;
}

/***************************************************************
 * Tabla de comandos para escritura expedited según tamaño
 ***************************************************************/
static uint8_t sdo_get_exped_cmd(uint8_t len) {
    switch (len) {
        case 1: return 	0x2F; // expedited, 1 byte
        case 2: return 	0x2B; // expedited, 2 bytes
        case 3: return 	0x27; // expedited, 3 bytes
        case 4: return 	0x23; // expedited, 4 bytes
        default: return 0x21; // por defecto
    }
}

/***************************************************************
 * sdo_expedited_write
 * -------------------------------------------------------------
 * Escribe entre 1 y 4 bytes en un índice/subíndice de un nodo CANopen.
 * Se usa para enviar valores pequeños (ej. enteros, banderas, etc.)
 *
 * Parámetros:
 *   node_id   -> ID del nodo destino (ej. 0x7B)
 *   _index     -> Índice del objeto (ej. 0x2000)
 *   subindex  -> Sub-índice (ej. 7)
 *   data      -> Puntero a los datos a enviar (little-endian)
 *   len       -> Tamaño (1–4)
 ***************************************************************/
static _winstar_status sdo_expedited_write(
	mcan_rtos_handle_t *can_open,
	uint8_t node_id,
	uint8_t _index,
	uint8_t subindex,
	const uint8_t *data,
	uint8_t len
){
	if(len > 2) return Winstar_Status_Error;

	_canMsg_t msg;
    memset(&msg, 0, sizeof(msg));

    msg.id =  cob_id_request(node_id);

    uint8_t utf16_data[UTF16_BUFFER_LEN] = {0};
    uint8_t utf16_len = convert_to_utf16(data, utf16_data, len);
    uint16_t indx = sdo_get_index(_index);

    msg.data[0] = sdo_get_exped_cmd(utf16_len); // Byte de comando según longitud

    msg.data[1] = (uint8_t)(indx & 0xFF);  // Index LSB
    msg.data[2] = (uint8_t)(indx >> 8);    // Index MSB
    msg.data[3] = subindex;                // Subíndice

    for(uint8_t i = 0; i < utf16_len; i++){
    	msg.data[4 + i] = utf16_data[i];
    }

//    PRINTF("\r\n[sdo_expedited_write] data: ");
//    for(uint8_t i = 0; i < CAN_DATA_SIZE; i++){
//    	PRINTF("0x%X ", msg.data[i]);
//    }

    return winstar_send_receive_msg(can_open, &msg, _index);
}

/***************************************************************
 * sdo_segment_write
 * -------------------------------------------------------------
 * Envía una cadena (VISIBLE_STRING) de más de 4 bytes (hasta 50)
 * usando transferencia segmentada.
 *
 * Flujo resumido:
 *   1. Enviar "Initiate Download" (comando 0x21) con tamaño total.
 *   2. Esperar acuse del servidor (0x60).
 *   3. Enviar segmentos alternando bit de toggle (0/1).
 *   4. Último segmento lleva flag "no más datos" (bit n=1).
 ***************************************************************/
static _winstar_status sdo_segment_write(
	mcan_rtos_handle_t *can_open,
	uint8_t node_id,
	uint16_t _index,
	uint8_t subindex,
	const uint8_t *text,
	uint8_t length
){
    if(length == 0 || length > MAX_LEN_LCD_DATA) return Winstar_Status_Error;

    _canMsg_t msg;
    memset(&msg, 0, sizeof(msg));

    uint8_t utf16_data[UTF16_BUFFER_LEN] = {0};
    uint8_t utf16_len = convert_to_utf16(text, utf16_data, length);
    uint16_t indx = sdo_get_index(_index);

    /**** Paso 1: iniciar transferencia ****/
    msg.id = cob_id_request(node_id);
    msg.data[0] = sdo_get_exped_cmd(utf16_len); // Byte de comando según longitud
    msg.data[1] = (uint8_t)(indx & 0xFF);
    msg.data[2] = (uint8_t)(indx >> 8);
    msg.data[3] = subindex;
    msg.data[4] = utf16_len; 			// tamaño total (LSB)
    msg.data[5] = 0x00;                	// tamaño total (MSB)

    if(winstar_send_receive_msg(can_open, &msg, _index) != Winstar_Status_Ok)
    	return Winstar_Status_Error;

    /**** Paso 2: enviar segmentos ****/
    uint8_t toggle = 0;
    uint8_t bytes_sent = 0;

    while (bytes_sent < utf16_len) {

        uint8_t remaining = utf16_len - bytes_sent;
        uint8_t chunk = (remaining > 7) ? 7 : remaining;
        memset(msg.data, 0, sizeof(msg.data));

        uint8_t cmd = (toggle ? 0x10 : 0x00); // bit4 = toggle

        bool is_last = ((bytes_sent + chunk) >= utf16_len);
        if (is_last) {
            uint8_t unused = 7 - chunk;    // bytes no usados en el último segmento (0..6)
            cmd |= (unused << 1);          // bits 3..1 = unused
            cmd |= 0x01;                   // bit0 = LAST (último segmento)
        }
        msg.data[0] = cmd;

        for (uint8_t i = 0; i < 7; i++) {
            if (i < chunk)
                msg.data[1 + i] = utf16_data[bytes_sent + i];
            else
                msg.data[1 + i] = 0x00;
        }

        if(winstar_send_receive_msg(can_open, &msg, 0x00) != Winstar_Status_Ok)
        	return Winstar_Status_Error;

        toggle = !toggle;      // alternar toggle bit
        bytes_sent += chunk;
    }

    return Winstar_Status_Ok;
}

static _winstar_status sdo_explicit_write(
	mcan_rtos_handle_t *can_open,
	uint8_t node_id,
	uint16_t _index,
	uint8_t subindex,
	const uint8_t data
){
	_canMsg_t msg;
	memset(&msg, 0, sizeof(msg));

	msg.id =  cob_id_request(node_id);
	uint16_t indx = sdo_get_index(_index);
	msg.data[0] = sdo_get_exped_cmd((uint8_t)(1));

    msg.data[1] = (uint8_t)(indx & 0xFF);  // Index LSB
    msg.data[2] = (uint8_t)(indx >> 8);    // Index MSB
    msg.data[3] = subindex;                // Subíndice
    msg.data[4] = data;

//	PRINTF("\r\n[sdo_explicit_write] Sended: ");
//	for(uint8_t i = 0; i < CAN_DATA_SIZE; i++){
//		PRINTF("0x%X ", msg.data[i]);
//	}

//    if(winstar_send_msg(can_open, &msg) != Winstar_Status_Ok) return Winstar_Status_Error;
//    PRINTF("\r\n[sdo_explicit_write] Pasó envío.");
    return winstar_send_receive_msg(can_open, &msg, _index);
}

static _winstar_status winstar_send_receive_msg(
	mcan_rtos_handle_t *can_open,
	_canMsg_t* myMsg,
	uint8_t _index
){
	uint8_t data_recv[CAN_DATA_SIZE] = {0};

	status_t status = mcan_rtos_transfer_send_receive(
									can_open, myMsg->data,
									CAN_DATA_SIZE,
									myMsg->id,
									data_recv,
									CAN_DATA_SIZE
								);

	if(status != kStatus_Success)
		return Winstar_Status_Error;

	return winstar_evaluate_response(data_recv, _index);
}

static _winstar_status winstar_evaluate_response(
	uint8_t *data_received,
	uint8_t indexIndicator
){
	if(data_received == NULL) return Winstar_Status_Error;

//	PRINTF("\r\nCAN Data : ");
//	for(uint8_t i = 0; i < CAN_DATA_SIZE; i++){
//		PRINTF("0x%X ", data_received[i]);
//	}
//	PRINTF("\r\n");

	switch(data_received[0]){
		case WINSTAR_CAN_RECEIVE_SUCCESS:
			if(data_received[1] == indexIndicator){
				return Winstar_Status_Ok;
			}
			break;
		case WINSTAR_CAN_RECEIVE_IND1:
		case WINSTAR_CAN_RECEIVE_IND2:
			if(data_received[1] == 0x00){
				return Winstar_Status_Ok;
			}
			break;
		default:
			break;
	}

	return Winstar_Status_Error;
}

static uint8_t convert_to_utf16(
	const uint8_t* data,
	uint8_t* utf16_buffer,
	uint8_t datLen
){
	uint8_t utf16_len = 0;

	for(uint8_t i = 0; i < datLen; i++){
		utf16_buffer[utf16_len++] = data[i];
		utf16_buffer[utf16_len++] = 0x00;
	}

	return utf16_len;
}

static _winstar_status winstar_send_to_lcd(
	mcan_rtos_handle_t *can_open,
	char* stringVal,
	uint8_t lenString,
	uint8_t _index,
	uint8_t subindex
){
	_winstar_status status;
	uint32_t bytes_to_send = (uint32_t)(lenString * 2);

//	PRINTF("\r\n[winstar_send_to_lcd] number_to_lcd: ");
//	for(uint8_t i = 0; i < lenString; i++){
//		PRINTF("0x%X ", stringVal[i]);
//	}
//	PRINTF("\r\n[winstar_send_to_lcd] size: %d (x2): %lu", lenString, bytes_to_send);


	if(bytes_to_send <= (uint8_t)(SIZE_TO_SWITCH_MODE)){
		status = sdo_expedited_write(can_open, DEFAULT_NODE, _index, subindex, (const uint8_t*)(stringVal), lenString);
	}else{
		status = sdo_segment_write(can_open, DEFAULT_NODE, _index, subindex, (const uint8_t*)(stringVal), lenString);
	}

	return status;
}
