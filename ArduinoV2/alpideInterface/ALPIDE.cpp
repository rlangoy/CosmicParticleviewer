/***************************************************************************
*  ALPIDE - library for the ALPIDE flex cable interface                    *
*                                                                          *
*  Copyright (C) 2022 Rune Langøy  <rune.langoy@gmail.com)                 * 
*  Copyright (C) 2017 Magnus Mager <Magnus.Mager@cern.ch>                  * 
*                                                                          *
*  This program is free software: you can redistribute it and/or modify    *
*  it under the terms of the GNU General Public License as published by    *
*  the Free Software Foundation, either version 3 of the License, or       *
*  (at your option) any later version.                                     *
*                                                                          *
*  This program is distributed in the hope that it will be useful,         *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of          *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
*  GNU General Public License for more details.                            *
*                                                                          *
*  You should have received a copy of the GNU General Public License       *
*  along with this program.  If not, see <http://www.gnu.org/licenses/>.   *
***************************************************************************/

#include <Arduino.h>
#include <time.h>
#include "ALPIDE.h"
#define ID2PLANE(id) (((id)&0x3)==0x2?(id)>>2:-1) /* this is for 4-bit chipIDs */
#define DEBUG false


ALPIDE::ALPIDE() {
  fADCSign    = false;
  fADCHalfLSB = false;

  
  //-- Init Input/Output---

  //digitalWrite(ALPIDE_PIN_CLK   ,LOW);
  //pinMode(ALPIDE_PIN_CLK   ,OUTPUT);
  //Def AVDD AND DVDD Pins
  pinMode(AVDD_EN ,OUTPUT);
  pinMode(DVDD_EN ,OUTPUT);

  //Power off -- Reset..
  digitalWrite(AVDD_EN, LOW);
  digitalWrite(DVDD_EN, LOW);

  delay(200); // TODO: Check time to stabilise?

  //Enable AVDD AND DVDD to the ALPIDES
  digitalWrite(AVDD_EN, HIGH);
  digitalWrite(DVDD_EN, HIGH);

  delay(200); // TODO: Check time to stabilise?

  // Init  Master Clock
  digitalWrite(OBMDB_MCLK_OE, HIGH);
  digitalWrite(OBMDB_MCLK_O, LOW);
  
  pinMode(OBMDB_MCLK_OE ,OUTPUT);    //Master clock is always output
  digitalWrite(OBMDB_MCLK_OE,HIGH);

  pinMode(OBMDB_MCLK_O  ,OUTPUT);
  //pinMode(OBMDB_MCLK_I  ,INPUT);
  
 // digitalWrite(ALPIDE_PIN_CTRL  ,LOW);
 // pinMode(ALPIDE_PIN_CTRL  ,OUTPUT);
 
  
  //Init  Data Control
  digitalWrite(OBMDB_DCTRL_OE, HIGH);
  digitalWrite(OBMDB_DCTRL_O, LOW);
  pinMode(OBMDB_DCTRL_OE ,OUTPUT);
  pinMode(OBMDB_DCTRL_O  ,OUTPUT);
  pinMode(OBMDB_DCTRL_I  ,INPUT);
 
 
  write_ctrl(LOW);

  
}

void ALPIDE::power(bool on) {
  if (on) {
    //  Enable AVDD AND DVDD to the ALPIDES
    digitalWrite(AVDD_EN, HIGH);
    digitalWrite(DVDD_EN, HIGH);
    delay(100); // TODO: Check time to stabilise?

    digitalWrite(ALPIDE_PIN_CLK   ,LOW);
    write_ctrl(HIGH);
    delay(100); // TODO: how long does it take to stabilise?
  }
  else {
    // Disable AVDD AND DVDD to the ALPIDES
    digitalWrite(AVDD_EN, HIGH);
    digitalWrite(DVDD_EN, HIGH);

    digitalWrite(ALPIDE_PIN_CLK   ,LOW);
    write_ctrl(LOW);
  }
}
void ALPIDE::initPixReadOut()
{

  /*
  broadcastWriteRegister(ALPIDE_REG_CMUDMU_CFG,0x001F); // disable manchester encoding, have initial token
  broadcastWriteRegister(0x0004,0x0000); //# disable BUSY monitoring
  broadcastWriteRegister(0x0001,0x020D); //# readout via CMU
  broadcastWriteRegister(0x0005,60000); //# strobe length
  broadcastWriteRegister(0x0602,147);   // # VRESETD
  */

  broadcastWriteRegister(ALPIDE_REG_FROMU_CFG1,0x0000); // disable busy monitoring

  broadcastWriteRegister(ALPIDE_REG_MODE_CTRL,0x020D);  // readout via CMU
  broadcastWriteRegister(ALPIDE_REG_FROMU_CFG2,  TRIGGER_STORBE_LENGTHT); // strobe length in clk cycles (Default 6000) ( 0 = one clock cycle strobe 
  broadcastWriteRegister(ALPIDE_REG_DAC_VRESETD,APIDE_DEFAULT_VRESETD);  //VRESETD
  //broadcastWriteRegister(0x0604,0x3C); //VCASN
  //broadcastWriteRegister(0x060E,30); // ITHR
 
  // disable all masking  -- Needed ???
  /*
  broadcastWriteRegister(0x0487,0x0000);
  broadcastWriteRegister(0x0500,0x0000);
  broadcastWriteRegister(0x0487,0xFFFF);
  broadcastWriteRegister(0x0487,0x0000);
  broadcastWriteRegister(ALPIDE_REG_MODE_CTRL,0x0209); // triggered readout, no clustering, fast matrix readout, no skewing, not cluck gating, read from CMU
  broadcastCommand(ALPIDE_CMD_RORST);
  */
  /*
   alpide.global_cmd(0xD2) # GRST
    alpide.global_cmd(0xE4) # PRST
    alpide.write_reg(0x0F,0x0010,0x007F) # disable manchester, initial token
    alpide.write_reg(0x0F,0x0004,0x0000) # disable BUSY monitoring
    alpide.write_reg(0x0F,0x0001,0x020D) # readout via CMU
    alpide.write_reg(0x0F,0x0005,60000)  # strobe length
    alpide.write_reg(0x0F,0x0602,147)    # VRESETD
*/
}
int ALPIDE::init() {
  clk(200); // TODO: needed?
  broadcastCommand(ALPIDE_CMD_GRST);  //Global reset
  broadcastCommand(ALPIDE_CMD_PRST);
  delay(100);
  broadcastCommand(ALPIDE_CMD_GRST);  //Global reset
  delay(100);
  broadcastCommand(ALPIDE_CMD_PRST);
  delay(100);

  broadcastWriteRegister(ALPIDE_REG_CMUDMU_CFG,0x0018); // disable manchester encoding, have initial token
  //Alpides Ready for reg R/W

  initPixReadOut();    //Enable Pixel-Readout

  return 0;
}

/*
Chip id 0x70 pixel:  39 , 490
Chip id 0x70 pixel:  70 , 258
Chip id 0x70 pixel:  75 , 254
Chip id 0x70 pixel:  512 , 439
Chip id 0x70 pixel:  549 , 64
Chip id 0x70 pixel:  817 , 503
*/

void ALPIDE::maskPxls()
{
   /*maskPixel(0x70,39,490,true);
   maskPixel(0x70,70,258,true);
   maskPixel(0x70,75,254,true);
   maskPixel(0x70,512,439,true);
   maskPixel(0x70,817,503,true);*/

   for(int i=0;i<pixMasked;i++)
   {
     maskPixel(pixMaskList[i].chipID,pixMaskList[i].x,pixMaskList[i].y,true);
   }
}

void ALPIDE::sendTrigger()
{
  //sendReset();
   broadcastCommand(ALPIDE_CMD_PRST);     // Pixel matrix reset
 //  broadcastCommand(ALPIDE_CMD_RORST);   //Reset Readout (RRU/TRU/DMU) reset 
  clk(10);
  /*
  broadcastWriteRegister(ALPIDE_REG_MODE_CTRL,0x020D);  // readout via CMU
  broadcastWriteRegister(ALPIDE_REG_FROMU_CFG2,  TRIGGER_STORBE_LENGTHT); // strobe length in clk cycles (Default 6000) ( 0 = one clock cycle strobe 
  broadcastWriteRegister(ALPIDE_REG_DAC_VRESETD,APIDE_DEFAULT_VRESETD);  //VRESETD
*/


  //maskPxls();     // Apply Masks
  broadcastCommand(0x55);        // Send trigger   
  
  clk(TRIGGER_STORBE_LENGTHT);   // Nedded ?? (to strobe data ... ???)

}

void ALPIDE::sendReset()
{

  // Send ALPIDE_CMD_RORST in stead of complete reset ? + 10clk
  //  rorst(self):
  //      self.global_cmd(0x63)    
  //

  broadcastCommand(ALPIDE_CMD_GRST);  //Global reset
  broadcastCommand(ALPIDE_CMD_PRST);
  broadcastCommand(ALPIDE_CMD_RORST);   //Reset Readout (RRU/TRU/DMU) reset 
  clk(10);


  //alpide_all.write_reg(0x0010,0x007F)   //Diasable manchester + Enable Double Data Rate (Previous Chip ID =0xF)
  broadcastWriteRegister(ALPIDE_REG_CMUDMU_CFG,0x0018); // disable manchester encoding, have initial token

  broadcastWriteRegister(ALPIDE_REG_FROMU_CFG1,0x0000); // disable busy monitoring

  broadcastWriteRegister(ALPIDE_REG_MODE_CTRL,0x020D);  // readout via CMU
  broadcastWriteRegister(ALPIDE_REG_FROMU_CFG2,  TRIGGER_STORBE_LENGTHT); // strobe length in clk cycles (Default 6000) ( 0 = one clock cycle strobe 
  broadcastWriteRegister(ALPIDE_REG_DAC_VRESETD,APIDE_DEFAULT_VRESETD);  //VRESETD
}


/*
int ALPIDE::trigger(int strobeMilliSeconds) {
  // start bit
  //digitalWrite(ALPIDE_PIN_CTRL,LOW );
  //pinMode(ALPIDE_PIN_CTRL,OUTPUT);
  write_ctrl(LOW);
  clk();
  // trigger bits (0x55, alternatives: 0xB1,0xC9,0x2D, or not caring about hamming: 0bxxxxxx01), LSB first
  //digitalWrite(ALPIDE_PIN_CTRL,HIGH);clk();
  //digitalWrite(ALPIDE_PIN_CTRL,LOW );clk(); 
  //digitalWrite(ALPIDE_PIN_CTRL,HIGH);clk();
  //digitalWrite(ALPIDE_PIN_CTRL,LOW );clk(); 
  //digitalWrite(ALPIDE_PIN_CTRL,HIGH);clk();
  //digitalWrite(ALPIDE_PIN_CTRL,LOW );clk();
  write_ctrl(HIGH); clk();
  write_ctrl(LOW); clk();
  write_ctrl(HIGH); clk();
  write_ctrl(LOW); clk();
  write_ctrl(HIGH); clk();
  write_ctrl(LOW); clk();

  delay(strobeMilliSeconds);
  //digitalWrite(ALPIDE_PIN_CTRL,HIGH);clk();
  //digitalWrite(ALPIDE_PIN_CTRL,LOW );clk();
  //digitalWrite(ALPIDE_PIN_CTRL,HIGH);clk();
  write_ctrl(HIGH); clk();
  write_ctrl(LOW); clk();
  write_ctrl(HIGH); clk();
  
  return 0;
}
*/

/*
*  int decodeEventWord(uint32_t wDmuFifoData,uint16_t *x,uint16_t *y)
*
*  Params:
*    wDmuFifoData           - word to decode (LPIDE_REG_DMU_FIFO -Pixel-event data)
*    x                      - activated pixel pos i x-dir
*    y                      - activated pixel pos i y-dir
*  Retrun
*    Error Codes
*     -1   chip not ready
*     -2   chip empty
*     -3   chip header
*     -4   chip trailer
*     -5   data regional header 
*
*    0 - No error (y and y) coordinates read
*  Usage:
*    uint32_t  wordDecode=0xf0ff;
*    uint16_t x,y;
*    int decodeOk =decodeEventWord (wordDecode,&x,&y);
*/
int ALPIDE::decodeEventWord(uint32_t wDmuFifoData,uint16_t *x,uint16_t *y)
{
  *x=10;
  *y=10;

  if (wDmuFifoData == 0xFFFFFF)  //  Chip not ready
    return -1;

  uint32_t maskedData=wDmuFifoData & 0xF000FF;
  if ( maskedData == 0xE000FF)  //  chip empty
     return -2; 

  if ( maskedData == 0xA000FF)  // chip header
     return -3; 

  if ( maskedData == 0xB000FF)  // chip trailer
     return -4; 

  if ( (wDmuFifoData & 0xC00000) == 0x400000) {  
    //  x= (d>>9&0x3FE|(d^d>>1)&0x1
    // y = d>>1&0x1FF
    *x= ((wDmuFifoData>>9) &0x3FE) | (wDmuFifoData^wDmuFifoData>>1) &0x01;
    *y= (wDmuFifoData>>1)&0x1FF;
    return 0;
  }
  

/*

  else if ((word&0xC0 00)==0x40 00) {
    *x=*currentRegion<<5|word>>9&0x1E|(word^word>>1)&0x1;
    *y=word>>1&0x1FF;

*/
/*
  if (wDmuFifoData&0xC0 0000 ==0x40 0000) {

    //*x=*currentRegion<<5|word>>9&0x1E|(word^word>>1)&0x1;
    //*y=word>>1&0x1FF;
    *y=  (wDmuFifoData>>1) & 0x1FF;
    *x=  0;
    return -5;
  }
*/

  return -5;  // X and Y was read
}

/*
*  int readEventWords(uint8_t chipID,uint32_t *wDataTable, uint16_t maxEventReturn)
*
*  Params:
*   chipID                -  ALPIDE Chip ID
*   wDataTable table      -  returns list of 32bits - ALPIDE_REG_DMU_FIFO <- Register values
*   maxEventReturn        -  Max items to store in wDataTable
*  Retrun
*   number of words added to the table wDataTable
*  Usage:
*  #define BUFF_SIZE 100
*  ...
*  uint32_t buff[BUFF_SIZE];
*  int buf_cnt=alpide.readEventWords(id,buff,BUFF_SIZE);
*/
int ALPIDE::readEventWords(uint8_t chipID,uint32_t *wDataTable, uint16_t maxEventReturn)
{
  int wordCnt=0;
  do
  {
    int32_t lo=readRegister(chipID,ALPIDE_REG_DMU_FIFO_LO);
    int32_t hi=readRegister(chipID,ALPIDE_REG_DMU_FIFO_HI);

    uint32_t word=((hi<<16) |lo)  &0xFFFFFF;   // Strip high byte of ALPIDE_REG_DMU_FIFO_HI
    
 //Debug.. 
 /*   Serial1.print(" hi : 0x");
    Serial1.print(hi,HEX);
    Serial1.print(" lo : 0x");
    Serial1.print(lo,HEX);
    Serial1.print(" word : 0x");
    Serial1.println(word,HEX);
*/
    wDataTable[wordCnt]=word;              // Append FIFO Data  to list

    wordCnt++;    
    //Dirty-Logic.. stop loop if no more data to fill into buffer
    if ((word>>16)&0xF0==0xB0)  break;   // chip trailer   (end of data)
    if ((word>>16)&0xF0==0xE0)  break;   // chip empty
    if  ( word==0xFFFFFF )      break;   // Not ready    
  }while(wordCnt<maxEventReturn);        // Break loop if MAX items stored..

  return wordCnt;   // Return buff size
}

/*   Function Putpose
*     Dumps ALPIDE DMU_FIFO data as a .cvs format
*    Usage  
*     Clip the terminal output and save as .\debugTools\trgData.csv
*     Python intepreter : .\debugTools\trgrdo.py
*/
void ALPIDE::dumpEventWords24Bit(HardwareSerial &SerialInOut,uint8_t chipID,uint16_t maxEventReturn)
{
  int wordCnt=0;
  bool brkLoop=false;
  //clk(6000);   //Nedded ?? Sendt after sendTrigger
  //Serial1.print("[");
  do
  {
    wordCnt++;

    int32_t lo=readRegister(chipID,ALPIDE_REG_DMU_FIFO_LO);
    int32_t hi=readRegister(chipID,ALPIDE_REG_DMU_FIFO_HI);
    
    uint32_t word=((hi<<16) |lo)  &0x00FFFFFF;   // Strip high byte of ALPIDE_REG_DMU_FIFO_HI
    
    SerialInOut.write((byte)(hi   ) &0xFF);  // Tx   lo byte 
    SerialInOut.write((byte)(lo>>8) &0xFF);  // Tx high byte
    SerialInOut.write((byte)(lo   ) &0xFF);  // Tx   lo byte 

    if ((hi&0xF0)==0xB0)           brkLoop=true;   // chip trailer   (end of data)
    if ((hi&0xF0)==0xE0)           brkLoop=true;   // chip empty

    if ( word==0xFFFFFF )          brkLoop=true;    // Not ready  ???  
    if ( wordCnt>=maxEventReturn)  brkLoop=true;    // Max words read....

  }while(brkLoop==false);        // Break loop if MAX items stored..

    SerialInOut.write(0xFF);  // Tx   lo byte 
    SerialInOut.write(0xFF);  // Tx high byte
    SerialInOut.write(0xFF);  // Tx   lo byte 

}

/*   Function Purpose
*     Dumps ALPIDE DMU_FIFO data as a .cvs format
*    Usage  
*     Clip the terminal output and save as .\debugTools\trgData.csv
*     Python intepreter : .\debugTools\trgrdo.py
*/
void ALPIDE::dumpEventWordsPy(HardwareSerial &SerialInOut,uint8_t chipID,uint16_t maxEventReturn)
{
  int wordCnt=0;
  bool brkLoop=false;
  //Serial1.print("[");
  do
  {
    if(wordCnt!=0) SerialInOut.print(",");  // Dont print , before the first number
    wordCnt++;

    int32_t lo=readRegister(chipID,ALPIDE_REG_DMU_FIFO_LO);
    int32_t hi=readRegister(chipID,ALPIDE_REG_DMU_FIFO_HI);
    
    uint32_t word=((hi<<16) |lo)  &0x00FFFFFF;   // Strip high byte of ALPIDE_REG_DMU_FIFO_HI
    
    SerialInOut.print("0x");
    SerialInOut.print(word,HEX);   

    if ((hi&0xF0)==0xB0)           brkLoop=true;   // chip trailer   (end of data)
    if ((hi&0xF0)==0xE0)           brkLoop=true;   // chip empty

    if ( word==0xFFFFFF )          brkLoop=true;    // Not ready  ???  
    if ( wordCnt>=maxEventReturn)  brkLoop=true;    // Max words read....

  }while(brkLoop==false);        // Break loop if MAX items stored..
  //Serial1.println("]");
  SerialInOut.println("");
}


int32_t ALPIDE::readFifo(uint8_t chipID)
{
  //return ((readRegister(chipID,ALPIDE_REG_DMU_FIFO_HI)<<16) | readRegister(chipID,ALPIDE_REG_DMU_FIFO_LO))&0x00FFFFFF;
  int32_t lo=readRegister(chipID,ALPIDE_REG_DMU_FIFO_LO);
  int32_t hi=readRegister(chipID,ALPIDE_REG_DMU_FIFO_HI);
    
  uint32_t word=((hi<<16) |lo)  &0x00FFFFFF;   // Strip high byte of ALPIDE_REG_DMU_FIFO_HI
  return word;
}

/*
*    Uses Global vars:
*  mWaitForChipHeader=true;
*       .....
*
*     return -1   - chip is Empty
             -2    -        Not ready  
             -3    -        to many word reads (should not happen)
*/
//Read and return first px found
int  ALPIDE::readPixel(uint8_t chipID,uint16_t *x,uint16_t *y ,uint16_t maxEventReturn)
{
    int32_t word;
    int wordCnt=0;
    // mWaitForChipHeader=true;
    
    //Wait for chip header
    do {   
        word=readFifo(chipID);        
        //Serial1.print("0x");
        //Serial1.print(word,HEX); 




        wordCnt++;
        if ((word&0xF000FF)==0xB000FF)  // chip trailer   (end of data)
        {
             mWaitForChipHeader=true;              
             Serial1.println("\r\nchipTrailer");               
        }
        if ((word&0xF000FF)==0xA000FF)  // chip header   (end of data)
        {
          word=readFifo(chipID);      // read next
          Serial1.println("\r\nchipHeader");  
           mWaitForChipHeader=false;
           wordCnt++;            
        }               
          
        if ((word&0xF000FF)==0xE000FF)  // chip empty    
           return -1;
      
        if ( word==0xFFFFFF )          //   Not ready 
          return  -2;
        if(wordCnt>maxEventReturn)      // to many words error
          return  -3;        
    }while(mWaitForChipHeader);

  //--Decode data..
  //if(( word&0xE0FFFF ) == 0xC0FFFF)  //# region header
  if ((word&0xE000FF)==0xC000FF) 
  {
      uint8_t reg= (word>>16) & 0x1F;
      Serial1.print("\r\n Region header  0x");  
      Serial1.println(reg,HEX);
  }

  if ((word&0xC00000)==0x000000) 
  {

    //Serial1.println("\r\n data long");  
  }
  

  if ((word&0xC000FF)==0x4000FF) 
  {

    //Serial1.println("\r\n data short-----");  
  }
  

  return 0;
}


/*
int ALPIDE::readEventWord(uint8_t chipID,uint16_t maxEventReturn)
{
  int wordCnt=0;
  bool brkLoop=false;
  //Serial1.print("[");
  do
  {
    if(wordCnt!=0) Serial1.print(",");  // Dont print , before the first number
    wordCnt++;

    int32_t lo=readRegister(chipID,ALPIDE_REG_DMU_FIFO_LO);
    int32_t hi=readRegister(chipID,ALPIDE_REG_DMU_FIFO_HI);
    
    uint32_t word=((hi<<16) |lo)  &0x00FFFFFF;   // Strip high byte of ALPIDE_REG_DMU_FIFO_HI
    
    Serial1.print("0x");
    Serial1.print(word,HEX);   

    if ((hi&0xF0)==0xB0)           brkLoop=true;   // chip trailer   (end of data)
    if ((hi&0xF0)==0xE0)           brkLoop=true;   // chip empty

    if ( word==0xFFFFFF )          brkLoop=true;    // Not ready  ???  
    if ( wordCnt>=maxEventReturn)  brkLoop=true;    // Max words read....

  }while(brkLoop==false);        // Break loop if MAX items stored..
  //Serial1.println("]");
  Serial1.println("");

  return word;
}
*/




/*
def decodeevent(words):
    assert len(words)>0
    reg=None
    hits=[]
    if   words[0]&0xF000FF==0xE000FF: # chip empty
        pass
    elif words[0]&0xF000FF==0xA000FF: # chip header
        for word in words[1:-1]:
            if   word&0xC00000==0x000000: # data long
                assert reg is not None
                addr=reg<<14|word>>8&0x3FFF
                hits.append(addr)
                hm=word&0xFF
                while hm:
                    addr+=1
                    if hm&1: hits.append(addr)
                    hm>>=1
            elif word&0xC000FF==0x4000FF: # data short
                assert reg is not None
                hits.append(reg<<14|word>>8&0x3FFF)
            elif word&0xE0FFFF==0xC0FFFF: # region header
                reg=word>>16&0x1F
            else: raise ValueError('bad word: %06X'%word)
        if words[-1]&0xF000FF==0xB000FF: # chip trailer
            pass
        else: raise ValueError('bad last word: %06X'%words[-1])
    else: raise ValueError('bad first word: %06X'%words[0])

    xy=[(d>>9&0x3FE|(d^d>>1)&0x1,d>>1&0x1FF) for d in hits]
    return xy
    */



int ALPIDE::readRawData(uint8_t plane,uint16_t *buf,int n) {
  int i=0;
  while (n>0) {
    int32_t lo=readRegister(plane,ALPIDE_REG_DMU_FIFO_LO);
    int32_t hi=readRegister(plane,ALPIDE_REG_DMU_FIFO_HI);
    if (lo<0) return -1;
    if (hi<0) return -2;
    if (lo&0x00FF!=0x00FF) return -3;
    if (hi==0xFF && lo==0xFFFF) break;
    buf[i++]=hi<<8|lo>>8;
    --n;
  }
  return i;
}





int ALPIDE::readEvent(uint8_t plane,uint16_t *x,uint16_t *y,int n) {
  int type;
  int8_t   currentRegion=-1;
  uint16_t xread           ;
  uint16_t yread           ;
  uint8_t  bc              ;
  int8_t   planeread       ;
  uint8_t  flags           ;
  uint16_t rawdata;
   //Serial1.println("Hi");
  int ret=readRawData(plane,&rawdata,1);
  if (ret==0) return -10;
  if (ret<0) return -100+ret;
  type=decodeRawDataWord(rawdata,&currentRegion,&xread,&yread,&bc,&planeread,&flags);
  
  //if(DEBUG)
  if(type!=-1)
  {
  Serial.print("Type ");
  Serial.println(type,HEX);
  Serial.print("DATA CHIP HEADER: ");
  Serial.println(ALPIDE_DATA_CHIP_HEADER,HEX);
  Serial.print("EMPTY CHIP HEADER: ");
  Serial.println(ALPIDE_DATA_CHIP_EMPTY,HEX);
  Serial.print("chip-ID: 0x");
  Serial.println(plane,HEX);
  
  Serial.print("Recieved ID: ");
  Serial.println(planeread);
  }
  
  if (!(type==ALPIDE_DATA_CHIP_HEADER || type==ALPIDE_DATA_CHIP_EMPTY)) return -1;

  if (planeread!=plane) return -2;
  if (type==ALPIDE_DATA_CHIP_EMPTY) return 0;
  int i=0;
  while (true) {
    ret=readRawData(plane,&rawdata,1);
    if (ret<0) return -1;
    if (ret==0) return -10;
    type=decodeRawDataWord(rawdata,&currentRegion,&xread,&yread,&bc,&planeread,&flags);
    switch (type) {
      case ALPIDE_DATA_REGION_HEADER:
        break;
      case ALPIDE_DATA_DATA_SHORT:
        if (n==0) return -3;
        x[i]=xread;
        y[i]=yread;
        ++i;
        --n;
        break;
      case ALPIDE_DATA_CHIP_TRAILER:
        //if (flags!=0) return -200-flags;
        return i;
      default:
        return -5;
    }
  }
}

int ALPIDE::writeRegister(uint8_t chipID,uint16_t addr,uint16_t data) {
  writeByte(ALPIDE_CMD_WROP);
  writeByte(chipID);
  for (int i=0;i<2;++i) writeByte(addr>>(i*8)&0xFF);
  for (int i=0;i<2;++i) writeByte(data>>(i*8)&0xFF);
  clk(10); // TODO: at least writing to DACs need some clocks to be effective...
  return 0;
}



int32_t ALPIDE::debugReadRegister(uint8_t chipID,uint16_t addr) {
  writeByte(ALPIDE_CMD_RDOP);
  writeByte(chipID);
  for (int i=0;i<2;++i) writeByte(addr>>(i*8)&0xFF);
  // bus turn around:
  clk(5);                   // -  0- 4: 5 cycles IDLE driven by master
  //pinMode(ALPIDE_PIN_CTRL,INPUT);
  read_ctrl(); //set PIN_CTRL as INPUT
  // TODO: double check this timing with chip designers. Number in the code seem to work...
  clk(5);                   // -  5- 9: 5 cycles with high impeance, ingnoring the bus
  bool idle1 =checkIdle(4); // - 10-14: 5 extra IDLE driven by slave
  int  chipid=readByte();   // - 15-24: chip ID character
  bool idle2 =checkIdle(0); // - 25-25: 1 extra IDLE driven by slave
  int  datal =readByte();   // - 26-35: data low character
  bool idle3 =checkIdle(0); // - 36-37: 2 extra IDLE driven by slave
  int  datah =readByte();   // - 38-47: data high character
  bool idle4 =checkIdle(5); // - 48-52: 5 extra IDLE driven by slave
  clk(4);                   // - 53-56: 4 cycles ingore
  //digitalWrite(ALPIDE_PIN_CTRL,HIGH);
  //pinMode(ALPIDE_PIN_CTRL,OUTPUT);
  //write_ctrl(HIGH);
  clk(8);                   // - 57-64: 8 cycles IDLE driven by master
  write_ctrl(HIGH);
  Serial1.print("idle1 : ");
  Serial1.println(idle1);
  Serial1.print("chipid :0b ");
  Serial1.println(chipid,BIN);
  
  Serial1.print("datal :0b ");
  Serial1.println(datal,BIN);

  Serial1.print("datah :0b ");
  Serial1.println(datah,BIN);

  
  //if (!idle1)  return -1;
  if (chipid!=chipID) {Serial.println(chipid);return -5;}
  if (!idle2)  return -2;
  if (datal<0) return -6;
  if (!idle3)  return -3;
  if (datah<0) return -7;
  if (!idle4)  return -4;
  return (uint16_t)datah<<8|datal;
}


int32_t ALPIDE::readRegister(uint8_t chipID,uint16_t addr) {
  writeByte(ALPIDE_CMD_RDOP);
  writeByte(chipID);
  for (int i=0;i<2;++i) writeByte(addr>>(i*8)&0xFF);   // 40 clcks (4x  (byte + start + stop)
  // bus turn around:
  clk(1);
  write_ctrl(HIGH);
 
  clk(5);                   // -  0- 4: 5 cycles IDLE driven by master  

  read_ctrl();              //set PIN_CTRL as INPUT (release the bus)

  clk(5);                   // -  5- 9: 5 cycles with high impeance, ingnoring the bus

  bool idle1 =checkIdle(5); // - 10-14: 5 extra IDLE driven by slave

  int  chipid=readByte();   // - 15-24: chip ID character (adds +2 clk if start not found)

  bool idle2 =checkIdle(0); // - 25-25: 1 extra IDLE driven by slave
  int  datal =readByte();   // - 26-35: data low character
  bool idle3 =checkIdle(0); // - 36-37: 2 extra IDLE driven by slave
  int  datah =readByte();   // - 38-47: data high character
  bool idle4 =checkIdle(5); // - 48-52: 5 extra IDLE driven by slave
  clk(4);                   // - 53-56: 4 cycles ingore
  clk(8);                   // - 57-64: 8 cycles IDLE driven by master
  write_ctrl(HIGH);

  clk(8);                   // Flush reg ???
  //clk(42);                   // Flush reg ???
/*
  //Debugg
  if(chipid!=0xFFFFFFFF)
  {   
   Serial1.print("chipid: 0x");
   Serial1.println(chipid,HEX);
   Serial1.print("chipid :0b ");
   Serial1.println(chipid,BIN);

   Serial1.print("datal : 0b");
   Serial1.println(datal,BIN);
   Serial1.print("datal : 0x");
   Serial1.println(datal,HEX);

   Serial1.print("datah : 0b");
   Serial1.println(datah,BIN);
   Serial1.print("datah : 0x");
   Serial1.println(datah,HEX);
  }

*/

  if (!idle1)  return -1; //OK
 
 
  if (chipid!=chipID) {return -5;}
  if (!idle2)  return -2;
  if (datal<0) return -6;
  if (!idle3)  return -3;
  if (datah<0) return -7;
  if (!idle4)  return -4;
  return (uint16_t)datah<<8|datal;
}

int ALPIDE::broadcastCommand(uint8_t cmd) {
  writeByte(cmd);
  return 0;
}

int ALPIDE::broadcastWriteRegister(uint16_t addr,uint16_t data) {
  writeByte(ALPIDE_CMD_WROP);
  writeByte(0x0F);
  for (int i=0;i<2;++i)
    writeByte(addr>>(i*8)&0xFF);
  for (int i=0;i<2;++i)
    writeByte(data>>(i*8)&0xFF);
  return 0;
}

//Sync write ALPIDE_PIN_CTRL_P / ALPIDE_PIN_CTRL_N
void ALPIDE::write_ctrl(bool bHigh)
{
  //Enable CTRL Output
  digitalWrite(OBMDB_DCTRL_OE,HIGH); 
  //Set CRTL state
  digitalWrite(OBMDB_DCTRL_O  ,bHigh);
}

bool ALPIDE::read_ctrl()
{ 
  //Disable CTRL Output (Enable signal read)
  digitalWrite(OBMDB_DCTRL_OE,LOW); 

  // Return CTRL State
  return digitalRead(OBMDB_DCTRL_I); 
}


void ALPIDE::writeByte(uint8_t data) {
  //Enable CTRL Output (Enable signal read)
  digitalWrite(OBMDB_DCTRL_OE,HIGH); 
 
  // start bit
  write_ctrl(LOW);  
  clk();

  // data bits, LSB first
  for (int i=0;i<8;++i) {
    write_ctrl(data>>i&0x1);
    clk();
  }

  // stop bit
  write_ctrl(HIGH);
  clk();
  
}

int16_t ALPIDE::readByte() {

  read_ctrl();  // sets CTRL as INPUT  

  bool startfound=false;  

  //IDE-state not read always 
  //   THE OB slaves needs two clks more than the master chip
  //   Apide-Usermanual 3.1.3(p32) IDLE can be up to 42 cycles..
  for(int i=0;i<42;i++)
  { clk();
    startfound=(digitalRead(OBMDB_DCTRL_I)==LOW );  
    if(startfound)  //Break loop if start was found
      break;
  }

  uint8_t data=0;
  // data bits, LSB first
  for (int i=0;i<8;++i) {
    clk();
    data>>=1;
    if (read_ctrl()==HIGH) data|=0x80;    // Insert bit    
  }

 // stop bit
  clk();
 
  bool stopfound =(read_ctrl()==HIGH);
  if (!startfound) return -1;
  if (!stopfound ) return -2;

  return data;
}

/* Not wokring
  When not using manscheste encoding IDLE and Released line looks the same...
*/
int ALPIDE::checkIdle(int n) {
  read_ctrl();
  bool idle=true;
  for (int i=0;i<n;++i) {
    clk();
    //if (digitalRead(ALPIDE_PIN_CTRL)!=HIGH) idle=false;
    if (read_ctrl()!=HIGH) idle=false;
  }
  return idle;
}



void ALPIDE::clk(int n) {
  //ENABLE  Clock Output (for OB this should alwais be on...)
 // digitalWrite(OBMDB_MCLK_OE,HIGH); 

  for (int i=0;i<n;++i) {      
    digitalWrite(OBMDB_MCLK_O,HIGH );
    digitalWrite(OBMDB_MCLK_O,LOW);   
  }

}

int ALPIDE::decodeRawDataWord(uint16_t word,int8_t *currentRegion,uint16_t *x,uint16_t *y,uint8_t *bc,int8_t *plane,uint8_t *flags) {
  if      ((word&0xF000)==0xA000) {
    *bc           =word&0xFF;
    *plane        =ID2PLANE(word>>8&0x0F);
    *currentRegion=-1;
    return ALPIDE_DATA_CHIP_HEADER;
  }
  else if ((word&0xF0FF)==0xB0FF) {
    *currentRegion=-1;
    *flags=word>>8&0xF;
    return ALPIDE_DATA_CHIP_TRAILER;
  }
  else if ((word&0xF000)==0xE000) {
    *bc           =word&0xFF;
    *plane        =ID2PLANE(word>>8&0x0F);
    *currentRegion=-1;
    return ALPIDE_DATA_CHIP_EMPTY;
  }
  else if ((word&0xE0FF)==0xC0FF) {
    *currentRegion=word>>8&0x1F;
    return ALPIDE_DATA_REGION_HEADER;
  }
  else if ((word&0xC000)==0x4000) {
    *x=*currentRegion<<5|word>>9&0x1E|(word^word>>1)&0x1;
    *y=word>>1&0x1FF;
    return ALPIDE_DATA_DATA_SHORT;
  }
  // TODO: will busy transactions appear and will they be reported?
  else if (word       ==0xF1FF) {
    return ALPIDE_DATA_BUSY_ON;
  }
  else if (word       ==0xF0FF) {
    return ALPIDE_DATA_BUSY_OFF;
  }
  else {
    return -1;
  }
}

void ALPIDE::maskAllPixels(uint8_t chipID,bool enable) {
  writeRegister(chipID,0x0487,0x0000);
  writeRegister(chipID,0x0500,enable?0x0002:0x0000);
  writeRegister(chipID,0x0487,0xFFFF);
  writeRegister(chipID,0x0487,0x0000);
}

void ALPIDE::enablePulserAllPixels(uint8_t chipID,bool enable) {
  writeRegister(chipID,0x0487,0x0000);
  writeRegister(chipID,0x0500,enable?0x0003:0x0001);
  writeRegister(chipID,0x0487,0xFFFF);
  writeRegister(chipID,0x0487,0x0000);
}

void  ALPIDE::addMaskPixel  (uint8_t chipID,uint16_t x,uint16_t y)
{
  if(pixMasked<MaxMaskPix)
  {
   pixMaskList[pixMasked].chipID=chipID;
   pixMaskList[pixMasked].x=x;
   pixMaskList[pixMasked].y=y;
   pixMasked++;
  }
}         

void ALPIDE::maskPixel(uint8_t chipID,uint16_t x,uint16_t y,bool enable) {
  writeRegister(chipID,0x0487,0x0000);
  writeRegister(chipID,0x0500,enable?0x0002:0x0000);
  writeRegister(chipID,(x&0x3E0)<<6|0x0400|(1+(x>>4&0x1)),1<<(x&0xF));
  writeRegister(chipID,(y&0x1F0)<<7|0x0404               ,1<<(y&0xF));
  writeRegister(chipID,0x0487,0x0000);
}

void ALPIDE::enablePulserPixel(uint8_t chipID,uint16_t x,uint16_t y,bool enable) {
  writeRegister(chipID,0x0487,0x0000);
  writeRegister(chipID,0x0500,enable?0x0003:0x0001);
  writeRegister(chipID,(x&0x3E0)<<6|0x0400|(1+(x>>4&0x1)),1<<(x&0xF));
  writeRegister(chipID,(y&0x1F0)<<7|0x0404               ,1<<(y&0xF));
  writeRegister(chipID,0x0487,0x0000);
}


/* ------------------------------------------------------
 * Sets the DAC Monitor multiplexer
 *
 * Parameter  : the Index of the DAC register.
 *              the IRef value
 *
 *Defaulr  Iref= IREF_100uA
 *
 */
void ALPIDE::setTheDacMonitor(uint8_t chipID,uint16_t ADacReg, TDACMonIref IRef)
{
  int VDAC, IDAC;
  uint16_t Value;
  switch (ADacReg) {
  case ALPIDE_REG_DAC_VRESETP:
    VDAC = 4;
    IDAC = 0;
    break;
  case ALPIDE_REG_DAC_VRESETD:
    VDAC = 5;
    IDAC = 0;
    break;
  case ALPIDE_REG_VCASP:
    VDAC = 1;
    IDAC = 0;
    break;
  case ALPIDE_REG_DAC_VCASN:
    VDAC = 0;
    IDAC = 0;
    break;
  case ALPIDE_REG_DAC_VPULSEH :
    VDAC = 2;
    IDAC = 0;
    break;
  case ALPIDE_REG_DAC_VPULSEL:
    VDAC = 3;
    IDAC = 0;
    break;
  case ALPIDE_REG_DAC_VCASN2:
    VDAC = 6;
    IDAC = 0;
    break;
  case ALPIDE_REG_VCLIP:
    VDAC = 7;
    IDAC = 0;
    break;
  case ALPIDE_REG_VTEMP:
    VDAC = 8;
    IDAC = 0;
    break;
  case ALPIDE_REG_IAUX2:
    IDAC = 1;
    VDAC = 0;
    break;
  case ALPIDE_REG_IRESET:
    IDAC = 0;
    VDAC = 0;
    break;
  case ALPIDE_REG_IDB:
    IDAC = 3;
    VDAC = 0;
    break;
  case ALPIDE_REG_IBIAS:
    IDAC = 2;
    VDAC = 0;
    break;
  case ALPIDE_REG_DAC_ITHR:
    IDAC = 5;
    VDAC = 0;
    break;
  default:
    VDAC = 0;
    IDAC = 0;
    break;
  }

  Value = VDAC & 0xf;
  Value |= (IDAC & 0x7) << 4;
  Value |= (IRef & 0x3) << 9;

  writeRegister(chipID, ALPIDE_REG_ANALOGMON, Value);
  return;
}


/* ------------------------------------------------------
 * Set the ADC Control Register
 *
 * Parameter  : Mode of ADC measurement [0:Manual 1:Calibrate 2:Auto 3:SupoerManual]
 *              SelectInput the source specification [0:AVSS 1:DVSS 2:AVDD 3:DVDD
 *                                                    4:VBGthVolScal 5:DACMONV 6:DACMONI
 *                                                    7:Bandgap 8:Temperature]
 *              ComparatorCurrent  [0:180uA 1:190uA 2:296uA 3:410uA]
 *              RampSpeed          [0:500ms 1:1us 2:2us 3:4us]
 *
 */
uint16_t ALPIDE::setTheADCCtrlRegister( uint8_t chipID,
                                        TADCMode Mode,
                                        TADCInput SelectInput,
                                        TADCComparator ComparatorCurrent,
                                        TADCRampSpeed RampSpeed)
{
  uint16_t Data;
  Data = Mode | (SelectInput<<2) | (ComparatorCurrent<<6) | (fADCSign<<8) | (RampSpeed<<9) | (fADCHalfLSB<<11);
  writeRegister( chipID, ALPIDE_REG_ADC_CONTROL, Data);
  return(Data);
}

int ALPIDE::sendCommand(uint8_t chipID,uint16_t cmd)
{
  return writeRegister( chipID, ALPIDE_REG_COMMAND,cmd);
}


float ALPIDE::readTemp(uint8_t chipID) {

  uint16_t theResult = 0;
  float theValue;
  calibrateADC(chipID);
/*
  if(fADCOffset == -1) { // needs calibration
    CalibrateADC();
  }
*/
  setTheDacMonitor(chipID,ALPIDE_REG_ANALOGMON); // uses the RE_ANALOGMON, in order to disable the monitoring !
  delayMicroseconds(5000);
  //usleep(5000);
  setTheADCCtrlRegister(chipID,MODE_MANUAL, INP_Temperature, COMP_296uA, RAMP_1us);

  sendCommand(chipID,ALPIDE_CMD_ADCMEASURE);
/*  fReadoutBoard->SendCommand ( Alpide::COMMAND_ADCMEASURE,  this);
  usleep(5000); // Wait for the measurement > of 5 milli sec
*/
  delayMicroseconds(5000);

  theResult=readRegister(chipID,ALPIDE_REG_ADC_AVSS);
  //ReadRegister( Alpide::REG_ADC_AVSS, theResult);
  
  //theResult -=  (uint16_t)fADCOffset;
  theValue =  ( ((float)theResult) * 0.1281) + 6.8; // first approximation
  
  return(theValue);
}




/* ------------------------------------------------------
 * Reads the output voltage of one DAC by means of internal ADC
 *
 * Parameter : the Index that define the DAC register
 *
 * Returns  : the value in Volts.
 *
 * Note  : if this was the first measure after the chip
 *         configuration phase, a calibration will be
 *         automatically executed.
 *
 *   13/6/17 - Returns negative values (A.Franco)
 *
 */
float ALPIDE::readDACVoltage(uint8_t chipID,uint16_t ADacReg) {

  uint16_t theRowValue = 0;
  int theResult = 0;
  float theValue = 0.0;
  calibrateADC(chipID);
  /*
  if(fADCOffset == -1) { // needs calibration
    CalibrateADC();
  }*/

  setTheDacMonitor(chipID,ADacReg);
  delay(4);
  
  setTheADCCtrlRegister(chipID,MODE_MANUAL, INP_DACMONV, COMP_296uA, RAMP_1us);

  sendCommand(chipID,ALPIDE_CMD_ADCMEASURE);
  //fReadoutBoard->SendCommand ( Alpide::COMMAND_ADCMEASURE,  this);
  //usleep(5000); // Wait for the measurement > of 5 milli sec
  //ReadRegister( Alpide::REG_ADC_AVSS, theRowValue);
  delay(4);
  theRowValue=readRegister(chipID,ALPIDE_REG_ADC_AVSS);
  //theResult = ((int)theRowValue) - fADCOffset;
  
  theValue =  ( ((float)theResult) * 0.001644); // V scale first approximation
  
  return(theValue);
}


/* ------------------------------------------------------
 * Reads the output current of one DAC by means of internal ADC
 *
 * Parameter : the Index that define the DAC register
 *
 * Returns  : the value in Micro Ampere.
 *
 * Note  : if this was the first measure after the chip
 *         configuration phase, a calibration will be
 *         automatically executed.
 *
 *   13/6/17 - Returns negative values (A.Franco)
 *
 */
float ALPIDE::readDACCurrent(uint8_t chipID,uint16_t ADacReg) 
{  
  return(readDACVoltage(chipID,ADacReg)*100); // scale to uA
}



/* ------------------------------------------------------
 * Calibrate the internal ADC
 *
 * Returns  : the value of the calculated Bias.
 *
 * Note  : the calibration parameter are stored into the
 *         devoted class members.
 *
 */
int ALPIDE::calibrateADC(uint8_t chipID)
{
  uint16_t theVal2,theVal1;
  float fADCOffset = 0;
  // Calibration Phase 1
  fADCHalfLSB = false;
  fADCSign = false;
  setTheADCCtrlRegister(chipID,MODE_CALIBRATE , INP_AVSS, COMP_296uA, RAMP_1us);
  sendCommand(chipID,ALPIDE_CMD_ADCMEASURE);
  delay(4);
 
  theVal1=readRegister(chipID,ALPIDE_REG_ADC_CALIB);
  fADCSign = true;
  sendCommand(chipID,ALPIDE_CMD_ADCMEASURE);
  theVal2=readRegister(chipID,ALPIDE_REG_ADC_CALIB);
  fADCSign =  (theVal1 > theVal2) ? false : true;

  // Calibration Phase 2
  fADCHalfLSB = false;
  
  setTheADCCtrlRegister(chipID,MODE_CALIBRATE , INP_Bandgap, COMP_296uA, RAMP_1us);
  sendCommand(chipID,ALPIDE_CMD_ADCMEASURE);
  delay(4);
  theVal1=readRegister(chipID,ALPIDE_REG_ADC_CALIB);
  fADCHalfLSB = true;
  setTheADCCtrlRegister(chipID,MODE_CALIBRATE , INP_Bandgap, COMP_296uA, RAMP_1us);
  sendCommand(chipID,ALPIDE_CMD_ADCMEASURE);
  delay(4);
  theVal2=readRegister(chipID,ALPIDE_REG_ADC_CALIB);
  fADCHalfLSB =  (theVal1 > theVal2) ? false : true;



   // Offset Measurement
  fADCOffset = 0;
  unsigned int n_samples = 20;
  for (unsigned int i = 0; i<n_samples; ++i) {
    setTheADCCtrlRegister(chipID,MODE_CALIBRATE , INP_AVSS, COMP_296uA, RAMP_1us);
    sendCommand(chipID,ALPIDE_CMD_ADCMEASURE);
    delay(4);    
    theVal1=readRegister(chipID,ALPIDE_REG_ADC_CALIB);
    fADCOffset += theVal1;
  }
  fADCOffset = static_cast<int>(static_cast<double>(fADCOffset)/static_cast<double>(n_samples)+0.5);
 
  return(fADCOffset);
}
