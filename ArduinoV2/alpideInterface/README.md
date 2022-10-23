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
       Click install. 

## Commands:
JSON input
   - single   reg read 
     - {"readRegister": {     "chipID": 112,     "addr": 1545   } }
     - Returns values as {"reg" : xxx ,  "chipID":xxx , "addr":xxx, "value"}
   - multiple reg read  
     - {"readRegister": [{"chipID": 112,     "addr": 1545},{"chipID": 113,     "addr": 1545}]    }
     - Returns values as {"reg" : xxx ,  "chipID":xxx , "addr":xxx, "value"}
 
   - single   reg write 
     - {"writeRegister": {     "chipID": 112,   "addr": 1545  ,"data" : 100  } }
     - Returns values as "ok"
   - multiple reg write  
     - {"writeRegister": [{"chipID": 112, "addr": 1545 ,"data" : 100},{"chipID": 113, "addr": 1545 ,"data" : 101}]    }
     - Returns values as "ok"

   -  sendRest  - Sends reset signal
      - {"sendRest": "True" }
      - Returns values as "ok"

   -  sendTrigger  - Sends trigger signal (6000clk's)
      - {"sendTrigger": "True" }
      - Returns values as "ok"

   - getPixelFrameData  - Return events/pixel-hit data 
      - format : streamRaw: Dumps raw data 
      - {"getPixelFrameData": ["chipID" , 112 ] }
      - Return stream of 24 bit binary data \
&nbsp;  Format: \
&nbsp;&nbsp;&nbsp;&nbsp; 0xA093FF - starting with header    \
&nbsp;&nbsp;&nbsp;&nbsp; 0xB0FFFF - end with trailer         \
\
&nbsp;&nbsp;&nbsp;&nbsp; 0xFFFFFF - If not ready           



 
