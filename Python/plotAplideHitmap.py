from picoif import OuterBarriel

serialPort='COM4'  #'COM95
print ("Open Serial port ",serialPort)
alpide=OuterBarriel(serialPort)  

print('Reset All ALPIDEs')
alpide.sendReset()

ALPIDE_REG_VTEMP=0x0609
print('Write to 0x1234 a  ALPIDE 0x70 Resister ALPIDE_REG_VTEMP')

alpide.writeRegister(0x70,ALPIDE_REG_VTEMP,0x1234)
print("VCASN2 - Read: 0x%X"% myAlpides.readRegister(0x70,VCASN2))

print('send Trigger signal to all ALPIDEs')
alpide.sendTrigger()

alpide.plotAllChips()