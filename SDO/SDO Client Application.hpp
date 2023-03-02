

#ifndef SDO_APPLICATION_H
#define SDO_APPLICATION_H

#include <stdint.h>
#include "SDO Client.h"
#include "Timer.h"                  // timout counter

typedef void (*SDO_Complete_Callback_t)(uint8_t *Data, uint8_t len);

typedef void (*CAN_WRITE_CALLBACK_t)(uint8_t Channel, struct *SDO_TX_CAN);

void setSDOAppCANWriteCallback(CAN_WRITE_CALLBACK_t NewCallback);

void SDOApplication(uint8_t Channel, 
                    TimerClass &SDOTimeOutCounter,
                    bool &SendInitialSDO, 
                    SDO_Complete_Callback_t FinaliseProcess,
                    uint32_t CAN_INDEX,
                    uint8_t ServerNode);


/*
example usage

setSDOAppCANWriteCallback(YOUR_CAN_WRITE_FUNCTION);

bool SendInitialSDO = true;
TimerClass SDOTimeOutCounter(5);                        // 5 second timout
while(1)
{
    // do some stuff

    // process next SDO
    SDOApplication( Channel, 
                    SDOTimeOutCounter,
                    SendInitialSDO, 
                    OptionalCallbackOnCompletion,
                    HARDWARE_VERSION,                         //CAN_INDEX
                    17);                                      // server ID
    // do some other stuff
}
*/
#endif //SDO_APPLICATION_H
