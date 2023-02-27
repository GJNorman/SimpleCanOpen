#include "CANMessageDecoder.h"
#include "SDO/SDO Server.h"
void processPDOTX1(uint8_t *Data, uint8_t *DLC, uint16_t *NodeIDWithFunctionCode)
{
 printf("PDO Message Processed\n"); 
}

int main(void)
{
  
  SetCANCallback(processPDOTX1,PDO_TX1_CALLBACK);
  
  SetCANCallback(Deal_With_SDO,SDO_TX_CALLBACK);
  
  while(true)
  {
    
    // Read a CAN message and parse into. DATA, Datalength abd nodeID with FunctionCode
    uint8_t Data[8];
    uint8_t DLC;
    uint16_t NodeIDWithFunctionCode;
    decodeMessage(&Data, &DLC, &NodeIDWithFunctionCode);
  }
 
  return 0;
}
