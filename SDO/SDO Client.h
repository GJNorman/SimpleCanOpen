

#ifdef __cplusplus
extern "C"{
#endif
#ifndef SDO_CLIENT_H
#define SDO_CLIENT_H
#include "SDO Data Types.h"
#include "../Function Codes/FunctionCodes.h"
#include <stdbool.h>
#include <stdio.h>
#define MAXIMUM_BYTES_IN_SDO_DOWNLOAD (8*8)
#define SDO_STILL_IN_PROGESS 1
#define SDO_NOT_IN_USE 0

#define SDO_MSG_READY_IN_BUFFER 2
typedef struct 
{
    struct SDO_TX_CAN LastRX;
    struct SDO_TX_CAN LastTX;
    uint8_t MultiRWBuffer[MAXIMUM_BYTES_IN_SDO_DOWNLOAD];
    uint8_t MultiRWExpectedBytes;
    uint8_t MultiRWBufferSize;
    bool MessageReady;              //message is ready from lsat RX
    bool SDOErrorDetected;
    bool MultiSDORequested;        //multi read/write currently underway
    bool MultiSDOHandshakeRequired; //server is ready for next message
    uint8_t SDOProgress;
    uint8_t nextHandShakeValue;
}SDO_CLIENT_BUFFER_t;

#define SDO_READ_REQUEST 0
#define SDO_WRITE_REQUEST 1
//use SDO_READ_REQEUST/SDO_WRITE_REQUEST for ReadOrWrite
struct SDO_TX_CAN Setup_SDO_Request(uint16_t NodeID,uint32_t CAN_INDEX,uint8_t Data[4], uint8_t len, uint8_t ReadOrWrite);

struct SDO_TX_CAN setup_SDO_Read_Request(uint16_t NodeID, uint32_t CAN_INDEX);

void handleSDORX(uint8_t *Data, uint8_t *DLC, uint16_t *NodeIDWithFunctionCode);

struct SDO_TX_CAN setupMultiReadHandshake(void);

uint8_t checkSDOProgress(void);
uint8_t * ReadSDOBuffer(uint8_t *len);

struct SDO_TX_CAN automateNextSDO(void);
#ifdef __cplusplus
}
#endif
#endif//SDO_CLIENT_H