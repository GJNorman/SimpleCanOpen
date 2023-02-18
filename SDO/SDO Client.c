#include "SDO Client.h"

SDO_CLIENT_BUFFER_t SDO_Buffer = {};


uint8_t checkSDOProgress(void)
{
    return SDO_Buffer.SDOProgress;
}
uint8_t * ReadSDOBuffer(uint8_t *len)
{    
    if(len!=NULL)
        *len = SDO_Buffer.MultiRWBufferSize;

    SDO_Buffer.SDOProgress = SDO_NOT_IN_USE;
    return SDO_Buffer.MultiRWBuffer;
}
struct SDO_TX_CAN automateNextSDO(void)
{
    struct SDO_TX_CAN NewMsg = {};
    if(SDO_Buffer.MultiSDORequested == true)
    {
        NewMsg = setupMultiReadHandshake();
        SDO_Buffer.MultiSDORequested = false;
    }
    else if(SDO_Buffer.MultiSDOHandshakeRequired == true)
    {
        NewMsg = setupMultiReadHandshake();
        SDO_Buffer.MultiSDOHandshakeRequired=false;
    }

    return NewMsg;
}
struct SDO_TX_CAN setupMultiReadHandshake(void)
{
    struct SDO_TX_CAN NewSDO =(struct SDO_TX_CAN ) {};
    NewSDO.Message_ID = SDO_Buffer.LastTX.Message_ID + SDO_RX;
    NewSDO.CCD = SDO_Buffer.nextHandShakeValue;
    NewSDO.MessageDLC = 8;

    return NewSDO;
}
void handleSDORX(uint8_t *Data, uint8_t *DLC, uint16_t *NodeIDWithFunctionCode)
{
    //in case of multi master setup (multiple SDOs going on etc.)
    if((*NodeIDWithFunctionCode&0x7ff) -SDO_TX != SDO_Buffer.LastRX.Message_ID - SDO_RX)
    {
        return;
    }
    
    SDO_Buffer.LastTX = (struct SDO_TX_CAN ){0};

    SDO_Buffer.LastTX.CCD = Data[0];
    SDO_Buffer.LastTX.Message_ID = (*NodeIDWithFunctionCode&0x7ff) -SDO_TX;
    SDO_Buffer.LastTX.Index_low  = Data[1];
    SDO_Buffer.LastTX.Index_high = Data[2];
    SDO_Buffer.LastTX.Index_sub  = Data[3];

    for(uint8_t index=0; index<4;index++)
    {
        SDO_Buffer.LastTX.Data_bytes[index] = Data[index+4];
    }

    uint8_t index= 0;
    uint8_t dataStartIndex = 4;
    uint8_t NumberOfBytesInMultiTransfer = 7;
    switch(SDO_Buffer.LastTX.CCD)
    {
        case SDO_READ + CSS_INITIATE_DOWNLOAD:
            SDO_Buffer.MultiSDORequested = true;
            SDO_Buffer.nextHandShakeValue = SDO_MULTI_READ_1;
            SDO_Buffer.MultiRWExpectedBytes = Data[4];
            break;
        case SDO_READ_ERROR:
            SDO_Buffer.SDOErrorDetected = true;
            break;
        case SDO_MULTI_READ_3:
        case SDO_MULTI_READ_4:
            SDO_Buffer.MultiSDOHandshakeRequired = true;
            if(SDO_Buffer.nextHandShakeValue==SDO_MULTI_READ_1)
            {
                SDO_Buffer.nextHandShakeValue = SDO_MULTI_READ_2;
            }
            else
            {
                SDO_Buffer.nextHandShakeValue = SDO_MULTI_READ_1;
            }
        break;

        default:
        //lower nibble can encode the number of bytes to read/write in 
        // the final multi RW SDO transfer
            SDO_Buffer.MultiSDOHandshakeRequired = true;
            if( SDO_Buffer.LastTX.CCD < (SDO_MULTI_READ_4+0x0f))
            {
                //NumberOfBytesInMultiTransfer = 0x17 - 4*(SDO_Buffer.LastTX.CCD &0xf);
                NumberOfBytesInMultiTransfer=SDO_Buffer.MultiRWExpectedBytes;
                SDO_Buffer.SDOProgress = SDO_MSG_READY_IN_BUFFER;
            }
        break;
    }
    //copy remaining bytes into buffer
    if(SDO_Buffer.MultiSDOHandshakeRequired == true)
    {
        dataStartIndex = 1;
    }
    if(SDO_Buffer.MultiSDORequested == false)
    {
        for(index=dataStartIndex;index<1+NumberOfBytesInMultiTransfer;index++)
        {
            SDO_Buffer.MultiRWBuffer[SDO_Buffer.MultiRWBufferSize] = Data[index];
            
            SDO_Buffer.MultiRWBufferSize++;
            if(SDO_Buffer.MultiRWBufferSize == MAXIMUM_BYTES_IN_SDO_DOWNLOAD)
            {
                SDO_Buffer.SDOErrorDetected = true;
                //TODO - add some error info here
                break;
            }
            SDO_Buffer.MultiRWExpectedBytes--;

        }
    }
    

}
struct SDO_TX_CAN setup_SDO_Read_Request(uint16_t NodeID, uint32_t CAN_INDEX)
{
    uint8_t DATA[4] = {0};
    struct SDO_TX_CAN NewSDO = Setup_SDO_Request(NodeID,CAN_INDEX,DATA,4,SDO_READ_REQUEST);
    return NewSDO;
}
struct SDO_TX_CAN Setup_SDO_Request(uint16_t NodeID, uint32_t CAN_INDEX,uint8_t Data[4], uint8_t len, uint8_t ReadOrWrite)
{
    SDO_Buffer.MultiRWBufferSize = 0;
    struct SDO_TX_CAN NewSDO = { .MessageDLC = 8};

    NewSDO.Message_ID = NodeID + SDO_RX;
    NewSDO.Index_high = (CAN_INDEX>>16) & 0xff;
    NewSDO.Index_low = (CAN_INDEX>>8) & 0xff;
    NewSDO.Index_sub = (CAN_INDEX) & 0xff;

    for(uint8_t index =0 ; index< len ; index++)
    {
        NewSDO.Data_bytes[index] = Data[index];
    }
    if(ReadOrWrite==SDO_READ_REQUEST)
    {
        NewSDO.CCD = SDO_READ;
    }
    else
    {
        NewSDO.CCD = 0x33 - len*4;
    }

    SDO_Buffer.LastRX = NewSDO;

    SDO_Buffer.SDOProgress = SDO_STILL_IN_PROGESS;
    SDO_Buffer.LastTX = (struct SDO_TX_CAN ){0};
    return NewSDO;
}