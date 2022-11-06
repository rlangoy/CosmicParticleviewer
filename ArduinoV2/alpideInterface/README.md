# Arduino Pico Readout Firmware

The Files ALPIDE.h & ALPIDE.cpp is removed due to properitary transfer protocol...

The Firmware is buildt for Arduino IDE 2.0 
   -  Code not specific for any hardware, but tested on RPI Pico (RP2040)
   -  Designed for the PCB board "Raspberry Pico (RP2040) interface to Outer Barrel Module"
   -  JSON for intepreting commands the reply might not be formed as JSON due to performance
        


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



 
