#include "CAN Object Dictionary.h"

static struct C_OD_ENTRY MostRecentlyAccessObject;      // for multi-read/write operations

static StoreValueInCOD_t    COD_STORAGE_FUNTION;
static RecallValueInCOD_t   COD_RECALL_FUNTION;

struct C_OD_ENTRY FIND_CAN_OBJECT(CAN_INDEX_TYPE CAN_INDEX,bool Factory_Reset_Enabled)
{
    struct C_OD_ENTRY newstruct={};
    return newstruct;
}

void SetCODStorageFunction(StoreValueInCOD_t NewFunc)
{
    COD_STORAGE_FUNTION = NewFunc;
}
void SetCODRecallFunction(RecallValueInCOD_t NewFunc)
{
    COD_RECALL_FUNTION = NewFunc;
}

struct C_OD_ENTRY *FindMostRecentlyAccessedCODObject(void)
{
    return &MostRecentlyAccessObject;
}
void  CHECK_COD_ENTRY(  CAN_INDEX_TYPE CAN_INDEX,CAN_INDEX_TYPE REFERENCE_CAN_INDEX,
                        uint8_t *Mem_Address, uint8_t default_value[],
                        uint8_t mem_usage,uint32_t max_value,uint32_t min_value,
                        bool Factory_Reset_Enabled,struct C_OD_ENTRY *OBJECT_DATA )
{
 
	if ((CAN_INDEX==REFERENCE_CAN_INDEX)||(Factory_Reset_Enabled==1))
	{
		setup_COD_data(*Mem_Address, default_value,mem_usage,max_value,min_value,OBJECT_DATA);
        MostRecentlyAccessObject = *OBJECT_DATA;
	}
    ///increments the EEPROM address
	*Mem_Address+=mem_usage;
	
	if(Factory_Reset_Enabled==1)///write default value to EEPROM
    {
        COD_STORAGE_FUNTION(OBJECT_DATA->Start_Address,OBJECT_DATA->default_value,OBJECT_DATA->length);
    }
	
	return ;
}
///populate C_OD_ENTRY struct values
void  setup_COD_data(uint8_t Mem_Address, uint8_t default_value[],uint8_t mem_usage,uint32_t max_value,uint32_t min_value,struct C_OD_ENTRY *OBJECT_DATA)
{
    OBJECT_DATA->Start_Address = Mem_Address;
    for(uint8_t i = mem_usage; i!=0; i--)   
    {
        OBJECT_DATA->default_value[i-1] = default_value[i-1] ;
    }
    OBJECT_DATA->length     =   mem_usage;
    OBJECT_DATA->max_value  =   max_value;
    OBJECT_DATA->min_value  =   min_value;
    OBJECT_DATA->read_index =   0;

    return ;
}
//Allows writing either a specified value "Data_To_Write" or a default value Data_To_Write=NULL
//using the CAN Address
//this is the one folks should use from external files
COD_ERROR_CODE Edit_COD(CAN_INDEX_TYPE CAN_INDEX,uint8_t Data_to_Write[8], uint8_t data_length) 
{
    struct C_OD_ENTRY Object_Data=FIND_CAN_OBJECT(CAN_INDEX,0);
    
    uint8_t error_code = COD_NO_ERROR ;
    
    if(Object_Data.length == 0 )
    {
        return COD_ENTRY_NOT_FOUND;
    }
    if(Data_to_Write == NULL)
    {
        COD_STORAGE_FUNTION(Object_Data.Start_Address,Object_Data.default_value,Object_Data.length);
    }
    else
    {
        error_code=Write_to_COD(Object_Data, Data_to_Write, data_length);
    }
    return error_code;    
}
//Implements the Write Request from above
//you can call this directly if you already have "Object_Data"
//if you want to write the default value you need to use Edit_COD
//or call function like this Write_to_Code (Object,Object.default_value,Object.length)
COD_ERROR_CODE Write_to_COD(struct C_OD_ENTRY Object_Data, uint8_t Data_to_Write[8], uint8_t data_length)
{
    uint32_t Value_To_Write = 0;
    //little endian madness
    //01 00 03 02 -> 03 02 00 01
    
    uint8_t shift_vals[] = {0,8,24,16};
    for(uint8_t index=0; index< data_length; index++)
    {
        Value_To_Write +=(uint32_t) Data_to_Write[index]<<(shift_vals[index]);
    }
    
    if((Value_To_Write < Object_Data.min_value)
    || (Value_To_Write > Object_Data.max_value))
    {
        return COD_VALUE_OUTSIDE_OF_BOUNDS; //return Error
    }

    COD_STORAGE_FUNTION(Object_Data.Start_Address,Data_to_Write,Object_Data.length);

    return COD_NO_ERROR;
}
/************************
read EEPROM address coded as Object-Dictionary address and return value
 * 
 * check_current_value_of_8bit_OD_entry exists to save having to mask with 0xff, as most parameters are 8-bit
************************/

uint32_t check_current_value_of_32bit_OD_entry(CAN_INDEX_TYPE CAN_INDEX, bool check_bounds)
{
    struct C_OD_ENTRY Object_Data=FIND_CAN_OBJECT(CAN_INDEX,0);
    if(Object_Data.length!=0)   //confirms that index exists
    {
        uint32_t Data_value=0;

        ///little endian nonsense
        for(uint8_t i=0; i <Object_Data.length && i<2; i++)
        {
            Data_value +=   (uint32_t)(COD_RECALL_FUNTION(Object_Data.Start_Address+i)<<(8*i));
        }
        for(uint8_t i=0; i <Object_Data.length-2 && i<2; i++)
        {
            Data_value +=   (uint32_t)(COD_RECALL_FUNTION(Object_Data.Start_Address+i+2)<<(8*(3-i)));
        }
        if(check_bounds==true)///check if min_value<Data_value<Max_value -> will find corrupted data; or fresh install 
        {
            if((Data_value>Object_Data.max_value)||(Data_value<Object_Data.min_value))
            {
                //overwrite invalid data with default value
                 COD_STORAGE_FUNTION(Object_Data.Start_Address,Object_Data.default_value,Object_Data.length);

                 Data_value=COD_RECALL_FUNTION(Object_Data.Start_Address);
            }  
        }
        return Data_value;
    }
    return COD_ENTRY_NOT_FOUND;   
}


uint8_t check_current_value_of_8bit_OD_entry(CAN_INDEX_TYPE CAN_INDEX, bool check_bounds)
{
    return (0xff & check_current_value_of_32bit_OD_entry(CAN_INDEX,check_bounds));
}
uint16_t check_current_value_of_16bit_OD_entry(CAN_INDEX_TYPE CAN_INDEX, bool check_bounds)
{
    return (0xffff & check_current_value_of_32bit_OD_entry(CAN_INDEX,check_bounds));
}

void ReadRawFromCOD(CAN_INDEX_TYPE CAN_INDEX,uint8_t *OutputData)
{
    struct C_OD_ENTRY Object_Data=FIND_CAN_OBJECT(CAN_INDEX,0);
    
    for(uint8_t counter=0; counter< Object_Data.length; counter++)
    {
        OutputData[counter] = COD_RECALL_FUNTION(Object_Data.Start_Address+counter);
    }
}

