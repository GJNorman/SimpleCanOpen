//
//  FunctionCodes.h
//  Created by GNorman on 26/6/2022.
//

#ifndef FunctionCodes_h
#define FunctionCodes_h

    enum FunctionCodes
    {
        NMT_NODE_CONTROL = 0x000,
        GLOBAL_FAILSAFE_COMMAND = 0x001,
        SYNC_COMMAND = 0x080,
        EMERGENCY = 0x080,
        TIMESTAMP = 0x100,
        NMT_NODE_MONITOR = 0x700,
        PDO_TX_1 = 0x180,               
        PDO_TX_2 = 0x280,               
        PDO_TX_3 = 0x380,               
        PDO_TX_4 = 0x480,               
        PDO_RX_1 = 0x200,
        PDO_RX_2 = 0x300,
        PDO_RX_3 = 0x400,
        PDO_RX_4 = 0x500,
        SDO_TX = 0x580,
        SDO_RX = 0x600,
    };

    enum NMTCodes
    {
        GOTO_OPERATIONAL        = 0x01,
        GOTO_STOPPED            = 0x02,
        GOTO_BOOTLOADER         = 0x03,
        GOTO_PREOPERATIONAL     = 0x80,
        GOTO_RESET_NODE         = 0x81,
        GOTO_RESET_COMMS        = 0x82
    };

    enum NMTHeartBeatCodes
    {
        COB_ID = NMT_NODE_MONITOR,
        BOOT_UP = 0x00,
        STOPPED = 0x04,
        OPERATIONAL = 0x05,
        PRE_OPERATIONAL = 0x7f,
        UNKNOWN = 0xff
    };

    enum SDOCodes{
        SDO_READ = 0x40,
        SDO_WRITE = 0x20,
        CSS_SEGMENT_DOWNLOAD = 0,
        CSS_INITIATE_DOWNLOAD = 1,
        CSS_INITIATE_UPLOAD = 2,
        CSS_SDO_SEGMENT_UPLOAD = 3,
        CSS_SDO_ABORT = 4,
        CSS_SDO_BLOCK_UPLOAD = 5,
        CSS_SDO_BLOCK_DOWNLOAD = 6,
        SDO_MULTI_READ_1 = 0x60,
        SDO_MULTI_READ_2 = 0x70,
        SDO_MULTI_READ_3 = 0x00,
        SDO_MULTI_READ_4 = 0x10
    };
    
    enum SDO_ERROR_CODES{
        SDO_READ_ERROR = 0x80,
    };


#endif /* FunctionCodes_h */
