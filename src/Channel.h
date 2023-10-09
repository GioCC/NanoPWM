// =======================================================================
// @file        Channel.h
//
// @project     NanoPWM
// @details     Pot controlled PWM brightness regulator with serial I/F
//
// @author      GiorgioCC (g.crocic@gmail.com) - 2023-08-20
// @modifiedby  GiorgioCC - 2023-10-09 15:58
//
// Copyright (c) 2023 GiorgioCC
// =======================================================================

#ifndef __CHANNEL__H__
#define __CHANNEL__H__

#include <stdint.h>
#include <Arduino.h>
#include <avr\pgmspace.h>
#include "PWMtables.h"
// #include <average_acc.h>
#include <ExpFilter.h>

class Channel
{
    static constexpr uint8_t ExpWeight = 30;

public:
    uint8_t          ADCpin;
    uint8_t          PWMpin;
    uint8_t          PWMval;
    // AverageAcc  acc;
    ExpFilter<int>   filter;
    bool             internal;
    bool             reverse;
    bool             LEDcorrect;
    bool             active;

    static constexpr uint8_t accSize = 3;
    static constexpr uint8_t cfgSize =
        /* sizeof(ADCpin)+sizeof(PWMpin)+sizeof(PWMval)+ */ 1;

    Channel(void);
    uint8_t ADCval(void)
    // { return ((acc.average() + 2) >> 2); }
    { return ((filter.Current() + 2) >> 2); }

    void    set(uint8_t Apin, uint8_t Ppin);
    uint8_t fetchInVal(void);
    void    setVal(uint8_t val);
    uint8_t getVal(void)            { return PWMval; }
    uint8_t pack(uint8_t *dst);
    uint8_t unpack(uint8_t *src);
};

#endif //!__CHANNEL__H__
