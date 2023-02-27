

#ifndef SDO_APPLICATION_H
#define SDO_APPLICATION_H

#include <stdint.h>
#include "SDO Client.h"
#include "../../utilities/Timer.h"
#include "../../CAN_Interface.h"
typedef void (*SDO_Complete_Callback_t)(uint8_t *Data, uint8_t len);

void SDOApplication(uint8_t Channel, 
                    TimerClass &SDOTimeOutCounter,
                    bool &SendInitialSDO, 
                    SDO_Complete_Callback_t FinaliseProcess,
                    uint32_t CAN_INDEX,
                    uint8_t ServerNode);


#endif //SDO_APPLICATION_H
