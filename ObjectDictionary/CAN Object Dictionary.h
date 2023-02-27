/* 
 * File:   CAN Object Dictionary.h
 * Author: Administrator
 *
 * Created on 18 January, 2021, 2:14 PM
 */

#ifndef CAN_OBJECT_DICTIONARY_H
#define	CAN_OBJECT_DICTIONARY_H
#ifdef __cplusplus
extern "C"{
#endif
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>      //null

typedef uint32_t CAN_INDEX_TYPE ;

enum COD_ERROR_CODES{
    COD_NO_ERROR = 0,
    COD_ENTRY_NOT_FOUND,
    COD_VALUE_OUTSIDE_OF_BOUNDS
};

typedef uint8_t COD_STORAGE_ADDRESS_TYPE;
typedef uint8_t* COD_STORAGE_DATA_TYPE;
typedef uint8_t COD_RECALL_DATA_TYPE;

typedef void                    (*StoreValueInCOD_t) (COD_STORAGE_ADDRESS_TYPE,COD_STORAGE_DATA_TYPE,uint8_t);
typedef COD_RECALL_DATA_TYPE    (*RecallValueInCOD_t)(COD_STORAGE_ADDRESS_TYPE);

void SetCODStorageFunction(StoreValueInCOD_t NewFunc);
void SetCODRecallFunction(RecallValueInCOD_t NewFunc);
//CAN-Object Dictionary
struct C_OD_ENTRY
{
    uint32_t max_value;
    uint32_t min_value; 
    uint8_t default_value[10];
    uint8_t read_index;
    uint8_t Start_Address;
    uint8_t length;
};
typedef uint8_t COD_ERROR_CODE;
///populate the C_OD_ENTRY struct from EEPROM; 
///keep track of current EEPROM address
///write default values to EEPROM during factory reset
void  CHECK_COD_ENTRY(  CAN_INDEX_TYPE CAN_INDEX,CAN_INDEX_TYPE REFERENCE_CAN_INDEX,
                        uint8_t *Mem_Address, uint8_t default_value[],
                        uint8_t mem_usage,uint32_t max_value,uint32_t min_value,
                        bool Factory_Reset_Enabled,struct C_OD_ENTRY *OBJECT_DATA );
///populate C_OD_ENTRY struct values
void  setup_COD_data(uint8_t Mem_Address, uint8_t default_value[],uint8_t mem_usage,uint32_t max_value,uint32_t min_value,struct C_OD_ENTRY *OBJECT_DATA);
///converts a can object dictionary entry index to an EEPROM memory address
struct C_OD_ENTRY FIND_CAN_OBJECT(CAN_INDEX_TYPE CAN_INDEX,bool Factory_Reset_Enabled);

struct C_OD_ENTRY *FindMostRecentlyAccessedCODObject(void);
//Allows writing either a specified value "Data_To_Write" or a default value Data_To_Write=NULL
//using the CAN Address
//this is the one folks should use from external files
COD_ERROR_CODE Edit_COD(CAN_INDEX_TYPE CAN_INDEX,uint8_t Data_to_Write[8], uint8_t data_length) ;
//Implements the Write Request from above
//you can call this directly if you already have "Object_Data"
//if you want to write the default value you need to use Edit_COD
//or call function like this Write_to_Code (Object,Object.default_value,Object.length)
COD_ERROR_CODE Write_to_COD(struct C_OD_ENTRY Object_Data, uint8_t Data_to_Write[8], uint8_t data_length);
/************************
read EEPROM address coded as Object-Dictionary address and return value
 * 
 * check_current_value_of_8bit_OD_entry exists to save having to mask with 0xff, as most parameters are 8-bit
************************/
uint8_t check_current_value_of_8bit_OD_entry(CAN_INDEX_TYPE CAN_INDEX, bool check_bounds);
uint16_t check_current_value_of_16bit_OD_entry(CAN_INDEX_TYPE CAN_INDEX, bool check_bounds);
uint32_t check_current_value_of_32bit_OD_entry(CAN_INDEX_TYPE CAN_INDEX, bool check_bounds);

//read data firect from storage without mapping endianess
void ReadRawFromCOD(CAN_INDEX_TYPE CAN_INDEX,uint8_t *OutputData);

#ifdef __cplusplus
}
#endif
#endif	/* CAN_OBJECT_DICTIONARY_H */

