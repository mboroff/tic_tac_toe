/******************************************************************
 This is an example for the Adafruit RA8875 Driver board for TFT displays
 ---------------> http://www.adafruit.com/products/1590
 The RA8875 is a TFT driver for up to 800x480 dotclock'd displays
 It is tested to work with displays in the Adafruit shop. Other displays
 may need timing adjustments and are not guanteed to work.
 
 Adafruit invests time and resources providing this open
 source code, please support Adafruit and open-source hardware
 by purchasing products from Adafruit!
 
 Written by Limor Fried/Ladyada for Adafruit Industries.
 BSD license, check license.txt for more information.
 All text above must be included in any redistribution.
 ******************************************************************/

#include <Adafruit_GFX.h>    // Core graphics library
#include <SPI.h>
#include <Wire.h>      
#include "Adafruit_RA8875.h"
#include <Adafruit_STMPE610.h>


// Library only supports hardware SPI at this time
// Connect SCLK to UNO Digital #13 (Hardware SPI clock)
// Connect MISO to UNO Digital #12 (Hardware SPI MISO)
// Connect MOSI to UNO Digital #11 (Hardware SPI MOSI)
#define RA8875_INT 23
#define RA8875_CS 22
#define RA8875_RESET 29

Adafruit_RA8875 tft = Adafruit_RA8875(RA8875_CS, RA8875_RESET);
uint16_t tx, ty;

#define TS_MINX 150
#define TS_MINY 130
#define TS_MAXX 3800
#define TS_MAXY 4000

// The STMPE610 uses hardware SPI 
// Option #2 - use hardware SPI, connect to hardware SPI port only!
// SDI to MOSI(50), SDO to MISO(51), and SCL (53) to SPI CLOCK
// on Arduino Uno, that's 11, 12 and 13 respectively
// Then pick a CS pin, any pin is OK but we suggest #10 on an Uno
// tie MODE to 3.3V and POWER CYCLE the STMPE610 (there is no reset pin)



#define STMPE_CS 28
Adafruit_STMPE610 ts = Adafruit_STMPE610(STMPE_CS);

#define BOXSIZE 80
#define PENRADIUS 3
int oldcolor, currentcolor;

void setup() 
{
  Serial.begin(9600);
  Serial.println("RA8875 start");

  /* Initialise the display using 'RA8875_480x272' or 'RA8875_800x480' */
  if (!tft.begin(RA8875_800x480)) {
    Serial.println("RA8875 Not Found!");
    while (1);
  }

  Serial.println("Found RA8875");

  tft.displayOn(true);
  tft.GPIOX(true);      // Enable TFT - display enable tied to GPIOX
  tft.PWM1config(true, RA8875_PWM_CLK_DIV1024); // PWM output for backlight
  tft.PWM1out(255);

 tft.fillScreen(RA8875_BLACK);
  tft.fillRect(0, 0, BOXSIZE, BOXSIZE, RA8875_RED);
  tft.fillRect(BOXSIZE, 0, BOXSIZE, BOXSIZE, RA8875_YELLOW);
  tft.fillRect(BOXSIZE*2, 0, BOXSIZE, BOXSIZE, RA8875_GREEN);
  tft.fillRect(BOXSIZE*3, 0, BOXSIZE, BOXSIZE, RA8875_CYAN);
  tft.fillRect(BOXSIZE*4, 0, BOXSIZE, BOXSIZE, RA8875_BLUE);
  tft.fillRect(BOXSIZE*5, 0, BOXSIZE, BOXSIZE, RA8875_MAGENTA);
  // tft.fillRect(BOXSIZE*6, 0, BOXSIZE, BOXSIZE, WHITE);
 
  tft.drawRect(0, 0, BOXSIZE, BOXSIZE, RA8875_WHITE);
  currentcolor = RA8875_RED;
    pinMode(RA8875_INT, INPUT);
  digitalWrite(RA8875_INT, HIGH);
  
  tft.touchEnable(true);
  if (!ts.begin()) {
    Serial.println("Couldn't start touchscreen controller");
  }
  Serial.println("Touchscreen started");

  Serial.print("("); Serial.print(tft.width());
  Serial.print(", "); Serial.print(tft.height());
  Serial.println(")");

  Serial.print("Status: "); Serial.println(tft.readStatus(), HEX);
  Serial.println("Waiting for touch events ...");
}
#define MINPRESSURE 10
#define MAXPRESSURE 1000


void loop()
{
 float xScale = 1024.0F/tft.width();
  float yScale = 1024.0F/tft.height();

  /* Wait around for touch events */
if (ts.touched()) {
    TS_Point p = ts.getPoint();
    delay(500);
 //   while(ts.bufferSize()>0){
 //     ts.getPoint();           // scale from 0->1023 to tft.width

   if(p.z ==0){
     return; 
   }
    if (p.y < (TS_MINY-5)) {
      Serial.println("erase");
      // press the bottom of the screen to erase 
      tft.fillRect(0, BOXSIZE, tft.width(), tft.height()-BOXSIZE, RA8875_BLACK);
    }
//        Serial.print("X = "); Serial.print(p.x);
//    Serial.print("\tY = "); Serial.print(p.y);
//    Serial.print("\tPressure = "); Serial.println(p.z);
    p.x = map(p.x, TS_MINX, TS_MAXX, 0,tft.width());
    p.y = map(p.y, TS_MINY, TS_MAXY, 0,tft.height());
        Serial.print("X = "); Serial.print(p.x);
    Serial.print("\tY = "); Serial.print(p.y);
    Serial.print("\tPressure = "); Serial.println(p.z);
        int col = (p.x)/(tft.width()/20);
    int row = (p.y)/(tft.height()/20);
    Serial.print("("); Serial.print(row);
    Serial.print(", "); Serial.print(col);
    Serial.println(")");

    if(p.x<0||p.y<0){
      return; 
      /* Draw a circle */
      if (p.y < BOXSIZE) {
        oldcolor = currentcolor;
        if (p.x < BOXSIZE) {
         currentcolor = RA8875_RED;
          tft.drawRect(0, 0, BOXSIZE, BOXSIZE, RA8875_WHITE);
       } else if (p.x < BOXSIZE*2) {
         currentcolor = RA8875_YELLOW;
         tft.drawRect(BOXSIZE, 0, BOXSIZE, BOXSIZE, RA8875_WHITE);
       } else if (p.x < BOXSIZE*3) {
         currentcolor = RA8875_GREEN;
         tft.drawRect(BOXSIZE*2, 0, BOXSIZE, BOXSIZE, RA8875_WHITE);
       } else if (p.x < BOXSIZE*4) {
         currentcolor = RA8875_CYAN;
         tft.drawRect(BOXSIZE*3, 0, BOXSIZE, BOXSIZE, RA8875_WHITE);
       } else if (p.x < BOXSIZE*5) {
         currentcolor = RA8875_BLUE;
         tft.drawRect(BOXSIZE*4, 0, BOXSIZE, BOXSIZE, RA8875_WHITE);
       } else if (p.x < BOXSIZE*6) {
         currentcolor = RA8875_MAGENTA;
         tft.drawRect(BOXSIZE*5, 0, BOXSIZE, BOXSIZE, RA8875_WHITE);
       }
       if (oldcolor != currentcolor) {
          if (oldcolor == RA8875_RED) tft.fillRect(0, 0, BOXSIZE, BOXSIZE, RA8875_RED);
          if (oldcolor == RA8875_YELLOW) tft.fillRect(BOXSIZE, 0, BOXSIZE, BOXSIZE, RA8875_YELLOW);
          if (oldcolor == RA8875_GREEN) tft.fillRect(BOXSIZE*2, 0, BOXSIZE, BOXSIZE, RA8875_GREEN);
          if (oldcolor == RA8875_CYAN) tft.fillRect(BOXSIZE*3, 0, BOXSIZE, BOXSIZE, RA8875_CYAN);
          if (oldcolor == RA8875_BLUE) tft.fillRect(BOXSIZE*4, 0, BOXSIZE, BOXSIZE, RA8875_BLUE);
          if (oldcolor == RA8875_MAGENTA) tft.fillRect(BOXSIZE*5, 0, BOXSIZE, BOXSIZE, RA8875_MAGENTA);
       }
    }
    if (((p.y-PENRADIUS) > BOXSIZE) && ((p.y+PENRADIUS) < tft.height())) {
      tft.fillCircle(p.x, p.y, PENRADIUS, currentcolor);
    }
    }
  }
}



