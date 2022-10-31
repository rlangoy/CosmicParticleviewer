## Usage
This folder contains the files nessesary to grenerate the microcontroller firmware \
More info on the firmware is available in the [README](/ArduinoV2/alpideInterface/README.md) 

## Installation

The microcontroller firmware could be buildt using Command-Line tools (Arduino CLI) or the Arduino IDE 

### Installl Arduino CLI

The Arduino CLI is used to compile and program the device from the command-line \
To program the device enter the folowing :

```
wget -qO arduino-cli.tar.gz https://downloads.arduino.cc/arduino-cli/arduino-cli_latest_Linux_64bit.tar.gz
sudo tar xf arduino-cli.tar.gz -C /usr/local/bin arduino-cli
rm -rf arduino-cli.tar.gz
arduino-cli lib install ArduinoJson
 
arduino-cli config init --additional-urls https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.json
arduino-cli core install rp2040:rp2040

arduino-cli core update-index
arduino-cli compile --fqbn rp2040:rp2040:rpipico  alpideInterface 
arduino-cli  upload --fqbn rp2040:rp2040:rpipico  alpideInterface -p /dev/ttyACM0
```

### Install Arduino IDE 2.x
Download and install  [Arduino IDE 2.x](https://github.com/arduino/arduino-ide)
In Arduino IDE 2.x \ 
-  Install the compiler Raspberry Pi Pico/RP2040 by Eale F. Phillower v 2.5.x \
      Inside The Arduino IDE select: \
         File | Preferences.. | Aditional Board Managers \
        add the URL: https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.json
 
 
- Installation Library ArduinoJson
     Complete install instructions could be found [here](https://arduinojson.org/v6/doc/installation/) \
     Short Install instructions: \
       Open the Arduino Library Manager  [Ctrl] + [Shift] + I \
       Search for “ArduinoJson” \
       Select the version: 6.19.4   \
       Click install. 

Compile and Upload the code
