#include <ArduinoJson.h>
#include <ArduinoJson.hpp>

/******************************************************************
 *   Install Raspberry Pi Pico/RP2040 by Eale F. Phillower v 2.5.x
 *     File | Preferences.. | Aditional Board Managers
 *   https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.json
 *
 *  Install arduinojson
 *   Install instructions : https://arduinojson.org/v6/doc/installation/
 *     Open the Arduino Library Manager  [Ctrl] + [Shift] + I
 *      Search for “ArduinoJson”
 *      Select the version: 6.19.4 
 *      Click install.
 *************************************************************/
#include "ALPIDE.h"

ALPIDE alpide;

//#undef  UseSerialUSB         // Send messages on USART0 (GPIO port 0,1 ) (for picoprobe output...)
#define UseSerialUSB      // Unmment to send messages on the USB<->RS232


#ifdef UseSerialUSB
  HardwareSerial &SerialInUse = Serial;
#else
  HardwareSerial &SerialInUse = Serial1;
#endif

/*   
*  Test function for checking alpide.readRegister / alpide.writeRegister
*  Checks that the Alpides is responding
*/
void regReadWriteTest()
{
  int cnt=0;
  int errorCnt=0;
  SerialInUse.println("Register R/W - Test on ALIDEs width id 0x70->0x76 & 0x78->0x7E");
  SerialInUse.println("  Writes ALPIDE_REG_VTEMP Reg values from 0x0 to 0xD");
  for(int ii=0x0;ii<=8;ii+=8)
    for(int id=0x70+ii;id<0x70+7+ii;id++)
      alpide.writeRegister(id, ALPIDE_REG_VTEMP, cnt++);
  
  //Read back
  SerialInUse.println("  Reads back ALPIDE_REG_VTEMP Reg values  0x0 to 0xD");
  cnt=0;
  for(int ii=0x0;ii<=8;ii+=8)
  {
    for(int id=0x70+ii;id<0x70+7+ii;id++)
    {   
        int32_t regVal=alpide.readRegister(id, ALPIDE_REG_VTEMP);
        if(regVal!=cnt)
        { errorCnt++;
          SerialInUse.println("  --== Error ==--");
          SerialInUse.print("  Chip ID : 0x");
          SerialInUse.print(id,HEX);
          SerialInUse.print("  Register ALPIDE_REG_VTEMP  Value ");
          SerialInUse.print(": 0x");
          SerialInUse.println(regVal,HEX);

          SerialInUse.print("  Expected value: 0x");
          SerialInUse.print(cnt,HEX);
          SerialInUse.print(" But read : 0x") ;
          SerialInUse.println(regVal,HEX);        
        }
        cnt++;
    }
  }

  if(errorCnt)
  {
    SerialInUse.println("  --------------------------------------");
    SerialInUse.println("  | Warning Regsiter Read/Write Error  |");
    SerialInUse.print  ("  |  ");
    SerialInUse.print(errorCnt);
    SerialInUse.println("   errors found !!!               |");
    SerialInUse.println("  --------------------------------------");
    SerialInUse.println();
  }  
  else
  {
    SerialInUse.println("  Sucessfully verifyed register contents");
    SerialInUse.println();
  }

}

bool bDoOnce=true;
void setup() {

  SerialInUse.begin(115200);   //Set baudrate

  //SerialInUse.println("\r\n--== Init Alpide ==-- ");
  alpide.init();

  //regReadWriteTest();   // Test reg read / Write 
  
  //Disable pix-on chip..
  // alpide.maskAllPixels(0x70,true);  
  alpide.sendTrigger();
  //delay(100);   //Wait a bit after sending trigger ???
 // alpide.sendTrigger();
}



#define BUFF_SIZE 100

/*  parseReadRegister - checks is JSON root-key is "readRegister"
 *   
 *  Executes ALPIDE read of register using chipID and address
 *  JSON Example input
 *       single   reg read   {"readRegister": {     "chipID": 112,     "addr": 1545   } }
 *       multiple reg read   {"readRegister": [{"chipID": 112,     "addr": 1545},{"chipID": 113,     "addr": 1545}]    }
 **
 *  Output Returns values as {"reg" : xxx ,  "chipID":xxx , "addr":xxx,"value"}
 *'
*/
void parseReadRegister(StaticJsonDocument<2000> doc,bool debug=false)
{
   //Check if readRegister exisists
   if(doc.containsKey("readRegister"))   //Command readRegister
    {
      JsonArray array = doc["readRegister"].as<JsonArray>();
      
      if(array.isNull())
      {   // read single register
          uint8_t chipID =doc["readRegister"]["chipID"].as<int>();
          uint16_t  addr =doc["readRegister"]["addr"].as<int>();
          int32_t  regVal=alpide.readRegister(chipID, addr);
          char strStrReply[80];
          sprintf(strStrReply,"{ \"alpideRegister\": [ { \"addr\": %d , \"data\": %d}  ] }",addr,regVal);
          SerialInUse.println(strStrReply);

          
          if(debug)
          {  
            SerialInUse.print("cmd - readRegister(");
            SerialInUse.print(" chipID 0x");
            SerialInUse.print(chipID,HEX);
            SerialInUse.print(",addr 0x");
            SerialInUse.print(addr,HEX); 
            SerialInUse.println(")");            
          }
          

      }else
      { // read multiple registers
        int cnt=doc["readRegister"].size();
        for(int idx=0;idx<cnt;idx++)
        {
          uint8_t chipID =doc["readRegister"][idx]["chipID"].as<int>();
          uint16_t  addr =doc["readRegister"][idx]["addr"].as<int>();
          
          int32_t  regVal=alpide.readRegister(chipID, addr);
          char strStrReply[80];
          sprintf(strStrReply,"{ \"alpideRegister\": [ { \"addr\": %d , \" value\": %d}  ] }",addr,regVal);
          SerialInUse.println(strStrReply);

          if(debug)
          {
            SerialInUse.print("cmd - readRegister(");
            SerialInUse.print(" chipID 0x");
            SerialInUse.print(chipID,HEX);
            SerialInUse.print(",addr 0x");
            SerialInUse.print(addr,HEX); 
            SerialInUse.println(")");
          }
        }
      }
    }
}


/*  parseWriteRegister - checks is JSON root-key is "readRegister"
 *   
 *  Executes ALPIDE read of register using chipID and address
 *  JSON Example input
 *       single   reg read   {"writeRegister": {     "chipID": 112,   "addr": 1545  ,"data" : 100  } }
 *       multiple reg read   {"writeRegister": [{"chipID": 112, "addr": 1545 ,"data" : 100},{"chipID": 113, "addr": 1545 ,"data" : 101}]    }
 **
 *
*/
void parseWriteRegister(StaticJsonDocument<2000> doc)
{
   //Check if writeRegister exisists
   if(doc.containsKey("writeRegister"))   //Command writeRegister
    {
      JsonArray array = doc["writeRegister"].as<JsonArray>();
      
      if(array.isNull())
      {   // read single register
          uint8_t chipID =doc["writeRegister"]["chipID"].as<int>();
          uint16_t  addr =doc["writeRegister"]["addr"].as<int>();
          int16_t   data =doc["writeRegister"]["data"].as<int>();
          
          int  ret=alpide.writeRegister(chipID, addr,data);
          SerialInUse.println("ok");          
      }else
      { // read multiple registers
        int cnt=doc["writeRegister"].size();
        for(int idx=0;idx<cnt;idx++)
        {
          uint8_t chipID =doc["writeRegister"][idx]["chipID"].as<int>();
          uint16_t  addr =doc["writeRegister"][idx]["addr"].as<int>();
          int16_t   data =doc["writeRegister"][idx]["data"].as<int>();
       
          int  ret=alpide.writeRegister(chipID, addr,data);
          SerialInUse.println("ok");          
        }
      }
    }
}


/*  parseGetPixelFrameData  - Return events/pixel-hit data 
 *   
 *  format : streamRaw: Dumps raw data 
 *
 *   usage:
 *      {"getPixelFrameData": ["chipID" , 112 ] }
 *      {"getPixelFrameData": "debugData" }  - Returns debug data
 *    Param:
 *           ReturnRawData  <-  Returns binary 24 bits data 
 *                                    starting with header   0xA093FF
 *                                    end with trailer       0xB0FFFF
 *                               If not ready                0xFFFFFF
 */
void parseGetPixelFrameData(StaticJsonDocument<2000> doc,bool debug=false)
{
    if(doc.containsKey("getPixelFrameData")) 
    {
       //  IDEA...  ["getPixelFrameData"]== "returnRawData")
      JsonArray array = doc["getPixelFrameData"].as<JsonArray>();
       if(array.isNull())
       {  //chipID - is missing ... 
          if (doc["getPixelFrameData"]=="debugData")
          { //Send debug data...
            //                     Header           Region-id         data - short     data - long         trailer
            //                    #0xA093FF          #0xC0FFF           0x42BDFF        #0x030128          0xB0FFFF
            char outPixelData[]={0xA0,0x93,0xFF,   0xC0,0xFF,0xFF,   0x42,0xBD, 0xFF,  0x03,0x01,0x28,    0xB0,0xFF,0xFF };
            SerialInUse.write(outPixelData, sizeof(outPixelData));
          }
        }
       else
       {      
         int cnt=doc["getPixelFrameData"].size();
         for(int idx=0;idx<cnt;idx++)
            {
                uint8_t chipID =doc["getPixelFrameData"][idx]["chipID"].as<int>();
                alpide.dumpEventWords24Bit(SerialInUse,chipID,63000);
            }          

          
       }


     //To be implemented...
     //alpide.dumpEventWords24Bit(0x70,25000);
    }
}


void parseSendReset(StaticJsonDocument<2000> doc,bool debug=false)
{
    if(doc.containsKey("sendRest")) 
    {
          alpide.sendTrigger();
          SerialInUse.println("ok");
          SerialInUse.flush();    
    }
}


void parseSendTrigger(StaticJsonDocument<2000> doc,bool debug=false)
{
    if(doc.containsKey("sendTrigger")) 
    {
          alpide.sendTrigger();
          SerialInUse.println("ok");
          SerialInUse.flush();    
    }
}

void readJson()
{
    StaticJsonDocument<2000> doc;
    while (SerialInUse.available() == 0) {}     //wait for data available

    String teststr = Serial.readString();
    //Serial.print(teststr);
 
    DeserializationError err = deserializeJson(doc, teststr);

    if (err == DeserializationError::Ok) 
    {
      parseReadRegister(doc);
      parseWriteRegister(doc);

      parseSendReset(doc);
      parseSendTrigger(doc);

      parseGetPixelFrameData(doc);
      

      doc.clear();  //Empty doc (rdy for next commands..)
    } 
    else 
    {
      // Print error to the "debug" serial port
      SerialInUse.print("deserializeJson() returned ");
      SerialInUse.println(err.c_str());

      // Flush all bytes in the "link" serial port buffer
      while (Serial1.available() > 0)
        SerialInUse.read();
    }
}



/* Make things happening :) */
void loop() {


  //regReadWriteTest();
  readJson();

  //alpide.dumpEventWordsPy(SerialInUse,0x70,25000);
  uint16_t x[100];
  uint16_t y[100];

 // uint32_t buff[BUFF_SIZE];
    //delay(3000);
  
  // alpide.broadcastCommand(0x55) // Send trigger  TRIGGER 
                                   // Strobe lasts for 6000 clks (def i alpide init.. (ALPIDE_REG_FROMU_CFG2))

  // alpide.sendTrigger();
   //readJson();

  //int nHits=alpide.readEvent(0x78,x,y,100);
  // alpide.sendTrigger();
  //delay(1000);
   //if(bDoOnce)
   // alpide.sendTrigger();
  /* {
     bDoOnce=false;
      //alpide.sendTrigger();
      int id=0x70;
      for(int ii=0x0;ii<=8;ii+=8)
      {
        for(int id=0x70+ii;id<0x70+7+ii;id++)
        {
          
          // Debug get data to decode using python
          //alpide.dumpEventWordsPy(0x70,25000);
          
          
          uint16_t x[100];
          uint16_t y[100];
          SerialInUse.print(",");
          SerialInUse.print(alpide.readPixel(0x70,x,y,1000));
          */
          //alpide.readPixel(0x70,x,y,1000);
          
/*
          int nhits=alpide.readEvent(0x70,x,y,1000);
          if( nhits>0) 
          {         
            Serial1.println(nhits);
            Serial1.println(nhits);
          }
          */


           /*
          //int nHits=alpide.readEvent(id,x,y,100);

          int buf_cnt=alpide.readEventWords(id,buff,BUFF_SIZE);
          //if(nHits>=0)
          {
            SerialInUse.print("Chip ID 0x");
            SerialInUse.print(id,HEX);                        

            SerialInUse.print(" buff[0] : ");
            SerialInUse.print(buff[0],HEX);  

            SerialInUse.print(" Data-Type : ");
            uint16_t x,y;
            int retDecodeWord=alpide.decodeEventWord(buff[0],&x,&y);
            if(retDecodeWord<0)
              SerialInUse.print(retDecodeWord);
            else
               {
                  SerialInUse.print( " X : 0x");
                  SerialInUse.print( x, HEX);
                  SerialInUse.print( " Y : 0x");
                  SerialInUse.print( y, HEX);
               }

            SerialInUse.print(" buf_cnt : ");
            SerialInUse.println(buf_cnt);
          }

          */

          /*  Debug...
          int32_t lo=alpide.readRegister(id,ALPIDE_REG_DMU_FIFO_LO);  //Wait 20-30 sec for events...
          int32_t hi=alpide.readRegister(id,ALPIDE_REG_DMU_FIFO_HI);

          if (!((lo==0xFFFF) & (hi=0xFFFF)))
          {
            SerialInUse.print("Chip ID 0x");
            SerialInUse.print(id,HEX);
            
            SerialInUse.print("  Register ALPIDE_REG_DMU_FIFO_LO  Value ");
            SerialInUse.print(": 0x");
            SerialInUse.print(lo,HEX);
            
            SerialInUse.print("  Fifi High : 0x");
            SerialInUse.println(hi,HEX);
          }
          else
            SerialInUse.println("-");
            */
/*
        }
      }
   }
*/
//  SerialInUse.print("\r\nHits : ");
 // SerialInUse.println(nHits);
 //delay(1000);
}