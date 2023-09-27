// =======================================================================
// @file        main.cpp
//
// @project     NanoPWM
// @details     Pot controlled PWM brightness regulator with serial I/F
//
// @author      GiorgioCC (g.crocic@gmail.com) - 2023-08-20
// @modifiedby  GiorgioCC - 2023-09-27 12:08
//
// Copyright (c) 2023 GiorgioCC
// =======================================================================

// TODO: (@2023-09-14)
// - Test / add or remove input filter

#include "main.h"
#include "serialCmd.h"

#define UART_BAUD       19200

#ifdef  USE_I2C
#define I2C_ADDRESS     0x01
#endif


#define DELAY_MS 1024
int               dly  = DELAY_MS;
int               dlyf = DELAY_MS / 4;

unsigned long     now      = 0;
unsigned long     lastPoll = 0;
unsigned long     last_1s  = 0;

Channel           chan[MAX_CH];
EEconfig          cfgStore;

constexpr uint8_t CfgBlockSize = MAX_CH * Channel::cfgSize;

// ===============================
//  Main functions
// ===============================

void saveParams(void)
{
    uint8_t  buf[CfgBlockSize];
    uint8_t *dst = buf;

    for (uint8_t i = 0; i < MAX_CH; i++) {
        dst += chan[i].pack(dst);
    }

    cfgStore.write(buf);
}

void fetchParams(void)
{
    uint8_t  buf[CfgBlockSize];
    uint8_t *src = buf;

    if (cfgStore.isValid()) {
        cfgStore.read(buf);

        for (uint8_t i = 0; i < MAX_CH; i++) {
            chan[i].unpack(src);
            src += Channel::cfgSize;
        }
    } else {
        resetParams();
    }
}

void resetParams(void)
{
    for (uint8_t i = 0; i < MAX_CH; i++) {
        chan[i].active     = true;
        chan[i].internal   = true;
        chan[i].reverse    = false;
        chan[i].LEDcorrect = true;
    }
    saveParams();
}

bool checkParamReset(void)
{
    // HW factory reset for jumper at boot on D12 (Nano) / D10 (ProMicro)
    bool    pinVal;
    uint8_t bootPin =
#ifdef PROMINI // also ProMicro
        10;
#else
        12;
#endif
    pinMode(bootPin, INPUT_PULLUP);
    delay(10);
    pinVal = !digitalRead(bootPin);
    pinMode(bootPin, INPUT_PULLUP);
    if (pinVal) resetParams();
    return pinVal;
}

#ifdef  USE_I2C
void onI2Crequest(void) 
{
    while(Wire.available()) {
        char c = Wire.read(); // receive byte as a character
        //int x = Wire.read();    // receive byte as an integer

        //! TODO ........
    }
}
#endif


// void TESTsetup() {
// }

// void TESTloop() {
// }

void setup()
{
    // TESTsetup();
    Serial.begin(UART_BAUD);

#ifdef  USE_I2C
    Wire.begin(I2C_ADDRESS);
    Wire.onRequest(onI2Crequest);
#endif

    // delay(1000);
#ifdef  HW_V1
    // Hardware v1.x
    chan[0].set(A0, 3);
    chan[1].set(A1, 5);
    chan[2].set(A2, 6);
    chan[3].set(A3, 9);
    #ifndef PROMINI
    chan[4].set(A4, 10);
    chan[5].set(A5, 11);
    #endif
#else
    // Hardware v2.x
    #ifndef PROMINI
    chan[0].set(A0, 3);
    chan[1].set(A1, 5);
    chan[2].set(A2, 6);
    chan[3].set(A3, 9);
    chan[4].set(A6, 10);
    chan[5].set(A7, 11);
    #else
    chan[0].set(A0, 5);
    chan[1].set(A1, 6);
    chan[2].set(A2, 9);
    chan[3].set(A3, 10);
    #endif
#endif  //HW_V1


    cfgStore.init(CfgBlockSize, 128);
    if (!checkParamReset()) fetchParams();
}

void loop()
{
    // TESTloop();
    static uint8_t nc = 0;
    static uint8_t v  = 0;

    now = millis();
    if ((now - lastPoll) > 2) {
        lastPoll = now;
        // Update next channel after 2 ms
        // (Channel update rate 2ms*4/6 = 8/12ms)
        v  = chan[nc].fetchInVal();
        if (chan[nc].internal) {
            chan[nc].setPWM(v);
        }
        if (++nc >= MAX_CH) nc = 0;
    }
    if ((now - last_1s) > 2000) {
        last_1s = now;
        // Serial.println("Tick.");
        // printAllValues();
    }
    processCmds(now);
}
