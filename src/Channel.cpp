// =======================================================================
// @file        Channel.cpp
//
// @project     NanoPWM
// @details     Pot controlled PWM brightness regulator with serial I/F     
//
// @author      GiorgioCC (g.crocic@gmail.com) - 2023-08-20
// @modifiedby  GiorgioCC - 2023-10-09 15:58
//
// Copyright (c) 2023 GiorgioCC
// =======================================================================

#include "Channel.h"

Channel::
Channel(void)
: ADCpin(0xFF), PWMpin(0xFF), PWMval(0x00),
internal(true), reverse(false), LEDcorrect(true), active(true)
{}

uint8_t Channel::
pack(uint8_t* dst)
{
    uint8_t flags = 0;
    uint8_t n = 0;
    if(LEDcorrect) flags |= 0x01; 
    if(reverse)    flags |= 0x02; 
    if(internal)   flags |= 0x04;
    if(active)     flags |= 0x08;
    // *dst++ = ADCpin;    // Unused here: fixed value
    // *dst++ = PWMpin;    // Unused here: fixed value
    // *dst++ = PWMval;    // Unused here: not saved
    *dst++ = flags; n++;
    return n; // MUST be equal to cfgSize!
}

uint8_t Channel::
unpack(uint8_t* src)
{
    uint8_t flags;
    uint8_t n = 0;
    // ADCpin = *src++;  // Unused here: fixed value
    // PWMpin = *src++;  // Unused here: fixed value
    // PWMval = *src++;  // Unused here: not saved
    flags  = *src++; n++;
    LEDcorrect = ((flags & 0x01) != 0); 
    reverse    = ((flags & 0x02) != 0); 
    internal   = ((flags & 0x04) != 0); 
    active     = ((flags & 0x08) != 0); 
    return n; // MUST be equal to cfgSize!
}

void Channel::
set(uint8_t Apin, uint8_t Ppin)
{
    ADCpin = Apin;
    PWMpin = Ppin;
    // acc.setLength(accSize);
    filter.SetWeight(ExpWeight);
    pinMode(Apin, INPUT); 
}

uint8_t Channel::
fetchInVal(void)
{
    uint16_t aval = analogRead(ADCpin);
    uint16_t res  = 0;
    
    // Always read ADC anyway, even if value is forced from Serial
    // acc.addVal(aval);
    filter.Filter(aval);
    res = ADCval();
    
    res = (aval+2) >> 2; //DEBUG
    if(res > 255) res = 255;
    return (uint8_t)res;
}

void  Channel::
setVal(uint8_t val)
{   
    PWMval = val;
    if(!active) val = 0;
    
    // BEWARE: "Reverse" should only be used to setup a low-side LED drive, NOT to
    // make up for an inverted connection of the control potentiometer.
    // If "Reverse" is applied to an LED driven high-side (or the other way around), 
    // applying "LEDcorrect" does not only fail to improve the brightness progression,
    // but it actually makes it worse!

    if(LEDcorrect) val = pgm_read_byte(PWMtables::TAB_CIE_8 + val);
    if(reverse) val = (255-val);
    if(val == 0) {
        digitalWrite(PWMpin, 0);
    } else
    if(val == 255) {
        digitalWrite(PWMpin, 1);
    } else {
        analogWrite(PWMpin, val);
    }
}

// end channel.cpp