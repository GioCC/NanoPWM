// =======================================================================
// @file        main.h
//
// @project     NanoPWM
// @details     Pot controlled PWM brightness regulator with serial I/F     
//
// @author      GiorgioCC (g.crocic@gmail.com) - 2023-08-20
// @modifiedby  GiorgioCC - 2023-08-28 16:40
//
// Copyright (c) 2023 GiorgioCC
// =======================================================================

#ifndef __MAIN__H__
#define __MAIN__H__

#include <Arduino.h>
#include <stdint.h>
#include <avr\pgmspace.h>
#include "PWMtables.h"
#include <average_acc.h>
#include <EEconfig.h>
#include <ExpFilter.h>

#define PIN_LED 1
#define PIN_PWM 1
#define PIN_ANA 2

class Channel {
public:

    uint8_t     ADCpin;
    uint8_t     PWMpin;
    uint8_t     PWMval;
    // AverageAcc  acc;
    ExpFilter<int> filter;
    bool        internal;
    bool        reverse;
    bool        LEDcorrect;
    bool        inhibit;

    static constexpr uint8_t accSize = 3;
    static constexpr uint8_t cfgSize =
    /* sizeof(ADCpin)+sizeof(PWMpin)+sizeof(PWMval)+ */ 1;
    
    Channel(void);
    uint8_t     ADCval(void) 
        // { return ((acc.average() + 2) >> 2); }
        { return ((filter.Current() + 2) >> 2); }
    void        set(uint8_t Apin, uint8_t Ppin);
    uint8_t     fetchInVal(void);
    void        setPWM(uint8_t val);
    uint8_t     pack(uint8_t* dst);
    void        unpack(uint8_t* src);
};

#ifndef PROMINI
constexpr uint8_t MAX_CH = 4;
#else 
constexpr uint8_t MAX_CH = 6;
#endif

extern Channel chan[MAX_CH];

extern EEconfig cfgStore;

// uint8_t fetchInVal(uint8_t nCh);
// void    setPWM(uint8_t nCh, uint8_t val);

void    saveParams(void);
void    fetchParams(void);

// void    processCmd(void);

#endif  //!__MAIN__H__


