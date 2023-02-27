#include "SDO Application.h"
void processSDOClient(uint8_t Channel, 
                    TimerClass &SDOTimeOutCounter,
                    bool &SendInitialSDO, 
                    SDO_Complete_Callback_t FinaliseProcess,
                    uint32_t CAN_INDEX,
                    uint8_t ServerNode)
{
	struct SDO_TX_CAN  newSDO = {};
	CANMsg_t MessageBuffer = {};
	CANTimestamp_t TimestampBuffer = {};
		
	if(checkSDOProgress() == SDO_MSG_READY_IN_BUFFER)
	{
		uint8_t len=0;
		uint8_t *Data=ReadSDOBuffer(&len);

        if(FinaliseProcess!=NULL)
		    FinaliseProcess(Data,len);
		
	}
	else if(checkSDOProgress() == SDO_STILL_IN_PROGESS)
	{
		newSDO = automateNextSDO();
	}
	if(SendInitialSDO == true)
	{
		SendInitialSDO = false;
		newSDO = setup_SDO_Read_Request(ServerNode, CAN_INDEX);
	}
	if(newSDO.Message_ID != 0)
	{
		SDOTimeOutCounter.updateTimerReference();
		for(uint8_t index=0;index<8;index++)
		{
			MessageBuffer.DATA[index] = 0;
		}
		MessageBuffer.DATA[0] = newSDO.CCD;
		MessageBuffer.DATA[1] = newSDO.Index_low;
		MessageBuffer.DATA[2] = newSDO.Index_high;
		MessageBuffer.DATA[3] = newSDO.Index_sub;
		MessageBuffer.ID = newSDO.Message_ID;
		MessageBuffer.LEN = newSDO.MessageDLC;

		//TODO - add error handling
		blockingCanWrite(Channel,&MessageBuffer);
	}
	else
	{
		if (SDOTimeOutCounter.checkTimeout( ) == true)
		{
			SDOTimeOutCounter.updateTimerReference();

			SendInitialSDO=true;
		}
	}
}
