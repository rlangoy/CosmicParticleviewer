## Usage
This folder contains the files needed to insert data into InfluxDB .

## InfluxDB Installation
Complete install instructions is available from the influxdata  [install page](https://docs.influxdata.com/influxdb/v2.5/install/) \
Lastest software could be downloaded from the influxdata's github [release page ](https://github.com/influxdata/influxdb/releases) 

## Insert APIDE Data into the page.
Edit the file insertToDb.py and replase token/org/bucket and update the serialPort
```
token = "Ok7Hy44aCG_1DJGyo0DciVqX3psxGnEDM8FUMVoxS6BPo7zKF82VVQFgQTSP5prj-jfvCvDfNClQhMDMQCRRgw=="
org = "USN"
bucket = "AlpideData"   #"AlpideData"

serialPort='/dev/ttyACM0'  #'COM95
```
