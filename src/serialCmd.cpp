// =======================================================================
// @file        serialCmd.cpp
//
// @project     NanoPWM
//
// @author      GiorgioCC (g.crocic@gmail.com) - 2023-08-27
// @modifiedby  GiorgioCC - 2023-08-31 17:51
//
// Copyright (c) 2023 GiorgioCC
// =======================================================================

#include "serialCmd.h"

const uint8_t  MsgBufLen  = 10;
const uint16_t MsgTimeout = 10000;
unsigned long lastCharTS;
char    msgBuf[MsgBufLen];
uint8_t ci = 0;
bool cmdDone;
bool cmdErr;

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


bool isValidCommand(uint8_t nChars, bool checkCh = true) {
    if(ci != nChars) return false;
    bool res = true;
    if(checkCh) {
        if(!(res = isChannelOK(msgBuf[1]))) {
            // Wrong channel aborts command
            cmdErr = true;
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

void printHelp(void)
{
        Serial.println(F("Vnbbb - Set brightness of channel #n to value bbb"));
        Serial.println(F("Ln    - set brightness of channel #n to pot reading"));
        Serial.println(F("Rn/rn - Reverse PWM On/Off"));
        Serial.println(F("Cn/cn - Correct PWM for CIE LED brightness On/Off"));
        Serial.println(F("s/S   - Save current params"));
        Serial.println(F("x     - Discard changes, revert to last saved configuration"));
        Serial.println(F("X     - Reset all params to factory defaults"));
        Serial.println(F("on/On - Single channel <o>ff/<O>n"));
        Serial.println(F("a/A   - All channels off/on"));
        Serial.println(F("p/P   - Report current channel setpoint / parameters"));
        Serial.println(F("h/H   - Print command help"));
}

void tryCommand(void)
{
    char    cmd = msgBuf[0];
    uint8_t chn = msgBuf[1]-'0';
    cmdDone = false;
    cmdErr  = false;

    switch(cmd) {
        // case 'v':
        case 'V':
        {
            // "Vnbbb" - set brightness of channel #n to value
            if(isValidCommand(5)) {
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
            if(isValidCommand(2)) {
                chan[chn].internal = true;
                cmdDone = true;
            }
        }
        break;

        case 'r':
        case 'R':
        {
            // "Rn"/"rn"- Reverse PWM On/Off
            if(isValidCommand(2)) {
                chan[chn].reverse = (cmd == 'R');
                cmdDone = true;
            }
        }
        break;

        case 'c':
        case 'C':
        {
            // "Cn"/"cn"- Correct PWM for CIE LED brightness On/Off
            if(isValidCommand(2)) {
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
        {
            // "x"- Discard changes, revert to last saved configuration
            fetchParams();
            cmdDone = true;
        }
        break;

        case 'X':
        {
            // "X"- Reset to factory defaults
            resetParams();
            cmdDone = true;
        }
        break;

        case 'o':
        case 'O':
        {
            // "on"/"On"- Single channel <o>ff/<O>n
            if(isValidCommand(2)) {
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

        case 'p':
        {
            // "p" - Report current channel values
            // Reports setpoint value; does not report 0 if inhibited
            for(uint8_t i = 0; i < MAX_CH; i++) {
                Serial.print("Ch");
                Serial.print(i);
                Serial.print(" = ");
                Serial.println(chan[i].PWMval);
            }           
            cmdDone = true;
        }
        break;

        case 'P':
        {
            // "P" - Report current channel parameters
            Serial.println(F("Active/Internal/Reverse/Corrected"));
            for(uint8_t i = 0; i < MAX_CH; i++) {
                Serial.print(i);
                Serial.print(chan[i].inhibit ? ": - " : ": A ");
                Serial.print(chan[i].internal ? "I " : "- ");
                Serial.print(chan[i].reverse ? "R " : "- ");
                Serial.println(chan[i].LEDcorrect ? "C" : "-");
            }           
            cmdDone = true;
        }
        break;

        case 'h':
        case 'H':
        {
            // "h"/"H"- Print command help
            printHelp();
            cmd = ' ';  // Don't print "h" twice
            cmdDone = true;
        }
        break;

        default: 
            if(cmd != '\n' && cmd != '\r') {
                Serial.print(cmd);
                Serial.println(" ?");
            } else {
                resetCmd();
            }
        break;

    }
    if(cmdDone || cmdErr) {
        resetCmd();
        Serial.print(cmd);
        Serial.println(cmdErr ? " ERR" : " OK");
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


