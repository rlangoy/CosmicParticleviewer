from picoif import OuterBarriel

serialPort='COM95'  #'COM95
print ("Open Serial port ",serialPort)
alpide=OuterBarriel(serialPort)  

#print('Reset All ALPIDEs')
#alpide.sendReset()

#ALPIDE_REG_VTEMP=0x0609
#print('Write to 0x1234 a  ALPIDE 0x70 Resister ALPIDE_REG_VTEMP')

#alpide.writeRegister(0x70,ALPIDE_REG_VTEMP,0x1234)
#print("ALPIDE_REG_VTEMP - Read: 0x%X"% alpide.readRegister(0x70,ALPIDE_REG_VTEMP))


print('send Trigger & Reset signal to all ALPIDEs')
alpide.sendTrigger()

alpide.plotAllChips()