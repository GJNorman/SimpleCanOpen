#include "SDO Application.h"

static CAN_WRITE_CALLBACK_t CAN_WRITE;
void setSDOAppCANWriteCallback(CAN_WRITE_CALLBACK_t NewCallback)
{
	CAN_WRITE = NewCallback;
}
void processSDOClient(uint8_t Channel, 
                    TimerClass &SDOTimeOutCounter,
                    bool &SendInitialSDO, 
                    SDO_Complete_Callback_t FinaliseProcess,
                    uint32_t CAN_INDEX,
                    uint8_t ServerNode)
{
	struct SDO_TX_CAN  newSDO = {};

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

		if(CAN_WRITE!=NULL)
			CAN_WRITE(Channel,&newSDO);
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
