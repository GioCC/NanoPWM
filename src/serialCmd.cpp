// =======================================================================
// @file        serialCmd.cpp
//
// @project     NanoPWM
//
// @author      GiorgioCC (g.crocic@gmail.com) - 2023-08-27
// @modifiedby  GiorgioCC - 2023-09-14 15:41
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
        Serial.println(F("> Values:"));
        Serial.println(F("Vnbbb - Set brightness of channel #n to value bbb"));
        Serial.println(F("O/o   - All channels On/off"));
        Serial.println(F("> Channel setup:"));
        Serial.println(F("An/an - Single channel On/off"));
        Serial.println(F("In/in - Set value source of channel #n to internal/external"));
        Serial.println(F("Rn/rn - Reverse PWM On/Off"));
        Serial.println(F("Cn/cn - Correct PWM for CIE LED brightness On/Off"));
        Serial.println(F("nAIRC - Set flags for ch. #n: A/a, I/i, R/r, C/c"));
        Serial.println(F("s/S   - Save current params"));
        Serial.println(F("x/X   - Discard changes, revert to last saved configuration"));
        Serial.println(F("F     - Reset all params to factory defaults"));
        Serial.println(F("p/P   - Report current channel setpoint / parameters"));
        Serial.println(F("h/H   - Print command help"));
        Serial.println(F("> DEBUG:"));
        Serial.println(F("ynnn  - Print <nnn> bytes from EEPROM (start from current pos)"));
        Serial.println(F("Ynnn  - Print <nnn> bytes from EEPROM (start from 0)"));
        Serial.println(F("Z     - Reset (zero out) EEPROM"));
}

void printAllValues(void)
{
    for(uint8_t i = 0; i < MAX_CH; i++) {
        Serial.print(chan[i].PWMval);
        Serial.print(" ");
    }           
    Serial.println();
}

void printEEpromContent(uint16_t start, uint8_t nvals) 
{
    Serial.print("Base "); 
    Serial.print(cfgStore.getBase()); 
    Serial.print(" / Pos "); 
    Serial.println(cfgStore.getCurrPos()); 

    uint8_t  n = 0;
    char     hbuf[12];

    while(1) {
        sprintf(hbuf, "%02X ", cfgStore.getByte(start + n++));
        Serial.print(hbuf);
        if(--nvals == 0) {
            Serial.println();
            break;
        }
        if((n & 0x0F) == 0) {
            Serial.println();
        }
    }
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
                if(!chan[chn].internal) {
                    uint8_t v = (uint8_t)(msgBuf[4]-'0');
                    v += times10((uint8_t)(msgBuf[3]-'0'));
                    v += times100((uint8_t)(msgBuf[2]-'0'));
                    chan[chn].setPWM(v);
                }
                cmdDone = true;
            }
        }
        break;
        
        case 'o':
        case 'O':
        {
            // "O"/"o"- All channels On/off
            for(uint8_t i = 0; i < MAX_CH; i++) {
                chan[chn].active = (cmd == 'O');
            }           
            cmdDone = true;
        }
        break;

        case 'a':
        case 'A':
        {
            // "An"/"an"- Single channel On/off
            if(isValidCommand(2)) {
                chan[chn].active = (cmd == 'A');
                cmdDone = true;
            }           
        }
        break;

        case 'i':
        case 'I':
        {
            // "In/in"- Set value source of channel #n to internal
            // (pot reading) or external (serial)
            if(isValidCommand(2)) {
                chan[chn].internal = (cmd == 'I');
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

        case '0':
        case '1':
        case '2':
        case '3':
#ifndef PROMINI
        case '4':
        case '5':
#endif  
        {
            if(isValidCommand(5)) {
                chn = cmd -'0';
                chan[chn].active        = (msgBuf[1] == 'A');
                chan[chn].internal      = (msgBuf[2] == 'I');
                chan[chn].reverse       = (msgBuf[3] == 'R');
                chan[chn].LEDcorrect    = (msgBuf[4] == 'C');
                cmdDone = true;
            }

        }


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
            // "x/X"- Discard changes, revert to last saved configuration
            fetchParams();
            cmdDone = true;
        }
        break;

        case 'F':
        {
            // "F"- Reset to factory defaults
            resetParams();
            cmdDone = true;
        }
        break;

        case 'p':
        {
            // "p" - Report current channel values
            // Reports setpoint value; does not report 0 if inactive
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
                Serial.print(chan[i].active     ? ": A " : ": - ");
                Serial.print(chan[i].internal     ? "I " :   "- ");
                Serial.print(chan[i].reverse      ? "R " :   "- ");
                Serial.println(chan[i].LEDcorrect ? "C"  :   "-");
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

        // =========================================
        //  TEST/DEBUG COMMANDS
        // =========================================

        case 'y':
        case 'Y':
        {
            // "ynnn" - Print <nnn> bytes from EEPROM (start from current pos)
            // "Ynnn" - Print <nnn> bytes from EEPROM (start from 0)
            if(isValidCommand(4)) {
                uint8_t v = (uint8_t)(msgBuf[3]-'0');
                v += times10((uint8_t)(msgBuf[2]-'0'));
                v += times100((uint8_t)(msgBuf[1]-'0'));
                uint16_t pos = (cmd == 'y' ? cfgStore.getCurrPos() : cfgStore.getBase());
                printEEpromContent(pos, v);
                cmdDone = true;
            }
        }
        break;

        case 'Z':
        {
            // "Z" - Reset (zero out) EEPROM
            cfgStore.erase();
            printEEpromContent(0, 64);
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
