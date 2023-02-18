
#ifndef CAN_MSG_DECODER_h
#define CAN_MSG_DECODER_h

#ifdef __cplusplus
extern "C"{
#endif
#include <stdint.h>
#include "Function Codes/FunctionCodes.h"
#include <stdio.h>  //null

enum CANCallbackPositions{
    PDO_TX1_CALLBACK = 0,
    PDO_TX2_CALLBACK,
    PDO_TX3_CALLBACK,
    PDO_TX4_CALLBACK,
    PDO_RX1_CALLBACK ,
    PDO_RX2_CALLBACK,
    PDO_RX3_CALLBACK,
    PDO_RX4_CALLBACK,
    SDO_TX_CALLBACK,
    SDO_RX_CALLBACK,
    NMT_CALLBACK,
    NMT_NODE_CALLBACK,
    SYNC_CALLBACK,
    TIMESTAMP_CALLBACK,
    UNKNOWN_CAN_CALLBACK,
    TOTAL_CAN_CALLBACKS
};
typedef void (*CAN_Callback_t)(uint8_t *Data, uint8_t *DLC, uint16_t *NodeIDWithFunctionCode);

void decodeMessage(uint8_t *Data, uint8_t *DLC, uint16_t *NodeIDWithFunctionCode);

void SetCANCallback(CAN_Callback_t NewCallback, uint8_t callbackSpecifier);

#ifdef __cplusplus
}
#endif

#endif