// =======================================================================
// @file        serialCmd.cpp
//
// @project     NanoPWM
//
// @author      GiorgioCC (g.crocic@gmail.com) - 2023-08-27
// @modifiedby  GiorgioCC - 2023-08-27 23:22
//
// Copyright (c) 2023 GiorgioCC
// =======================================================================

#include "serialCmd.h"

const uint8_t  MsgBufLen  = 10;
const uint16_t MsgTimeout = 10000;
unsigned long lastCharTS;
char    msgBuf[MsgBufLen];
uint8_t ci = 0;

bool isChannelOK(char c) {
            return (c >= '0' 
#ifdef PROMINI
            && c <= '3');
#else
            && c <= '5');
#endif
}

bool isCommandReady(uint8_t nChars, bool checkCh = true) {
    if(ci != nChars) return false;
    bool res;
    if(checkCh) {
        if(!(res = isChannelOK(msgBuf[1]))) {
            ci = 0;
        }
    }
    return res;
}

void tryCommand(void)
{
    char c0 = msgBuf[0];
    char c1 = msgBuf[1];
    switch(c0) {
        // case 'v':
        case 'V':
        {
            // "Vnbbb" - set brightness of channel #n to value
            if(isCommandReady(5)) {
                // ....
            }
        }
        break;
        
        // case 'l':
        case 'L':
        {
            // "Ln"- set brightness of channel #n to pot reading
            if(isCommandReady(2)) {
                // ....
            }
        }

        case 'r':
        case 'R':
        {
            // "Rn"/"rn"- Reverse PWM On/Off
            if(isCommandReady(2)) {
                // ....
            }
        }

        case 'c':
        case 'C':
        {
            // "Cn"/"cn"- Correct PWM for CIE LED brightness On/Off
            if(isCommandReady(2)) {
                // ....
            }
        }
    }
}


void processCmds(unsigned long now)
{
    char c; 
    if(!Serial.available()) {
        if((now - lastCharTS) > MsgTimeout) ci = 0;
        return;
    }
    lastCharTS = now;
    while(Serial.available()) {
        c = (char)Serial.read();
        if(c == '#') {
            ci = 0;
        } else 
        if(ci < MsgBufLen) {
            msgBuf[ci++] = c;
            tryCommand();
        }
    }
}


