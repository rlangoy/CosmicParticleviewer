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

#ifndef ALPIDE_H
#define ALPIDE_H

#include <Arduino.h>
#include <stdint.h>



/*Header J2 is mirrored Pin 1 3 4 5 6 swaped with 7 8 9 10 11 12 */
/* Outer Bariell Muon Detector Borad PIN definitions */

/*
// Power supply enable singals 
#define AVDD_EN         16    // Enable Alpide 1.8V Analog  Power
#define DVDD_EN         15    // Enable Alpide 1.8V Digital Power

//  Master Clock                                       HW-Enabled (Receiver enable  RE-SN65MLVD20xx)
#define OBMDB_MCLK_O    14   // MCU-Output : Clock Output PIN     (Driver   input    D-SN65MLVD20xx)
#define OBMDB_MCLK_I    13   // MCU-Input  : Clock Input  PIN     (Reciever out      R-SN65MLVD20xx)
#define OBMDB_MCLK_OE   12   // MCU-Output : Clock Derectioin PIN (Driver enable    DE-SN65MLVD20xx) 

//  Data Control                                         HW-Enabled (Receiver enable  RE-SN65MLVD20xx)
#define OBMDB_DCTRL_O    17   // MCU-Output : Data - Output PIN     (Driver   input    D-SN65MLVD20xx)
#define OBMDB_DCTRL_I    18   // MCU-Input  : Data - Input  PIN     (Reciever out      R-SN65MLVD20xx)
#define OBMDB_DCTRL_OE   19   // MCU-Output : Data - Derectioin PIN (Driver enable    DE-SN65MLVD20xx) 
*/



//-- 4x Borad's Master-Clock is common 
//  Master Clock                                       HW-Enabled (Receiver enable  RE-SN65MLVD20xx)
#define OBMDB_MCLK_O    16   // MCU-Output : Clock Output PIN     (Driver   input    D-SN65MLVD20xx)
//#define OBMDB_MCLK_I    13   // MCU-Input  : Clock Input  PIN     (Reciever out      R-SN65MLVD20xx)
 // Not in use
#define OBMDB_MCLK_OE   17   // MCU-Output : Clock Derectioin PIN (Driver enable    DE-SN65MLVD20xx) 
// -- Data Control Output is Common
#define OBMDB_DCTRL_OE   14   // MCU-Output : Data - Derectioin PIN (Driver enable    DE-SN65MLVD20xx) 


//connector J0
// Power supply enable singals 
#define AVDD_EN         21    // Enable Alpide 1.8V Analog  Power
#define DVDD_EN         2    // Enable Alpide 1.8V Digital Power

//  Data Control                                         HW-Enabled (Receiver enable  RE-SN65MLVD20xx)
#define OBMDB_DCTRL_O    6   // MCU-Output : Data - Output PIN     (Driver   input    D-SN65MLVD20xx)
#define OBMDB_DCTRL_I    10   // MCU-Input  : Data - Input  PIN     (Reciever out      R-SN65MLVD20xx)



//connector J1
// Power supply enable singals 
/*
#define AVDD_EN         20    // Enable Alpide 1.8V Analog  Power
#define DVDD_EN         3    // Enable Alpide 1.8V Digital Power

//  Data Control                                         HW-Enabled (Receiver enable  RE-SN65MLVD20xx)
#define OBMDB_DCTRL_O    7   // MCU-Output : Data - Output PIN     (Driver   input    D-SN65MLVD20xx)
#define OBMDB_DCTRL_I    11   // MCU-Input  : Data - Input  PIN     (Reciever out      R-SN65MLVD20xx)
*/


//connector J2
// Power supply enable singals 
/*
#define AVDD_EN         19    // Enable Alpide 1.8V Analog  Power
#define DVDD_EN         4    // Enable Alpide 1.8V Digital Power

//  Data Control                                         HW-Enabled (Receiver enable  RE-SN65MLVD20xx)
#define OBMDB_DCTRL_O    8   // MCU-Output : Data - Output PIN     (Driver   input    D-SN65MLVD20xx)
#define OBMDB_DCTRL_I    12   // MCU-Input  : Data - Input  PIN     (Reciever out      R-SN65MLVD20xx)
*/


//connector J3
// Power supply enable singals 
/*
#define AVDD_EN         18    // Enable Alpide 1.8V Analog  Power
#define DVDD_EN         5    // Enable Alpide 1.8V Digital Power

//  Data Control                                         HW-Enabled (Receiver enable  RE-SN65MLVD20xx)
#define OBMDB_DCTRL_O    9   // MCU-Output : Data - Output PIN     (Driver   input    D-SN65MLVD20xx)
#define OBMDB_DCTRL_I    13   // MCU-Input  : Data - Input  PIN     (Reciever out      R-SN65MLVD20xx)
*/


#define TRIGGER_STORBE_LENGTHT  65000 //  Old 6000

// Pinout
// ALPIDE     AVR (Arduino)
// ------     -------------
// PWR_EN     PB1 (7)
// RST_N      PD7 (9) 
// DCLK       A1 (15)
// CTRL       PB0 (8)
// 1-wire     PD5 (12)

#define ALPIDE_PIN_PWR_EN  7   //<-Not used
#define ALPIDE_PIN_RST_N   9   //<-Not used

#define ALPIDE_PIN_CLK    15   // A1

#define ALPIDE_REG_COMMAND       0x0000
#define ALPIDE_REG_MODE_CTRL     0x0001
#define ALPIDE_REG_FROMU_CFG1    0x0004
#define ALPIDE_REG_FROMU_CFG2    0x0005
#define ALPIDE_REG_CMUDMU_CFG    0x0010
#define ALPIDE_REG_DMU_FIFO_LO   0x0012
#define ALPIDE_REG_DMU_FIFO_HI   0x0013
#define ALPIDE_REG_ANALOGMON     0x0600
#define ALPIDE_REG_DAC_VRESETP   0x0601
#define ALPIDE_REG_DAC_VRESETD   0x0602
#define ALPIDE_REG_VCASP         0x0603
#define ALPIDE_REG_DAC_VCASN     0x0604
#define ALPIDE_REG_DAC_VPULSEH   0x0605
#define ALPIDE_REG_DAC_VPULSEL   0x0606
#define ALPIDE_REG_DAC_VCASN2    0x0607
#define ALPIDE_REG_VCLIP         0x0608
#define ALPIDE_REG_VTEMP         0x0609
#define ALPIDE_REG_IAUX2         0x060A
#define ALPIDE_REG_IRESET        0x060B
#define ALPIDE_REG_IDB           0x060C
#define ALPIDE_REG_IBIAS         0x060D
#define ALPIDE_REG_DAC_ITHR      0x060E
#define ALPIDE_REG_ADC_CONTROL   0x0610
#define ALPIDE_REG_ADC_CALIB     0x0612
#define ALPIDE_REG_ADC_AVSS      0x0613


#define APIDE_DEFAULT_VRESETD    147 /* TODO: value in mV?*/
//#define APIDE_DEFAULT_VRESETD    200 /* TODO: mV?*/

#define ALPIDE_CMD_TRIGGER       0x0055
#define ALPIDE_CMD_GRST          0x00D2
#define ALPIDE_CMD_PRST          0x00E4
#define ALPIDE_CMD_PULSE         0x0078
#define ALPIDE_CMD_BCRST         0x0036
#define ALPIDE_CMD_RORST         0x0063
#define ALPIDE_CMD_DEBUG         0x00AA
#define ALPIDE_CMD_WROP          0x009C
#define ALPIDE_CMD_RDOP          0x004E
#define ALPIDE_CMD_CMU_CLEAR_ERR 0xFF00
#define ALPIDE_CMD_FIFOTEST      0xFF01
#define ALPIDE_CMD_LOADOBDEFCFG  0xFF02
#define ALPIDE_CMD_XOFF          0xFF10
#define ALPIDE_CMD_XON           0xFF11
#define ALPIDE_CMD_ADCMEASURE    0xFF20

#define ALPIDE_DATA_CHIP_HEADER   0xA0
#define ALPIDE_DATA_CHIP_TRAILER  0xB0
#define ALPIDE_DATA_CHIP_EMPTY    0xE0
#define ALPIDE_DATA_REGION_HEADER 0xC0
#define ALPIDE_DATA_DATA_SHORT    0x40
#define ALPIDE_DATA_BUSY_ON       0xF1
#define ALPIDE_DATA_BUSY_OFF      0xF0

typedef enum {
  IREF_025uA = 0,
  IREF_075uA = 1,
  IREF_100uA = 2,
  IREF_125uA = 3
  } TDACMonIref;

typedef enum {
  MODE_MANUAL = 0,
  MODE_CALIBRATE = 1,
  MODO_AUTO = 2,
  MODE_SUPERMANUAL = 3
  } TADCMode;

typedef enum {
  INP_AVSS = 0,
  INP_DVSS = 1,
  INP_AVDD = 2,
  INP_DVDD = 3,
  INP_VBGthVolScal = 4,
  INP_DACMONV = 5,
  INP_DACMONI = 6,
  INP_Bandgap = 7,
  INP_Temperature = 8
  } TADCInput;

typedef enum {
  COMP_180uA = 0,
  COMP_190uA = 1,
  COMP_296uA = 2,
  COMP_410uA = 3
  } TADCComparator;


typedef enum {
    RAMP_500ms = 0,
    RAMP_1us = 1,
    RAMP_2us = 2,
    RAMP_4us = 3
  } TADCRampSpeed;

#define MaxMaskPix 1000  //5K storage..
struct pxStr{
    uint8_t chipID;
    uint16_t x;
    uint16_t y;
};

//


class ALPIDE {
public:
  ALPIDE();
  void        power(bool on=true);
  int         init();
  //int         trigger(int strobeMilliSeconds=0);
  void        sendTrigger();
   void       sendReset();
  int         readRawData(uint8_t plane,uint16_t *buf,int n);
  static int  decodeRawDataWord(uint16_t word,int8_t *currentRegion,uint16_t *x,uint16_t *y,uint8_t *bc,int8_t *plane,uint8_t *flags);
  int         readEvent(uint8_t plane,uint16_t *x,uint16_t *y,int n);
  int         readPixel(uint8_t chipID,uint16_t *x,uint16_t *y ,uint16_t maxEventReturn=100);
  int         readEventWords(uint8_t chipID,uint32_t *wDataTable ,uint16_t maxEventReturn=100);
  int         decodeEventWord(uint32_t wDataTable,uint16_t *x,uint16_t *y);
  void        dumpEventWordsPy(HardwareSerial &SerialInOut,uint8_t chipID,uint16_t maxEventReturn);
  void        dumpEventWords24Bit(HardwareSerial &SerialInOut,uint8_t chipID,uint16_t maxEventReturn);
  float       readTemp(uint8_t chipID);                         // read ALPIDE internal temperature
  float       readDACVoltage(uint8_t chipID,uint16_t ADacReg);  // read ADC voltage
  float       readDACCurrent(uint8_t chipID,uint16_t ADacReg);  // read ACD current in uA
//protected:
  void    initPixReadOut        ();
  int     broadcastCommand      (uint8_t cmd);
  int     sendCommand           (uint8_t chipID,uint16_t cmd);
  int     broadcastWriteRegister(uint16_t addr,uint16_t data);
  int     writeRegister         (uint8_t chipID,uint16_t addr,uint16_t data);
  int32_t readRegister          (uint8_t chipID,uint16_t addr);
  int32_t debugReadRegister     (uint8_t chipID,uint16_t addr);
  int32_t readFifo              (uint8_t chipID);
  void    maskAllPixels         (uint8_t chipID,bool enable);
  void    enablePulserAllPixels (uint8_t chipID,bool enable);
  void    maskPxls(); 
  void    addMaskPixel          (uint8_t chipID,uint16_t x,uint16_t y);         
  void    maskPixel             (uint8_t chipID,uint16_t x,uint16_t y,bool enable);
  void    enablePulserPixel     (uint8_t chipID,uint16_t x,uint16_t y,bool enable);
//private:
  
  bool     mWaitForChipHeader=true; 
  void     clk(int n=1);
  void     clk_high();
  void     clk_low();
  int      checkIdle(int n=1);
  void     writeByte(uint8_t byte);
  int16_t  readByte();
  void     write_ctrl(bool bHigh);   // wirite data to the differntial CTRL
  bool     read_ctrl();              // reads data from the differntial CTRL (only CRTL_P..)
  void     setTheDacMonitor(uint8_t chipID,uint16_t ADacReg, TDACMonIref IRef=IREF_100uA);   //Configures the ADC MUX
  bool     fADCHalfLSB;
  bool     fADCSign;
  uint16_t setTheADCCtrlRegister( uint8_t chipID,
                                        TADCMode Mode,
                                        TADCInput SelectInput,
                                        TADCComparator ComparatorCurrent,
                                        TADCRampSpeed RampSpeed);
  int      calibrateADC(uint8_t chipID); 
  int      pixMasked=0;                                       
  struct pxStr pixMaskList[MaxMaskPix];
};

#endif
