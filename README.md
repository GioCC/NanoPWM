# NanoPWM - Multichannel PWM controller board

## Description

4/6 channel controller board with Arduino Nano or ProMicro.  
Setpoint inputs : Potentiometers or Serial I/F  
PWM outputs     : GPIO or power MOSFETS (low-side switching)

## Other features

- Serial comms (19200, 8N1) for parameter setup (saved in EEPROM) and setpoint input
- Option for CIE brightness correction when driving LEDS

## Serial interface Commands

Fixed format, no spaces within commands

|_Command_|_Description_|
|------|---------------------------------------------------------|
|__V__ nbbb | Set brightness of channel #n to value bbb |
|__O__ / __o__   | All channels On/off |
|__A__ n / __a__ n | Single channel On/off |
|__I__ n / __i__ n | Set value source of channel #n to internal/external |
|__R__ n / __r__ n | Reverse PWM On/Off for ch. #n |
|__C__ n / __c__ n | Correct PWM for CIE LED brightness On/Off |
|n _AIRC_ | Set flags for ch. #n: A/a, I/i, R/r, C/c |
|__s__ / __S__ | Save current params |
|__x__ / __X__ | Discard changes, revert to last saved configuration |
|__F__     | Reset all params to factory defaults |
|__p__ / __P__   | Report current channel setpoint / parameters |
|__h__ / __H__   | Print command help |
|__y__ nnn  | Print _nnn_ bytes from EEPROM (start from current pos) |
|__Y__ nnn  | Print _nnn_ bytes from EEPROM (start from 0) |
|__Z__     | Reset (zero out) EEPROM |

___Caveat___: _Reverse_ should only be used to setup a low-side LED drive, NOT to make up for an inverted connection of the control potentiometer.  
If _Reverse_ is applied to an LED driven high-side (or the other way around), applying _LEDcorrect_ does not only fail to improve the brightness progression, but it actually makes it worse.

## Future improvements

An ESP32 version could be interesting; fewer channels and worse ADC performance (not really paramount here), but configurable and accessible via integrated webserver.  
(Would probably require additional external EEPROM.)
