#include <stdint.h>

/*
 * for holding a single message service-data-object 
 */
    struct SDO_TX_CAN{
       uint16_t Message_ID;
       uint8_t MessageDLC;      //data length code
       uint8_t CCD;
       uint8_t Index_low;
       uint8_t Index_high;
       uint8_t Index_sub;
       uint8_t Data_bytes[4];
    };



/*
 * for holding a multi-message service-data-object 
 */
    struct MULTI_READ_SDO{
        uint16_t Message_ID;
        uint8_t MessageDLC;     //each message is always 8 bytes
        uint8_t valid_length;   //but not all bytes are to be read
        uint8_t Data_bytes[8];
    };