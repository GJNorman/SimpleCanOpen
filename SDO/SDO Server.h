/* 
 * File:   SDO Protocol.h
 * Author: Administrator
 *
 * Created on 7 December, 2021, 11:14 AM
 */

#ifndef SDO_PROTOCOL_H
#define	SDO_PROTOCOL_H

#ifdef __cplusplus
extern "C"{
#endif

#include "../ObjectDictionary/CAN Object Dictionary.h"

#include "../Function Codes/FunctionCodes.h"
#include "SDO Data Types.h"
/****************************
respond to service data object message
****************************/
typedef void (*SDO_Application_Callback_t)(CAN_INDEX_TYPE,uint8_t array[]);

void HANDLE_SDO_READ_REQUEST(struct SDO_TX_CAN *Response,CAN_INDEX_TYPE CAN_Index);


void HANDLE_SDO_WRITE_REQUEST(struct SDO_TX_CAN *Response,CAN_INDEX_TYPE CAN_Index,uint8_t Msg[8]);
//respond to a service-data object protocol request
void Deal_With_SDO(uint8_t Msg[8], uint8_t *DLC, uint16_t *NodeID);
/*
 * for SDO response of more than 4 bytes
 * 
 * this operates over a handshake protocol
 * 
 * 600+ID 8 60 00 00 00 00 00 00 00 
 * 580+ID 8 00 DATA BYTES
 * 600+ID 8 70 00 00 00 00 00 00 00
 * 580+ID 8 10 DATA BYTES
 * 
 * The master alternates between 60 and 70h
 * the slave alternates between 0 and 1h for the upper nibble of the first data byte
 * the lower nibble encodes how many of the data bytes should be read
 * this is calculated as 2* (7-(READ_SIZE)) + 1
 * this is in the standard, i have no idea why you would do it this way
 * seems dumb to me
 */
void initialise_multi_read_response(struct MULTI_READ_SDO *Response);
void deal_with_multi_read_SDO(uint16_t *MsgPtr);


void setSDOProcessCallback(SDO_Application_Callback_t NewCallback);
#ifdef __cplusplus
}
#endif
#endif	/* SDO_PROTOCOL_H */

