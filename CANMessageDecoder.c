#include "CANMessageDecoder.h"

static CAN_Callback_t CANCallbacks[TOTAL_CAN_CALLBACKS];

void SetCANCallback(CAN_Callback_t NewCallback, uint8_t callbackSpecifier)
{
    CANCallbacks[callbackSpecifier] = NewCallback;
}

void decodeMessage(uint8_t *Data, uint8_t *DLC, uint16_t *NodeIDWithFunctionCode)
{
    uint16_t FunctionCode = (*NodeIDWithFunctionCode & (0x780));

    uint8_t callbackID = UNKNOWN_CAN_CALLBACK;
    switch(FunctionCode)
    {
        case NMT_NODE_CONTROL:
            callbackID=NMT_CALLBACK;
            break;
            
        case NMT_NODE_MONITOR:
            callbackID=NMT_NODE_CALLBACK;
            break;
            
        case SYNC_COMMAND:
            callbackID=SYNC_CALLBACK;
            break;
            
        case TIMESTAMP:
            callbackID=TIMESTAMP_CALLBACK;
            break;
            
        case PDO_TX_1:
            callbackID=PDO_TX1_CALLBACK;
            break;

        case PDO_TX_2:
            callbackID=PDO_TX2_CALLBACK;
            break;
            
        case PDO_TX_3:
            callbackID=PDO_TX3_CALLBACK;
            break;
            
        case PDO_TX_4:
            callbackID=PDO_TX4_CALLBACK;
            break;
            
        case PDO_RX_1:
            callbackID=PDO_RX1_CALLBACK;
            break;
            
        case PDO_RX_2:
            callbackID=PDO_RX2_CALLBACK;
            break;
            
        case PDO_RX_3:
            callbackID=PDO_RX3_CALLBACK;
            break;
            
        case PDO_RX_4:
            callbackID=PDO_RX4_CALLBACK;
            break;
            
        case SDO_RX:
            callbackID=SDO_RX_CALLBACK;
            break;

        case SDO_TX:
            callbackID=SDO_TX_CALLBACK;
            break;
            
        default:
        break;
    }
    if(CANCallbacks[callbackID]!=NULL)
        CANCallbacks[callbackID](Data,DLC,NodeIDWithFunctionCode);
            
}