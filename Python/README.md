# picoif - Alpide Readout Firmware Interface
Library containing functions for interacting with the OB Alpides  

## Usage
Import library by adding : \
&nbsp;&nbsp; from picoif import OuterBarriel \
Initiate lib by specifying selial port using pySerial \
&nbsp;&nbsp; alpide=OuterBarriel(serialPort) \
Reset ALPIDES \
&nbsp;&nbsp;alpide.sendReset() \
Reset ALPIDES \
&nbsp;&nbsp;alpide.sendReset() \
Send Trigger & Reset signal to all ALPIDEs\ \
&nbsp;&nbsp; alpide.sendTrigger() \
Plot all pix hits \
&nbsp;&nbsp;alpide.plotAllChips() \

## Example
[plotAplideHitmap.py](plotAplideHitmap.py) - Plot ALPIDE Pixels Hits for all OB ALPIDES into a sigle plot
