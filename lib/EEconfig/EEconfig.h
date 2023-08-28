// =======================================================================
// @file        EEconfig.h
//
// @details     Simple EEPROM config storage manager
//
// @author      GiorgioCC (g.crocic@gmail.com) - 2023-08-26
// @modifiedby  GiorgioCC - 2023-08-26 12:36
//
// Copyright (c) 2023 GiorgioCC

// =======================================================================
#ifndef __EECONFIG__H__
#define __EECONFIG__H__

#include <stdint.h>
#include <EEPROM.h>

class EEconfig
{
private:

    static const uint8_t ERASED = 0xFF;
    static const uint8_t VALID  = 0x00;
    
    uint16_t    start = 0;
    uint16_t    size = 0;
    uint16_t    currpos;
    uint8_t     blksize = 0;

    uint16_t    seekStart(void);
    bool        seekNext(void);

public:

    EEconfig(void);
    EEconfig(uint16_t CfgSize, uint16_t EESize, uint16_t EEStart = 0);
    
    // Remember that 1 byte overhead is added to the payload size.
    // This could be spared if the first byte of a valid payload is guaranteed
    // never to be a 0xFF (or any other given value defined as ERASED);
    // in this case, the marker is part of the payload and 'VALID' const is no 
    // longer required (as are the offsets for the marker byte added to the counters).

    void    init(uint16_t CfgSize, uint16_t EESize, uint16_t EEStart = 0);
    bool    isInited(void) { return (size != 0); }

    uint8_t cfgWrite(uint8_t *CfgData);
    uint8_t cfgRead(uint8_t *CfgData);

};

#endif  //!__EECONFIG__H__
