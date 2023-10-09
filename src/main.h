// =======================================================================
// @file        main.h
//
// @project     NanoPWM
// @details     Pot controlled PWM brightness regulator with serial I/F
//
// @author      GiorgioCC (g.crocic@gmail.com) - 2023-08-20
// @modifiedby  GiorgioCC - 2023-10-09 17:02
//
// Copyright (c) 2023 GiorgioCC
// =======================================================================

#ifndef __MAIN__H__
#define __MAIN__H__

#include <Arduino.h>
#include <stdint.h>
// #include <avr\pgmspace.h>
// #include "PWMtables.h"
// #include <average_acc.h>
// #include <ExpFilter.h>
#include <EEconfig.h>
#include "channel.h"

// #define PIN_LED 1
// #define PIN_PWM 1
// #define PIN_ANA 2

#ifdef PROMINI
constexpr uint8_t MAX_CH = 4;
#else
constexpr uint8_t MAX_CH = 6;
#endif

extern Channel  chan[MAX_CH];
extern EEconfig cfgStore;

// uint8_t fetchInVal(uint8_t nCh);
// void    setVal(uint8_t nCh, uint8_t val);
// void    processCmd(void);

void            saveParams(void);
void            fetchParams(void);
void            resetParams(void);

#endif //!__MAIN__H__
