#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>

/**
  Pre-requisites
   - Blog to connect GPS to Camera: https://mayankgsingh.blogspot.com/2018/11/diy-gps-for-nikon-d7000.html
   - Arduino Nano: https://store.arduino.cc/usa/arduino-nano (5V)
   - Adafruit GPS: https://www.adafruit.com/product/746
  
  What I have?
  By default, GPS breakout board's baud rate is 9600.
  
  What I need?
  Nikon D7000 requires GPS unit to spit out data at 4800 baud rate
  
  This script will set the unit to 4800 baud rate.
  
  Why I needed the script/Arduio Nano?
  Initially, I configured the GPS device using FTDI cable and Raspberry PI.
  It was easy to send out a single PMTK command to set the baud rate.
  
  However, GPS unit was getting reset to default 9600 baud rate. I googled for reason:
  - could be because of voltage fluctuations
  - noise on TX,RX terminals etc.
  ... and I could not figure out the problem in my case.
  
  And once the unit is reset in field, I did not had access to anything using which I can easily reconfigure GPS.
  
  Eventually, I purchased Arduino nano as it will be small and I can use it to
  re-configure GPS unit.
  
  How script works?
  Connect the GPS unit to Arduino and power up.
  Script uses a brute force method to check and set baud rate to 4800.
  
  Indicators:
   - L indicator on Arduino keeps on flashing while the work is in progress.
   - Once the unit is at 4800 baud rate, L indicator will be turned on (no flashing)
   
   Script flips between 9600 and 4800 to read the data. Until the data is readable, it keeps on flipping.
   Once it starts reading data at 4800, stop the set/reset process.
   This approach was taken and commands in init() method was not effective in one shot.
   
   Once done, connect GPS to Camera unit.
*/
// Global vars
SoftwareSerial mySerial(3, 2);
Adafruit_GPS GPS(&mySerial);  //TX,RX

#define GPSECHO  true
#define PMTK_SET_BAUD_4800 "$PMTK251,4800*14"

boolean usingInterrupt = true;
void useInterrupt(boolean); // Func prototype keeps Arduino 0023 happy
boolean readable = false;
boolean isDefaultBaud = true;
int counter = 0;
int INDICATOR_PIN = 13;
boolean INDICATOR_ON = false;

void setup() {
  pinMode(INDICATOR_PIN,OUTPUT);

  Serial.begin(115200);
  Serial.println("Adafruit GPS library basic test!");

  GPS.begin(9600);
  //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  delay(1000);
  GPS.sendCommand(PMTK_SET_BAUD_4800);
  delay(1000);
  GPS.begin(4800);
  isDefaultBaud = false;
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);
  useInterrupt(true);
}

void loop() {
  if(readable && !isDefaultBaud) {
    if(!INDICATOR_ON) {
      digitalWrite(INDICATOR_PIN,HIGH);
      INDICATOR_ON = true;
    }
  } else {
    if(INDICATOR_ON) {
      digitalWrite(INDICATOR_PIN,LOW);
    } else {
      digitalWrite(INDICATOR_PIN,HIGH);
    }
    INDICATOR_ON = !INDICATOR_ON;
  }
  delay(500);
}

SIGNAL(TIMER0_COMPA_vect) {
  char c = GPS.read();
  // if you want to debug, this is a good time to do it!
  if(!readable)
    readable = (c == '$');
  if (!readable) {
    counter++;
    //GPS.sendCommand(PMTK_SET_BAUD_4800);
    //GPS.begin(4800);
  }
  if (counter >= 2000) {
    if(isDefaultBaud) {
      Serial.println("Retrying 4800...");
      GPS.begin(4800);
    } else {
      Serial.println("Retrying 9600...");
      GPS.begin(9600);
      delay(1000);
      GPS.sendCommand(PMTK_SET_BAUD_4800);
      readable = false;
    }
    isDefaultBaud = !isDefaultBaud;
    delay(1000);
    counter = 0;
  }
  if (GPSECHO && readable)
    UDR0 = c;
    // writing direct to UDR0 is much much faster than Serial.print
    // but only one character can be written at a time.
}

void useInterrupt(boolean v) {
  if (v) {
    // Timer0 is already used for millis() - we'll just interrupt somewhere
    // in the middle and call the "Compare A" function above
    OCR0A = 0xAF;
    TIMSK0 |= _BV(OCIE0A);
    usingInterrupt = true;
  } else {
    // do not call the interrupt function COMPA anymore
    TIMSK0 &= ~_BV(OCIE0A);
    usingInterrupt = false;
  }
}
