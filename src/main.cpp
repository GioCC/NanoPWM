// =======================================================================
// @file        main.cpp
//
// @project     NanoPWM
// @details     Pot controlled PWM brightness regulator with serial I/F     
//
// @author      GiorgioCC (g.crocic@gmail.com) - 2023-08-20
// @modifiedby  GiorgioCC - 2023-08-27 19:46
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

void setChannel(uint8_t nCh, uint8_t Apin, uint8_t Ppin)
{
    if(nCh >= MAX_CH) return;
    chan[nCh].ADCpin = Apin;
    chan[nCh].PWMpin = Ppin;
    chan[nCh].acc.setLength(chan[nCh].accSize);
    pinMode(Apin, INPUT); 
}

uint8_t fetchInVal(uint8_t nCh)
{
    if(nCh >= MAX_CH) return 0;
    uint16_t aval = analogRead(chan[nCh].ADCpin);
    uint8_t  res  = 0;
    
    chan[nCh].acc.addVal(aval);
    if(true | chan[nCh].acc.ready()) {
        res = chan[nCh].ADCval();
        if(chan[nCh].LEDcorrect) res = PWMtables::TAB_CIE_8[res];
        if(chan[nCh].reverse) res = (255-res);
        chan[nCh].PWMval = res;
    }
    return res;
}

void setPWM(uint8_t nCh)
{   
    analogWrite(chan[nCh].PWMpin, chan[nCh].PWMval);
}

void TESTsetup() {
}

void TESTloop() {
}

void setup() {
    Serial.begin(19200);
    delay(5000);
    setChannel(0, A0, 3);
    setChannel(1, A1, 5);
    setChannel(2, A2, 6);
    setChannel(3, A3, 9);
#ifndef PROMINI
    setChannel(4, A4, 10);
    setChannel(5, A5, 11);
#endif
    cfgStore.init(15, 128);
}

void loop() {
    // TESTloop();
    static uint8_t nc = 0;
    
    now = millis();
    if((now - lastPoll) > 2) {
        lastPoll = now;
        // Update next channel after 2 ms
        // (Channel update rate 2ms*4/6 = 8/12ms)
        if(chan[nc].internal) {
            fetchInVal(nc);
            setPWM(nc);
        }
        if(++nc >= MAX_CH) nc = 0;
    }
    processCmds(now);
}



