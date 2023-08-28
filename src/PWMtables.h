// =======================================================================
// @file        PWMtables.h
//
// @project     NanoPWM - Arduino Nano PWM generator for LED dimming
// @details     This file contains brightness profile data
//  for PWM outputs for LEDs, in order to obtain a perceived brightness 
//  matching the setting value.
//  The input value is in the range 0-255;
//  The output value is in the range 0-255
//
// @author      GiorgioCC (g.crocic@gmail.com) - 2023-08-20
// @modifiedby  GiorgioCC - 2023-08-25 17:39
//
// Copyright (c) 2023 GiorgioCC
// =======================================================================

#ifndef __PWMTABLE_H__
#define __PWMTABLE_H__

#include <stdint.h>

#ifdef ARDUINO_ARCH_AVR
    #include <avr\pgmspace.h>
    // Reminder - to access table value in progmem:
    // For 8-bit values:
    //   uint8_t v;
    //   const uint8_t* p;
    //   v = pgm_read_byte_near(p++);
    // For 16-bit values:
    //   uint16_t v;
    //   const uint16_t* p;
    //   v = pgm_read_word_near(p++);
#else
    #define PROGMEM 
#endif

namespace PWMtables
{
    // extern const uint16_t   TAB_CIE_12[256];        // CIE 12-bit
    extern const uint8_t    TAB_CIE_8[256];         // CIE 8-bit
}

#endif  //__PWMTABLE_H__


