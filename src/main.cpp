// =======================================================================
// @file        main.cpp
//
// @project     NanoPWM
// @details     Pot controlled PWM brightness regulator with serial I/F     
//
// @author      GiorgioCC (g.crocic@gmail.com) - 2023-08-20
// @modifiedby  GiorgioCC - 2023-08-31 17:53
//
// Copyright (c) 2023 GiorgioCC
// =======================================================================

#include "main.h"
#include "serialCmd.h"

#define DELAY_MS 1024
int dly  = DELAY_MS;
int dlyf = DELAY_MS/4;

unsigned long now = 0;
unsigned long lastPoll = 0;

Channel     chan[MAX_CH];
EEconfig    cfgStore;

constexpr uint8_t CfgBlockSize = MAX_CH * Channel::cfgSize;
constexpr uint8_t ExpWeight = 30;


// =================================
//  Channel class member definition
// =================================

Channel::Channel(void)
: ADCpin(0xFF), PWMpin(0xFF), PWMval(0x00),
internal(true), reverse(false), LEDcorrect(true)
{}

uint8_t Channel::
pack(uint8_t* dst)
{
    uint8_t flags;
    if(LEDcorrect) flags |= 0x01; 
    if(reverse)    flags |= 0x02; 
    if(internal)   flags |= 0x04;
    if(inhibit)    flags |= 0x08;
    // *dst++ = ADCpin;    // Unused here: fixed value
    // *dst++ = PWMpin;    // Unused here: fixed value
    // *dst++ = PWMval;    // Unused here: not saved
    *dst++ = flags;
    return cfgSize;
}

void Channel::
unpack(uint8_t* src)
{
    uint8_t flags;
    // ADCpin = *src++;  // Unused here: fixed value
    // PWMpin = *src++;  // Unused here: fixed value
    // PWMval = *src++;  // Unused here: not saved
    flags  = *src++;
    LEDcorrect = ((flags & 0x01) != 0); 
    reverse    = ((flags & 0x02) != 0); 
    internal   = ((flags & 0x04) != 0); 
    inhibit    = ((flags & 0x08) != 0); 
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
    uint8_t  res  = 0;
    
    // Always read ADC anyway, even if value is forced from Serial
    // acc.addVal(aval);
    filter.Filter(aval);
    res = ADCval();
    res = (aval+2) >> 2; //DEBUG
    return res;
}

void  Channel::
setPWM(uint8_t val)
{   
    PWMval = val;
    val = (inhibit ? 0 : val);
    // if(LEDcorrect) val = PWMtables::TAB_CIE_8[val];
    // if(reverse) val = (255-val);
    analogWrite(PWMpin, val);
}

// ===============================
//  Main functions
// ===============================

void saveParams(void)
{
    uint8_t buf[CfgBlockSize];
    uint8_t* dst = buf;

    for(uint8_t i = 0; i < MAX_CH; i++) {
        chan[i].pack(dst);
        dst += Channel::cfgSize;
    }
    
    cfgStore.write(buf);
}

void fetchParams(void)
{
    uint8_t buf[CfgBlockSize];
    uint8_t* src = buf;
    
    cfgStore.read(buf);

    for(uint8_t i = 0; i < MAX_CH; i++) {
        chan[i].unpack(src);
        src += Channel::cfgSize;
    }
}

void resetParams(void)
{
    for(uint8_t i = 0; i < MAX_CH; i++) {
        chan[i].inhibit = false;
        chan[i].internal = true;
        chan[i].LEDcorrect = true;
        chan[i].reverse = false;
   }
   saveParams();
}

// void TESTsetup() {
// }

// void TESTloop() {
// }

void setup() {
    // TESTsetup();
    Serial.begin(19200);
    delay(5000);
    chan[0].set(A0, 3);
    chan[1].set(A1, 5);
    chan[2].set(A2, 6);
    chan[3].set(A3, 9);
#ifndef PROMINI
    chan[4].set(A4, 10);
    chan[5].set(A5, 11);
#endif
    cfgStore.init(CfgBlockSize, 128);
    fetchParams();
}

void loop() {
    // TESTloop();
    static uint8_t nc = 0;
    static uint8_t v  = 0;
    
    now = millis();
    if((now - lastPoll) > 2) {
        lastPoll = now;
        // Update next channel after 2 ms
        // (Channel update rate 2ms*4/6 = 8/12ms)
        v = chan[nc].fetchInVal();
        if(chan[nc].internal) {
            chan[nc].setPWM(v);
        }
        if(++nc >= MAX_CH) nc = 0;
    }
    processCmds(now);
}



