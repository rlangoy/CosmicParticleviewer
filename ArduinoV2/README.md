

## Installl Arduino CLI

The Arduino CLI is used to compile and program the device from the command-line


wget -qO arduino-cli.tar.gz https://downloads.arduino.cc/arduino-cli/arduino-cli_latest_Linux_64bit.tar.gz
sudo tar xf arduino-cli.tar.gz -C /usr/local/bin arduino-cli
m -rf arduino-cli.tar.gz
arduino-cli lib install ArduinoJson
 
arduino-cli config init --additional-urls https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.json
arduino-cli core install rp2040:rp2040

arduino-cli core update-index
arduino-cli compile --fqbn rp2040:rp2040:rpipico  alpideInterface 
arduino-cli  upload --fqbn rp2040:rp2040:rpipico  alpideInterface -p /dev/ttyACM0

