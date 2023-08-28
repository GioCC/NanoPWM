// =======================================================================
// @file        main.h
//
// @project     NanoPWM
// @details     Pot controlled PWM brightness regulator with serial I/F     
//
// @author      GiorgioCC (g.crocic@gmail.com) - 2023-08-20
// @modifiedby  GiorgioCC - 2023-08-27 19:46
//
// Copyright (c) 2023 GiorgioCC
// =======================================================================

#ifndef __MAIN__H__
#define __MAIN__H__

#include <Arduino.h>
#include <stdint.h>
#include <avr\pgmspace.h>
#include "average_acc.h"
#include "PWMtables.h"
#include "EEconfig.h"

#define PIN_LED 1
#define PIN_PWM 1
#define PIN_ANA 2

using Channel = struct {
    uint8_t     ADCpin;
    uint8_t     PWMpin;
    AverageAcc  acc;
    const uint8_t accSize = 3;
    bool        internal = true;
    bool        reverse  = false;
    bool        LEDcorrect = true;
    uint8_t     PWMval;
    uint8_t     ADCval(void) { return ((acc.average() + 2) >> 2); };
};

#ifndef PROMINI
constexpr uint8_t MAX_CH = 4;
#else 
constexpr uint8_t MAX_CH = 6;
#endif

extern Channel chan[MAX_CH];

extern EEconfig cfgStore;

void    setChannel(uint8_t nCh, uint8_t Apin, uint8_t Ppin);
uint8_t fetchInVal(uint8_t nCh);
void    setPWM(uint8_t nCh);
// void    processCmd(void);

#endif  //!__MAIN__H__


