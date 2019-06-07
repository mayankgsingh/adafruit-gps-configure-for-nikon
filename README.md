# adafruit-gps-configure-for-nikon

GPS configure script on Arduino

## Pre-requisites
- Blog to connect GPS to Camera: https://mayankgsingh.blogspot.com/2018/11/diy-gps-for-nikon-d7000.html
- Arduino Nano: https://store.arduino.cc/usa/arduino-nano (5V)
- Adafruit GPS: https://www.adafruit.com/product/746
  
## What we have?
By default, GPS breakout board's baud rate is 9600.

## What we need?
Nikon D7000 requires GPS unit to spit out data at 4800 baud rate
 
## Why I needed the script/Arduio Nano?
Initially, I configured the GPS device using FTDI cable and Raspberry PI. It was easy to send out a single PMTK command to set the baud rate.

However, GPS unit was getting reset to default 9600 baud rate. I googled for reason:
- could be because of voltage fluctuations
- noise on TX,RX terminals etc.

... and I could not figure out the problem in my case.
Once the unit is reset in field, I did not had access to anything using which I can easily reconfigure GPS.

Eventually, I purchased Arduino nano as it will be small and I can use it to re-configure GPS unit.
  
## How script works?
Connect the GPS unit to Arduino and power up. Script uses a brute force method to check and set baud rate to 4800.
  
### Indicators:
- L indicator on Arduino keeps on flashing while the work is in progress.
- Once the unit is at 4800 baud rate, L indicator will be turned on (no flashing)
   
Script flips between 9600 and 4800 to read the data. Until the data is readable, it keeps on flipping. Once it starts reading data at 4800, we stop the set/reset process. This approach was taken and commands in init() method was not effective in one shot.
   
Once done, connect GPS to Camera unit.
