from picoif import OuterBarriel
from datetime import datetime

from influxdb_client import InfluxDBClient, Point, WritePrecision, WriteOptions   #pip install influxdb-client
from influxdb_client.client.write_api import SYNCHRONOUS

# You can generate an API token from the "API Tokens Tab" in the UI
token = "Ok7Hy44aCG_1DJGyo0DciVqX3psxGnEDM8FUMVoxS6BPo7zKF82VVQFgQTSP5prj-jfvCvDfNClQhMDMQCRRgw=="
org = "USN"
bucket = "AlpideData"   #"AlpideData"

serialPort='/dev/ttyACM0'  #'COM95
print ("Open Serial port ",serialPort)
alpide=OuterBarriel(serialPort)  

seq =0
idList=alpide.getChipIDList()

#
#"vcasn":50,
#  "vcasn2":62,
#  "ithr":51,

alpide.sendReset()

vcasn  = 0x0604
vcasn2 = 0x0607
ithr   = 0x060E

for chipID in idList:
  print("Init vcasn/vcasn2/ithr ALPIDE 0x%0x"%chipID)
  alpide.writeRegister(chipID=chipID,addr=vcasn,data=50) 
  alpide.writeRegister(chipID=chipID,addr=vcasn2,data=62)
  alpide.writeRegister(chipID=chipID,addr=ithr,data=51)


print("Create Noisy PixMap")
alpide.createAllPixelMask()

while(True) :
  print('send Trigger  signal to all ALPIDEs')
  alpide.sendTrigger()
  
    
  with InfluxDBClient(url="http://127.0.0.1:8086", token=token, org=org,timeout=30_000) as client:
    write_api = client.write_api(write_options=SYNCHRONOUS)    
    for AlpideID in idList :
      print (f"Reading id 0x{AlpideID:02X}")  
      lst=alpide.getNewHits(chipID=AlpideID)
      if(len(lst) >0 ) :
          indx=0
          indy=0
          if ((AlpideID >=0x70) & (AlpideID<=0x76)) :
            indx= AlpideID-0x70
            indy=0
          else :
            indx= AlpideID-0x78
            indy=1
          print (f"   - Storing id 0x{AlpideID:02X}")
          data = []
          for x,y in lst :   #calc and store global x,y
              xGlobal=x+(indx*1024)
              yGlobal=y+(indy*512)
              point = Point("Horten") \
                      .tag(f"chipID={AlpideID}", f"seq={seq}") \
                      .field("X",xGlobal ) \
                      .field("Y",yGlobal ) \
                      .time(datetime.utcnow(), WritePrecision.NS)

              data.append(point)
                #insertString =f"Horten,chipID={AlpideID},seq={seq} X={x+(indx*1024)},Y={y+(indy*512)}" 
                #write_api.write(bucket, org, insertString)
          write_api.write(bucket, org, data)
          #write_api.write_points(data, database='AlpideData', time_precision='ms', batch_size=len(lst), protocol='line')            
          #insertString =f"Horten,chipID={AlpideID},seq={seq} cnt={len(lst)}"
          #write_api.write(bucket, org, insertString)
  seq +=1  
write_api.flush()
client.close()