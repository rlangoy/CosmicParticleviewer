# Kicad Files for Raspberry Pico (RP2040) interface to Outer Barrel Module
## PCB Layout. 
 ![PCB Layout](images/PCB_Overview.png?raw=true "PCB Layout")
\
The Mounting holes is designed for 4M Screws \

## RP2040 Pinout 
![PCB Layout](images/Pico_Pinout.png?raw=true "Pico Pinout")
## +3,3V Source Selection
![PCB Layout](images/3v3_Selection.png?raw=true "+3.3V Souce Selection")

## Fabrication
The PCB can be fabricated by uploading the gerber files (KicadV6\gerber\Pico_Interface.zip)  to a [PCB manufacurer](https://jlcpcb.com/) 


## Bil of Materials (BOM)

| Quantity | Reference(s) | Prod no             | Farnell order no | Description                                                |   |
|----------|--------------|---------------------|------------------|------------------------------------------------------------|---|
| 1        | C1           | T491A106K016AT      | 1457414          | T491A106K016AT CAP, 10µF, 16V, 10%, 1206, SMD              |   |
| 4        | J0-J3        | MC-254-12-00-ST-DIP | 2843528          | MC-254-12-00-ST-DIP CONNECTOR, HEADER, 12POS, 2ROW, 2.54MM |   |
| 1        | U2           | NCP1117ST33T3G      | 1652366          | NCP1117ST33T3G IC, LINEAR VOLTAGE REGULATOR                |   |
| 1        | U1           |                     |                  | Rasbperry Pi Pico                                          |   |

