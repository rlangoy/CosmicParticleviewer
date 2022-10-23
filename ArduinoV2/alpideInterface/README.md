# Arduino Pico Readout Firmware

The Firmware is buildt for Arduino IDE 2.0 
   -  Code not specific for any hardware, but tested on RPI Pico (RP2040)
   -  Designed for the PCB board "Raspberry Pico (RP2040) interface to Outer Barrel Module"
   -  JSON for intepreting commands the reply might not be formed as JSON due to performance
        
## Installation
### Install Arduino IDE 2.x
Download and install  [Arduino IDE 2.x](https://github.com/arduino/arduino-ide)
In Arduino IDE 2.x \ 
-  Install the compiler Raspberry Pi Pico/RP2040 by Eale F. Phillower v 2.5.x \
      Inside The Arduino IDE select: \
         File | Preferences.. | Aditional Board Managers \
        add the URL: https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.json
 
 
- Installation Library ArduinoJson
     Complete install instructions could be found [here](https://arduinojson.org/v6/doc/installation/) \
     Short Install instructions: \
       Open the Arduino Library Manager  [Ctrl] + [Shift] + I \
       Search for “ArduinoJson” \
       Select the version: 6.19.4   \
       Click install. \
 
