// =======================================================================
// @file        EEconfig.cpp
//
// @details     Simple EEPROM config storage manager
//
// @author      GiorgioCC (g.crocic@gmail.com) - 2023-08-26
// @modifiedby  GiorgioCC - 2023-08-28 13:06
//
// Copyright (c) 2023 GiorgioCC
// =======================================================================


#include "EEconfig.h"
#include <EEPROM.h>

EEconfig::
EEconfig(void): start(0), size(0), currpos(0), blksize(0)
{}

EEconfig::
EEconfig(uint16_t CfgSize, uint16_t EESize, uint16_t EEStart)
: start(EEStart), size(0), currpos(0)
{
    init(CfgSize, EESize, EEStart);
}

uint16_t EEconfig::
seekStart(void)
{
    currpos = start;
    // while((eeprom_read_byte((uint8_t*)currpos) == 0xFF) && seekNext());
    while((EEPROM.read(currpos) == 0xFF) && seekNext());
    return currpos;
}

bool EEconfig::
seekNext(void)
{
    bool res = true;
    currpos += blksize; 
    if((currpos + blksize) >= (start + size)) {
        currpos = start;
        res = false;
    }
    return res;
}

void EEconfig::
init(uint16_t CfgSize, uint16_t EESize, uint16_t EEStart)
{
    size = 0;   // Assume not inited until it is
    if((CfgSize > EESize-1)
    || (CfgSize == 0)
    || (EESize == 0)) return;
    start   = EEStart;
    size    = EESize;
    blksize = CfgSize + 1;  // Account for "valid/invalid" marker
    
    seekStart();
}

uint8_t  
EEconfig::write(uint8_t *CfgData)
{
    if(!isInited()) return 0;
    // eeprom_write_byte((uint8_t*)currpos, ERASED); // void current record
    EEPROM.write(currpos, ERASED); // void current record
    seekNext();
    // eeprom_write_byte((uint8_t*)currpos, VALID); // mark valid record
    EEPROM.write(currpos, VALID); // mark valid record
    // AVR EEPROM is byte-oriented, so no harm to erase/write cycles
    // in writing single bytes in a loop
    // eeprom_write_block(CfgData, (uint8_t*)(currpos+1), blksize-1);
    for(uint8_t i = 1; i < blksize; i++) {
        EEPROM.write((currpos + i), CfgData[i-1]);
    }
    return blksize-1;
}

uint8_t  
EEconfig::read(uint8_t *CfgData)
{
    if(!isInited()) return 0;
    // eeprom_read_block(CfgData, (uint8_t*)(currpos+1), blksize-1);
    for(uint8_t i = 1; i < blksize; i++) {
        CfgData[i-1] = EEPROM.read(currpos + i);
    }
    return blksize-1;
}


// END EEconfig.cpp
