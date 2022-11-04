from picoif import OuterBarriel

serialPort= 'COM6' #'COM95'  #'COM6'  #'COM95
print ("Open Serial port ",serialPort)
alpide=OuterBarriel(serialPort)  

#print('Reset All ALPIDEs')
alpide.sendReset()


vcasn  = 0x0604
vcasn2 = 0x0607
ithr   = 0x060E

# for chipID in idList:
#chipID=0x79
chipID=0x79 ##Chip wery photo sensitive
print("Init vcasn/vcasn2/ithr ALPIDE 0x%0x"%chipID)
alpide.writeRegister(chipID=chipID,addr=vcasn,data=0x39-19)    # inc -> More Noise 
alpide.writeRegister(chipID=chipID,addr=vcasn2,data=0x40-14)   ##Strek vises...
alpide.writeRegister(chipID=chipID,addr=ithr,data=1)     #inc reduces sensitivity

#ALPIDE_REG_VTEMP=0x0609
#print('Write to 0x12 a  ALPIDE 0x70 Resister ALPIDE_REG_VTEMP')

#alpide.writeRegister(0x70,ALPIDE_REG_VTEMP,0x12)
#print("ALPIDE_REG_VTEMP - Read: 0x%X"% alpide.readRegister(0x70,ALPIDE_REG_VTEMP))


print('send Trigger & Reset signal to all ALPIDEs')
alpide.sendTrigger()

#alpide.plotAllChips()
alpide.plotChipPixels(0x79,True)