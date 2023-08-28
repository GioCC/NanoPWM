// =======================================================================
// @file        serialCmd.cpp
//
// @project     NanoPWM
//
// @author      GiorgioCC (g.crocic@gmail.com) - 2023-08-27
// @modifiedby  GiorgioCC - 2023-08-28 16:40
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

inline void resetCmd(void)
{
    ci = 0;
}


bool isCommandReady(uint8_t nChars, bool checkCh = true) {
    if(ci != nChars) return false;
    bool res = true;
    if(checkCh) {
        if(!(res = isChannelOK(msgBuf[1]))) {
            // Wrong channel aborts command
            resetCmd();
        }
    }
    return res;
}

inline uint8_t times10(uint8_t v)
{
    return ((v<<3) + (v<<1));
}

inline uint8_t times100(uint8_t v)
{
    uint8_t w = ((v<<3) + (v<<1));
    return ((w<<3) + (w<<1));
}

void tryCommand(void)
{
    char    cmd = msgBuf[0];
    uint8_t chn = msgBuf[1]-'0';
    bool    cmdDone = false;

    switch(cmd) {
        // case 'v':
        case 'V':
        {
            // "Vnbbb" - set brightness of channel #n to value
            if(isCommandReady(5)) {
                chan[chn].internal = false;
                uint8_t v = (uint8_t)(msgBuf[4]-'0');
                v += times10((uint8_t)(msgBuf[3]-'0'));
                v += times100((uint8_t)(msgBuf[2]-'0'));
                chan[chn].setPWM(v);
                cmdDone = true;
            }
        }
        break;
        
        // case 'l':
        case 'L':
        {
            // "Ln"- set brightness of channel #n to pot reading
            // (effective at next loop)
            if(isCommandReady(2)) {
                chan[chn].internal = true;
                cmdDone = true;
            }
        }
        break;

        case 'r':
        case 'R':
        {
            // "Rn"/"rn"- Reverse PWM On/Off
            if(isCommandReady(2)) {
                chan[chn].reverse = (cmd == 'R');
                cmdDone = true;
            }
        }
        break;

        case 'c':
        case 'C':
        {
            // "Cn"/"cn"- Correct PWM for CIE LED brightness On/Off
            if(isCommandReady(2)) {
                chan[chn].LEDcorrect = (cmd == 'C');
                cmdDone = true;
            }
        }
        break;

        case 's':
        case 'S':
        {
            // "s"/"S"- Save current params
            saveParams();
            cmdDone = true;
        }
        break;

        case 'x':
        case 'X':
        {
            // "x"/"X"- Discard changes, revert to last saved configuration
            fetchParams();
            cmdDone = true;
        }
        break;

        case 'o':
        case 'O':
        {
            // "on"/"On"- Single channel <o>ff/<O>n
            if(isCommandReady(2)) {
                chan[chn].inhibit = (cmd == 'o');
                cmdDone = true;
            }           
        }
        break;

        case 'a':
        case 'A':
        {
            // "a"/"A"- All channels channel off/on
            for(uint8_t i = 0; i < MAX_CH; i++) {
                chan[chn].inhibit = (cmd == 'a');
            }           
            cmdDone = true;
        }
        break;

        default: 
        break;

    }
    if(cmdDone) {
        resetCmd();
        Serial.println("OK");
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


