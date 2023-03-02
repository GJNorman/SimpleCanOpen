#include "CAN Object Dictionary.h"
#include <stdint.h>

#define DEVICE_TYPE                     0x100000 
#define DEVICE_NAME_INDEX               0x100800
#define HARDWARE_VERSION_INDEX          0x100900


void WriteToEEPROM(uint8_t address, uint8_t value[], uint8_t len)
{
  // as per requirements
  
}
uint8_t ReadFromEEPROM(uint8_t address)
{
  // as per requirements
  
}
int main (void)
{
  // set read and write callbacks
  SetCODStorageFunction(WriteToEEPROM);
  SetCODRecallFunction(ReadFromEEPROM);
  // populate the COD
  FIND_CAN_OBJECT(0,true);  // factory reset mode -> populate with default values
  
  
  
  // this will grab all of the information on the can object
  struct C_OD_ENTRY DeviceName = FIND_CAN_OBJECT(DEVICE_NAME_INDEX,false) ;
  
  
  // this will grab the value
  uint32_t currentValueOfDeviceType = check_current_value_of_32bit_OD_entry(DEVICE_TYPE,false); 
  
  
  // this will overwrite the value
  uint8_t array[] = {1,2,3,4};
  
  Edit_COD(HARDWARE_VERSION_INDEX,array, sizeof(array)); 
  
  return 0; 
}
struct C_OD_ENTRY FIND_CAN_OBJECT(CAN_INDEX_TYPE CAN_INDEX,bool Factory_Reset_Enabled)
{
 
    struct C_OD_ENTRY OBJECT_DATA;

    uint8_t default_value[10];
    uint8_t default_value_index = 0;

 /**
 * device type
 */

  default_value_index=0; 
  default_value[default_value_index++] = 'T'; 
  default_value[default_value_index++] = 'E'; 
  default_value[default_value_index++] = 'S';
  default_value[default_value_index++] = 'T';
  CHECK_COD_ENTRY(CAN_INDEX,DEVICE_TYPE,&total_mem_use, default_value,default_value_index,0xffffffff,0,Factory_Reset_Enabled,&OBJECT_DATA);
 
   /**
   * device name
   */

  default_value_index=0; 
  default_value[default_value_index++] = 'T';
  default_value[default_value_index++] = 'E';
  default_value[default_value_index++] = 'S';
  default_value[default_value_index++] = 'T';
  CHECK_COD_ENTRY(CAN_INDEX,DEVICE_NAME_INDEX,&total_mem_use, default_value,default_value_index,0xffffffff,0,Factory_Reset_Enabled,&OBJECT_DATA);

  
  /*
  hardware version
  */
  default_value_index=0; 
  default_value[default_value_index++] = 'T';
  default_value[default_value_index++] = 'E';
  default_value[default_value_index++] = 'S';
  default_value[default_value_index++] = 'T';
  CHECK_COD_ENTRY(CAN_INDEX,HARDWARE_VERSION_INDEX,&total_mem_use, default_value,default_value_index,0xffffffff,0,Factory_Reset_Enabled,&OBJECT_DATA);

  return OBJECT_DATA;
}

