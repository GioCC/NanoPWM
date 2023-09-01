// =======================================================================
// @file        EEconfig.h
//
// @details     Simple EEPROM config storage manager
//
// @author      GiorgioCC (g.crocic@gmail.com) - 2023-08-26
// @modifiedby  GiorgioCC - 2023-09-01 17:14
//
// Copyright (c) 2023 GiorgioCC

// =======================================================================
#ifndef __EECONFIG__H__
#define __EECONFIG__H__

#include <stdint.h>
#include <Arduino.h>

class EEconfig
{
private:

    static const uint8_t ERASED = 0xFF;
    static const uint8_t VALID  = 0x00;
    
    uint16_t    base = 0;
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
    bool    isValid(void);

    uint8_t write(uint8_t *CfgData);
    uint8_t read(uint8_t *CfgData);

    // Debug only:
    uint16_t getCurrPos(void)   { return currpos; };
    uint16_t getBase(void)      { return base; };
    uint8_t  getByte(uint16_t pos);
    void     erase(void);

};

#endif  //!__EECONFIG__H__
