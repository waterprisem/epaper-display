#include <GxEPD2_BW.h>
#include <Fonts/FreeMono18pt7b.h>
#include <Fonts/Picopixel.h>
#include <Fonts/FreeMonoBoldOblique24pt7b.h>
#include "images.h"

#define EPD_CS    10
#define EPD_RST   9
#define EPD_BUSY  8

const int PLUSbutton = 16; 
int lastPLUSbuttonState = 0;

int counter = 0;


GxEPD2_BW<GxEPD2_it60_1448x1072, GxEPD2_it60_1448x1072::HEIGHT / 8> display(GxEPD2_it60_1448x1072(/*CS=*/10, /*DC=*/-1, /*RST=*/9, /*BUSY=*/8));

void setup() {

  Serial.begin(115200);
  SPI.begin(12, 13, 11, 10); // SCK, MISO, MOSI, CS

  delay(100); // give the IT8951 time to power up

  display.init(115200, true, 50, false, SPI, SPISettings(4000000, MSBFIRST, SPI_MODE0));

  display.setRotation(90);
  display.setFont(&FreeMonoBoldOblique24pt7b);
  display.setTextColor(GxEPD_BLACK);

  display.setFullWindow();
  display.firstPage();
  do {
    display.fillScreen(GxEPD_WHITE);
    display.setCursor(100, 100);
    display.print("Hello World!");
    display.drawBitmap (150, 150, kittyDATA, 480, 480, GxEPD_BLACK);
  } while (display.nextPage());

  

  Serial.println("Done!");
  pinMode(PLUSbutton, INPUT_PULLUP);

}

void loop() {
  if (Serial.available() > 0) {
    String incomingString = Serial.readStringUntil('\n');
    display.setPartialWindow(700, 700, 500, 500);

    display.firstPage();
    do {
      display.fillScreen(GxEPD_WHITE);
      display.setFont(&FreeMono18pt7b);
      display.setCursor(750, 750);
      display.print(incomingString);
    } while (display.nextPage());
    
  }



  if (digitalRead(PLUSbutton)  == LOW && lastPLUSbuttonState==0) {
    Serial.println("whatt");
    lastPLUSbuttonState = 1;
    counter +=1;

    display.setPartialWindow(600, 600, 100, 100);

    display.firstPage();
    do {
      display.fillScreen(GxEPD_WHITE);
      display.setFont(&FreeMono18pt7b);
      display.setCursor(650, 650);
      display.print(counter);
    } while (display.nextPage());

  }

  if(digitalRead(PLUSbutton)  == HIGH){
    lastPLUSbuttonState = 0;
  }
}