// =======================================================================
// @file        EEconfig.cpp
//
// @details     Simple EEPROM config storage manager
//
// @author      GiorgioCC (g.crocic@gmail.com) - 2023-08-26
// @modifiedby  GiorgioCC - 2023-09-01 17:29
//
// Copyright (c) 2023 GiorgioCC
// =======================================================================


#include "EEconfig.h"
#include <EEPROM.h>

EEconfig::
EEconfig(void): base(0), size(0), currpos(0), blksize(0)
{}

EEconfig::
EEconfig(uint16_t CfgSize, uint16_t EESize, uint16_t EEStart)
: base(EEStart), size(0), currpos(0)
{
    init(CfgSize, EESize, EEStart);
}

uint16_t EEconfig::
seekStart(void)
{
    currpos = base;
    // while((eeprom_read_byte((uint8_t*)currpos) == 0xFF) && seekNext());
    while((EEPROM.read(currpos) == 0xFF) && seekNext());
    return currpos;
}

bool EEconfig::
seekNext(void)
{
    bool res = true;
    currpos += blksize; 
    if((currpos + blksize) >= (base + size)) {
        currpos = base;
        res = false;
    }
    return res;
}

bool
EEconfig::isValid(void)
{
    return (EEPROM.read(currpos) == VALID);
}

void EEconfig::
init(uint16_t CfgSize, uint16_t EESize, uint16_t EEStart)
{
    size = 0;   // Assume not inited until it is
    if((CfgSize > EESize-1)
    || (CfgSize == 0)
    || (EESize == 0)) return;
    base   = EEStart;
    size    = EESize;
    blksize = CfgSize + 1;  // Account for "valid/invalid" marker
    
    seekStart();
}

uint8_t  
EEconfig::write(uint8_t *CfgData)
{
    if(!isInited()) return 0;
    if(isValid()) {
        // eeprom_write_byte((uint8_t*)currpos, ERASED); // void current record
        EEPROM.write(currpos, ERASED); // void current record
        // Serial.print(F("Write ERASED at: ")); Serial.println(currpos);
        seekNext();
    }
    // eeprom_write_byte((uint8_t*)currpos, VALID); // mark valid record
    EEPROM.write(currpos, VALID); // mark valid record
    // Serial.print(F("Write VALID at: ")); Serial.println(currpos);
    // AVR EEPROM is byte-oriented, so no harm to erase/write cycles
    // in writing single bytes in a loop
    // eeprom_write_block(CfgData, (uint8_t*)(currpos+1), blksize-1);
    for(uint8_t i = 1; i < blksize; i++) {
        EEPROM.write((currpos + i), CfgData[i-1]);
        // Serial.print(F("Write data: ")); Serial.print(CfgData[i-1]);
        // Serial.print(F(" at: ")); Serial.println(currpos + i);
    }
    return blksize-1;
}

uint8_t  
EEconfig::read(uint8_t *CfgData)
{
    if(!isInited() || !isValid()) return 0;

    // eeprom_read_block(CfgData, (uint8_t*)(currpos+1), blksize-1);
    for(uint8_t i = 1; i < blksize; i++) {
        CfgData[i-1] = EEPROM.read(currpos + i);
    }
    return blksize-1;
}

uint8_t  
EEconfig::getByte(uint16_t pos)
{
    return EEPROM.read(pos); 
}

void  
EEconfig::erase(void)
{
    if(!isInited()) return;
    for(uint16_t i = base; i < base+size; i++) {
        EEPROM.write(i, 0xFF);
    }
    currpos = base;
}

// END EEconfig.cpp
