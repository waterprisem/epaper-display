#include <GxEPD2_BW.h>
#include <Fonts/FreeMono18pt7b.h>
#include <Fonts/Picopixel.h>
#include <Fonts/FreeMonoBoldOblique24pt7b.h>
#include "images.h"
#include <IRremote.hpp>


#define EPD_CS    10
#define EPD_RST   9
#define EPD_BUSY  8

const int PLUSbutton = 16; 
int lastPLUSbuttonState = 0;

int counter = 0;


int receiver = 15; // Signal Pin of IR receiver to Arduino Digital Pin 13

void translateIR() // takes action based on IR code received
{
  switch(IrReceiver.decodedIRData.command)
  {
  case 0x45: Serial.println("power"); break;
  case 0x46: Serial.println("mode"); break;
  case 0x47: Serial.println("mute"); break;
  case 0x44: Serial.println("pause"); break;
  case 0x40: Serial.println("back"); break;
  case 0x43: Serial.println("forward"); break;
  case 0x07: Serial.println("equalizer"); break;
  case 0x15: Serial.println("minus"); break;
  case 0x09: Serial.println("plus"); break;
  case 0x16: Serial.println("0"); break;
  case 0x19: Serial.println("arrows idk waht this does"); break;
  case 0xD: Serial.println("U/SD I also don't know what this does"); break;
  case 0xC: Serial.println("1"); break;
  case 0x18: Serial.println("2"); break;
  case 0x5E: Serial.println("3"); break;
  case 0x8: Serial.println("4"); break;
  case 0x1C: Serial.println("5"); break;
  case 0x5A: Serial.println("6"); break;
  case 0x42: Serial.println("7"); break;
  case 0x52: Serial.println("8"); break;
  case 0x4A: Serial.println("9"); break;

  default: 
    Serial.print("Unknown code: 0x");
    Serial.println(IrReceiver.decodedIRData.command, HEX);
  }

  delay(500); // Do not get immediate repeat
}

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

  IrReceiver.begin(receiver, DISABLE_LED_FEEDBACK); // Start the receiver


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
  if (IrReceiver.decode()) // have we received an IR signal?
  {
    translateIR(); 
    //IrReceiver.printIRResultShort(&Serial);  // prints everything it received, useful for mapping to other remotes :)
    IrReceiver.resume(); // Enable receiving of the next value
  }
}