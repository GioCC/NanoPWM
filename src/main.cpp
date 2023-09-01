// =======================================================================
// @file        main.cpp
//
// @project     NanoPWM
// @details     Pot controlled PWM brightness regulator with serial I/F     
//
// @author      GiorgioCC (g.crocic@gmail.com) - 2023-08-20
// @modifiedby  GiorgioCC - 2023-09-01 17:29
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
unsigned long last_1s = 0;

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
setPWM(uint8_t val)
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

// ===============================
//  Main functions
// ===============================

void saveParams(void)
{
    uint8_t buf[CfgBlockSize];
    uint8_t* dst = buf;

    for(uint8_t i = 0; i < MAX_CH; i++) {
        dst += chan[i].pack(dst);
    }
    
    cfgStore.write(buf);
}

void fetchParams(void)
{
    uint8_t buf[CfgBlockSize];
    uint8_t* src = buf;
    
    if(cfgStore.isValid()) {
        cfgStore.read(buf);

        for(uint8_t i = 0; i < MAX_CH; i++) {
            chan[i].unpack(src);
            src += Channel::cfgSize;
        }
    } else {
        resetParams();
    }
}

void resetParams(void)
{
    for(uint8_t i = 0; i < MAX_CH; i++) {
        chan[i].active      = true;
        chan[i].internal    = true;
        chan[i].reverse     = false;
        chan[i].LEDcorrect  = true;
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
    //delay(1000);
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
    if((now - last_1s) > 2000) {
        last_1s = now;
        //Serial.println("Tick.");
        //printAllValues();
    }
    processCmds(now);
}
