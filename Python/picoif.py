# #!/usr/bin/env python3
# 
# import alpideseq
from time import sleep
import matplotlib.pyplot as plt
import serial
import serial.tools.list_ports
import numpy as np
import json

class OuterBarriel():
    
        # constructor
    def __init__(self,serialPortName):
     
        comlist = serial.tools.list_ports.comports()
        connected = []
        for element in comlist:
            connected.append(element.device)
        #print("Available COM ports: " + str(connected))

        ser = serial.Serial(serialPortName )  # open serial port
        self.ser=ser        
        self.obMaskList={}
    
    
    def readevent(alpide,chipid):
        words=[]
        while True:
            lo=alpide.read_reg(chipid,0x0012)
            hi=alpide.read_reg(chipid,0x0013)
            word=(hi<<16|lo)&0xFFFFFF
            words.append(word)
            if (word>>16)&0xF0==0xB0: break
            if (word>>16)&0xF0==0xE0: break
            if word==0xFFFFFF: break
        return words

    # Function decodeevent()
    # 
    #  param words= 24bits unsigned ints (Pixel data)
    #  param decode - Sellect pixel-info to decode ( only ment for debuging ) 
    #   decode=0 - Dont decode any pixels :)
    #   decode=1 - Decode only long-data
    #   decode=2 - Decode ony  short-data
    #   decode=3   Decode long- & short-data
    def decodeevent(this,words,decode=3):
        assert len(words)>0
        reg=None
        hits=[]
        if   words[0]&0xF000FF==0xE000FF: # chip empty
            pass
        elif words[0]&0xF000FF==0xA000FF: # chip header
            for word in words[1:-1]:
                if   word&0xC00000==0x000000: # data long
                    #print("Pix fmt long 0x%X"%word)
                    if(decode & 1) :
                        assert reg is not None
                        addr=reg<<14|word>>8&0x3FFF
                        hits.append(addr)
                        hm=word&0xFF
                        while hm:
                            addr+=1
                            if hm&1: hits.append(addr)
                            hm>>=1
                elif word&0xC000FF==0x4000FF: # data short                
                    #print("Pix fmt short 0x%X"%word)
                    if(decode&2):
                        assert reg is not None
                        hits.append(reg<<14|word>>8&0x3FFF)
                elif word&0xE0FFFF==0xC0FFFF: # region header
                    reg=word>>16&0x1F
                    #print("Region ID %06X  Region-Value %d"% (word,reg))
                else: raise ValueError('bad word: %06X'%word)
            if words[-1]&0xF000FF==0xB000FF: # chip trailer
                pass
            else: print('bad last word: %06X'%words[-1])#raise ValueError('bad last word: %06X'%words[-1])
        else: raise ValueError('bad first word: %06X'%words[0])

        xy=[(d>>9&0x3FE|(d^d>>1)&0x1,d>>1&0x1FF) for d in hits]
        return xy

#     def trgrdo(this,alpide):
#         alpide.global_cmd(0xD2) # GRST
#         alpide.global_cmd(0xE4) # PRST
#         alpide.write_reg(0x0F,0x0010,0x007F) # disable manchester, initial token
#         alpide.write_reg(0x0F,0x0004,0x0000) # disable BUSY monitoring
#         alpide.write_reg(0x0F,0x0001,0x020D) # readout via CMU
#         alpide.write_reg(0x0F,0x0005,60000) # strobe length
#         alpide.write_reg(0x0F,0x0602,147) # VRESETD
#         for i in range(1000):
#             alpide.global_cmd(0x55) # TRIGGER
#             #for chipid in [0x78,0x79,0x7A,0x7B,0x7C,0x7D,0x7E]:
#             for chipid in [0x10]:
#                 hits=decodeevent(readevent(alpide,chipid))
#                 for x,y in hits:
#                     print(chipid,x,y)

    # Get list from memeory struct
    #  Debug Funn :)
    def getPixelsFromMem(this):
        # input Data Stream stream of 24 bits to reduce Tx time
        # 0xA093FF  - header     ( start pixel data)
        # 0xC0FFFF  - Region-id  ( X pos Ofset )
        # 0x42BDFF  - data short ( px fmt single   pixel  )
        # 0x030128  - data long  ( px fmt multiple pixels )    
        # 0xB0FFFF  - trailer    ( end of pixel data)
                     # Header             Region-id           data - short     data - long       trailer
                     #0xA093FF            #0xC0FFF            0x42BDFF         #0x030128         0xB0FFFF
        data = bytes([0xA0, 0x93,0xFF,   0xC0, 0xFF,0xFF , 0x42,0xBD, 0xFF,  0x03,0x01,0x28,    0xB0,0xFF,0xFF ])

        #Send data as 24bits to reduce time.. (PC-faster than ucontroller :)
        lst=[]
        # Splitt in 24 bits
        for i in range(0,len(data),3) :
            lst.append(int.from_bytes(data[i:i+3], byteorder='big', signed=False))        

        return lst

    #   Return data JSON Formated information
    def getQueryGetPixelFrameData(this,chipID):
        sId = ("%s" % chipID).encode()
        return b'{"getPixelFrameData": [{"chipID":' +sId+ b'}]} \r\n'    

    #   Return data JSON Formated information
    #      example output: {"readRegister": { "chipID": 112, "addr": 609   } }
    def getQueryReadRegister(this,chipID,addr):
        sId   = ("%s" % chipID).encode()
        sAddr = ("%s" % addr).encode()
        return   b'{"readRegister": {"chipID":'+sId +b',"addr":'+sAddr+b' } } \r\n' ;

    def sendQueryReadRegister(this,chipID,addr):
        this.ser.write(this.getQueryReadRegister(chipID,addr))
        
        #return '{ "alpideRegister": [ { "addr": 609 , " value": 35}  ] }'
        return  this.ser.readline() 

    #   Return data JSON Formated information
    #      example output:  {"writeRegister": {     "chipID": 112,   "addr": 1545  ,"data" : 100  } }
    def getQueryWriteRegister(this,chipID,addr,data):
        sId   = ("%s" % chipID).encode()
        sAddr = ("%s" % addr).encode()
        sData = ("%s" % data).encode()
        
        return   b'{"writeRegister": {"chipID":'+sId +b',"addr":'+sAddr+b',"data":'+sData +b'} } \r\n' ;

    def sendQueryWriteRegister(this,chipID,addr,data):
        this.ser.write(this.getQueryWriteRegister(chipID,addr,data))
        ok=this.ser.readline()
       

    def sendTrigger(this):
        this.ser.write(b' {"sendTrigger": "True" }\r\n')
        ok=this.ser.readline()
     
    def sendReset(this):
        this.ser.write(b' {"sendRest": "True" }\r\n')
        ok=this.ser.readline()


    def sendMaskPixels(this,chipID,lstPixels) :     
        #lst=[(39, 490), (70, 258), (75, 254), (512, 439), (549, 64), (817, 503), (1007, 458)]
        #str='{"maskPixels": ['
        #for x,y in lst :
        #    print(x,y)
        #    
        strMask= b'{"maskPixels": [{"chipID": 112, "x": 1 ,"y" : 350  },{"chipID": 112, "x": 1 ,"y" : 384  },{"chipID": 112, "x": 1 ,"y" : 386  },{"chipID": 112, "x": 0 ,"y" : 387  }]    } \r\n'
        this.ser.write(strMask)


    # Get list from memeory struct via the serial port
    #  Debug Funn :)
    def getPixelsFromSerialJsonCmd(this,chipID=0x70):
        #print("Connected to ", ser.name)         # check which port was really used

        #Send command to Get pix values
        #ser.write(b'{"getPixelFrameData": "debugData" }\r\n')
        #ser.write(b' {"sendTrigger": "True" }\r\n')
        # 0x70 = 112
        
        #ser.write(b'{"getPixelFrameData": [{"chipID": 114}]  }\r\n')
        this.ser.flushInput()
        this.ser.flushOutput()
        this.ser.write(this.getQueryGetPixelFrameData(chipID))
        
        lst=[]
        #Read serial data until stop was found
        contLoop=True;
        cnt=0
        cnt1000=0;
        while contLoop:
            b24Word = this.ser.read(3) #Read 3 bytes
            word    = int.from_bytes(b24Word, byteorder='big', signed=False)
            lst.append(word)
            cnt +=1
            if(cnt>1000):
                cnt=0
                cnt1000 +=1
                if(cnt1000==1):
                    print("ID 0x%X cnt %d "%(chipID,cnt1000))
                else :
                    print("cnt ",cnt1000)
            
            if((word & 0xB00000) ==0xB00000 ) :    #Break loop when Trailer(end-token) was found
                #lst[cnt-1]=0xB0FFFF
                contLoop=False
        return lst;

    def getPixelsFromCSV(this):
        pixFrm = np.loadtxt("trgData.csv", delimiter=",")
        #pixFrm = np.loadtxt("data2.csv", delimiter=",")
        return pixFrm.astype(int).tolist()

    def readRegister(this,chipID,addr):           
         # No need to check reply address.. 
         return json.loads(this.sendQueryReadRegister(chipID,addr))["alpideRegister"][0]["data"]

    def writeRegister(this,chipID,addr,data):           
         # No need to check reply address.. 
         this.sendQueryWriteRegister(chipID,addr,data)

    def plotChipPixels(this,chipID,newFig=False):
            lst=[]
            lst=this.getPixelsFromSerialJsonCmd(chipID=chipID) #read data from given alpide
            #lst=this.getPixelsFromMem()   #debug read px-data from memory
            #lst=this.getPixelsFromCSV()   #debug read px-dayt from file

            if (lst[0]==0xFFFFFF) :
                print("0xFFFFFF : ChipID 0x%X"%chipID +" Not Rdy..")        
                
            else: 
                hitMap = this.decodeevent(lst,3)     # Decode to pixel points  [(x0,y0), (x1,y1) ,... ]
                #plt.figure(cnt+1)
                figName="Himap from all Aplides combined"
                if (newFig) :
                    figName=("Aplide id %X"%chipID)                    
                
                plt.figure(figName)
                
                npXYArray=np.array(hitMap)    # Points(x,y) to 2d array [ [x0,y0] , [x1,y1],[x2,y2] ] ]
                plt.scatter(npXYArray[:,0], npXYArray[:,1],s=1)  #Plot Data
                plt.title(figName)
                plt.draw()
                #cnt +=1
                plt.pause(0.01)           
           # plt.show(break=False)

    def getHitmap(this,chipID) :    
        lst=this.getPixelsFromSerialJsonCmd(chipID)
        #lst=this.getPixelsFromMem()   #debug read px-data from memory
        return this.decodeevent(lst,3)


    def plotChips(this,alpideIdList):
        for chipID in alpideIdList :
            print("Plotting pixel hists in chipID 0x%X"%chipID)
            this.plotChipPixels(chipID,newFig=False)
            
        print("Finished Plotting pixels")
        plt.show()
        
    def getChipIDList(this) :
        return [0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x78,0x79,0x7A,0x7B,0x7C,0x7D,0x7E]

    # Plots all the chips in the Outer Brarriel  Module
    def plotAllChips(this):
        
        this.plotChips(this.getChipIDList())
    
    #
    # Run this twice to get correct mask
    def updatePixelMask(this,chipID,lstPixels):
        
        if( len(this.obMaskList)== 0) : # If list not created create it
            for alpideID in this.getChipIDList() :
                this.obMaskList.update({alpideID:[]})
            this.obMaskList[chipID]=lstPixels        # Insert list and return
            return
        
        #  if not two equals in lists remove it
        for item in this.obMaskList[chipID]:
            if item not in lstPixels:
              (this.obMaskList[chipID]).remove(item)

        return
    
    def createPixelMask(this,chipID) :
        print('send Trigger & Reset signal to all ALPIDEs')
        this.sendTrigger()
        lst=this.getHitmap(chipID=chipID)
        this.updatePixelMask(chipID,lst)

        print('send Trigger & Reset signal to all ALPIDEs')
        this.sendTrigger()
        lst=this.getHitmap(chipID=chipID) 
        this.updatePixelMask(chipID,lst)

    def getNewHits(this,chipID) :
         if( len(this.obMaskList)== 0) : # If list not created create it
            this.createPixelMask(chipID)
         
         this.sendTrigger()
         lstPixels=this.getHitmap(chipID=chipID) 

         for item in this.obMaskList[chipID]:
            if item in lstPixels:
              lstPixels.remove(item)
        
         return lstPixels
     
    