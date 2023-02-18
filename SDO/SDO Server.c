/* 
 * File:   SDO Server
 * Author: Administrator
 *
 * Created on 7 December, 2021, 11:14 AM
 */

#include "SDO Server.h"
static SDO_Application_Callback_t Process_SDO_REQUEST;
void setSDOProcessCallback(SDO_Application_Callback_t NewCallback)
{
    Process_SDO_REQUEST = NewCallback;
}
void HANDLE_SDO_READ_REQUEST(struct SDO_TX_CAN *Response,CAN_INDEX_TYPE CAN_Index)
{    
    
    struct C_OD_ENTRY Object_Data=FIND_CAN_OBJECT(CAN_Index,0);
    
    if(Object_Data.length==0)   //if the object isn't found inside the Can-object dictionary, it will have a length of zero
    {    
        ///the Can Index was not found inside the Can Object dictionary
        Response->CCD = SDO_READ_ERROR;            //ERROR #1
        Response->Data_bytes[0]=0x01;    
    }
    /*
     * determine if this is a single or multi response protocol
     */
    
    //the standard SDO response can only contain 4 data bytes
    if(Object_Data.length<=4)
    {
        // 32 bit read
        ReadRawFromCOD(CAN_Index,Response->Data_bytes);
        
        Response->CCD=0x53-Object_Data.length*4;    //encodes how many bytes to NOT read
    }
    else    //use the extended response
    {
        Response->CCD=0x41;                         //code word for extended response
        Response->Data_bytes[0]=Object_Data.length; //only one data byte is sent, which contains the total entry size in bytes
    }
}


void HANDLE_SDO_WRITE_REQUEST(struct SDO_TX_CAN *Response,CAN_INDEX_TYPE CAN_Index,uint8_t Msg[8])
{
    Response->CCD=0x60; //code for successful write

    uint8_t Write_Status = Edit_COD(CAN_Index,&Msg[4],4);

    if(Write_Status!=0)
    {
        ///80h indicates an error code 
        //here the error indicates that the user tried to write a value outside of the min<=x<=max range
        Response->CCD = 0x80;            
        Response->Data_bytes[0]=Write_Status;    
    }
}
//respond to a service-data object protocol request
void Deal_With_SDO(uint8_t Msg[8], uint8_t *DLC, uint16_t *NodeID)
{
    
    if ((Msg[0]==SDO_MULTI_READ_1)||(Msg[0]==SDO_MULTI_READ_2))
    {
        deal_with_multi_read_SDO(NodeID);
        
        return;
    }
	struct SDO_TX_CAN *Response = (struct SDO_TX_CAN*)NodeID;    // Node ID is the message header
	///setup as per SDO protocol
    Response->Message_ID -= (SDO_RX - SDO_TX);//+Assigned_Node_ID;
    
    CAN_INDEX_TYPE Index = (CAN_INDEX_TYPE)(Response->Index_low<<8)+((CAN_INDEX_TYPE)Response->Index_high<<16)+Response->Index_sub;
    
    uint8_t check_high_byte=Msg[0]&0xf0;
    

        if(check_high_byte==SDO_READ)//read request
        {
            HANDLE_SDO_READ_REQUEST(Response,Index);
        }
        if(check_high_byte==SDO_WRITE)//write request
        {
            HANDLE_SDO_WRITE_REQUEST(Response, Index,Msg);
        }


    //after a write request we check if any process needs to be executed
    if(check_high_byte==SDO_WRITE)//write request
    {
        if(Response->CCD != SDO_READ_ERROR)//write did not result in an error
        {   
            if(Process_SDO_REQUEST!=0)
                Process_SDO_REQUEST(Index,Msg);
        }
    }
}
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
void initialise_multi_read_response(struct MULTI_READ_SDO *Response)
{
    ///setup as per SDO protocol
	Response->Message_ID -= (SDO_RX - SDO_TX);
	Response->MessageDLC=0x8;
      
    //we aren't barbarians 
    for(uint8_t i = 0; i<7 ; i++)
    {
        Response->Data_bytes[i]=0;
    } 
    
    ///handshake alternates between 00 and 0x10 (offset by 0x60 from incoming message)   
    Response->valid_length -= 0x60;
    
    //first byte also indicates the number of valid bytes in the message
    
    struct C_OD_ENTRY *Last_accessed_object = FindMostRecentlyAccessedCODObject();
    
    uint8_t Message_length=Last_accessed_object->length -Last_accessed_object->read_index;
    
    if(Message_length <7)
    {
        Response->valid_length +=2* (7-(Message_length)) + 1;
    }
    else
    {
        Message_length=7;
    }
    //remaining 7 bytes are the message
   
    for(uint8_t i = 0; i<Message_length ; i++)
    {
        Response->Data_bytes[i]=Last_accessed_object->default_value[Last_accessed_object->read_index];
        Last_accessed_object->read_index++;
    } 
}

void deal_with_multi_read_SDO(uint16_t *CAN_Message_ptr)
{     
    struct MULTI_READ_SDO *Response = (struct MULTI_READ_SDO*)CAN_Message_ptr;
    
    initialise_multi_read_response(Response);
       
}
