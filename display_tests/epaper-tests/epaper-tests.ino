#include <GxEPD2_BW.h>
#include <Fonts/FreeMonoBold18pt7b.h>
#include <Fonts/Picopixel.h>
#include <Fonts/FreeMonoBoldOblique24pt7b.h>
#include "images.h"
#include "secrets.h"
#include <IRremote.hpp>
#include <WiFi.h>
#include <time.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#define EPD_CS    10
#define EPD_RST   9
#define EPD_BUSY  8

String fact;


const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;

const char* ntpServer1 = "pool.ntp.org"; // Primary NTP server
const char* ntpServer2 = "time.nist.gov"; // Secondary NTP server

const long gmtOffset_sec = -18000;  // Eastern Time (UTC-5)
const int daylightOffset_sec = 3600; // 1 hour daylight saving

int receiver = 15; // Signal Pin of IR receiver to Arduino Digital Pin 13

enum Page { HOME, TODO, RANDOM };
Page currentPage = HOME;
Page lastPage;

void drawHOME(); //Date and image
void drawTODO();
void drawRANDOM();
void updateRANDOM();


void translateIR();

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
  } while (display.nextPage());

  Serial.println("Done!");

  IrReceiver.begin(receiver, DISABLE_LED_FEEDBACK); // Start the receiver


  //wifi connection
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected.");

  // Configure time using configTime()
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer1, ntpServer2);
  drawHOME();

}

void loop() {
  //update clock info
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }
  // Print time in a readable format
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  delay(1000);

  //read IR REMOTE
  if (IrReceiver.decode()) // have we received an IR signal?
  {
    translateIR(); 
    //IrReceiver.printIRResultShort(&Serial);  // prints everything it received, useful for mapping to other remotes :)
    IrReceiver.resume(); // Enable receiving of the next value
  }
  if (lastPage != currentPage){
    switch(currentPage) {
      case HOME: drawHOME(); break;
      case TODO: drawTODO(); break;
      case RANDOM: drawRANDOM(); break;
    }
    lastPage = currentPage;
  }
}

//functions
void drawHOME(){
  Serial.println("HOME Page");


  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }
  // Print time in a readable format
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  delay(1000);

  display.setPartialWindow(100, 100, 1248, 900);
  display.firstPage();
  do {
    display.fillScreen(GxEPD_WHITE);
    display.setFont(&FreeMonoBoldOblique24pt7b);
    display.setTextColor(GxEPD_BLACK);
    display.setCursor(150, 150);
    display.print("HOME PAGE");

    display.setCursor(500, 400);
    display.print(&timeinfo, "%B %d %Y");
    
    display.drawBitmap (150, 500, bunny2DATA, 480, 480, GxEPD_BLACK);
    display.drawBitmap (450, 500, bunny3DATA, 480, 480, GxEPD_BLACK);
    display.drawBitmap (750, 500, bunny1DATA, 480, 480, GxEPD_BLACK);
  } while (display.nextPage());

} //Date and images

void drawTODO(){
  Serial.println("TODO Page");
  display.setPartialWindow(100, 100, 1248, 900);
  display.firstPage();
  do {
    display.fillScreen(GxEPD_WHITE);
    display.setCursor(150, 150);
    display.print("TODO PAGE");
  } while (display.nextPage());

}
void drawRANDOM(){
  updateRANDOM();
  Serial.println("RANDOM Page");
  display.setPartialWindow(100, 100, 1248, 900);
  display.firstPage();

  do {
    display.fillScreen(GxEPD_WHITE);
    display.setCursor(150, 150);
    display.setFont(&FreeMonoBoldOblique24pt7b);
    display.print("RANDOM PAGE");
    display.drawBitmap (900, 600, kittyDATA, 480, 480, GxEPD_BLACK);
    display.setFont(&FreeMonoBold18pt7b);
    display.setCursor(200, 400);
    display.print("Fun Fact:");
    printWrapped(fact, 250, 450, 1000, 50);
  } while (display.nextPage());
  
}

void updateRANDOM(){
  HTTPClient http;
  http.begin("https://uselessfacts.jsph.pl/api/v2/facts/random");
  int httpCode = http.GET();
  String payload = http.getString();
  http.end();
  
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, payload);
  fact = doc["text"].as<String>();
  Serial.println(fact);
}

void printWrapped(String text, int x, int y, int maxWidth, int lineHeight) {
  String line = "";
  String word = "";
  
  for (int i = 0; i <= text.length(); i++) {
    char c = text[i];
    if (c == ' ' || c == '\0') {
      // test if adding this word exceeds width
      int16_t x1, y1;
      uint16_t w, h;
      display.getTextBounds(line + word, x, y, &x1, &y1, &w, &h);
      if (w > maxWidth) {
        display.setCursor(x, y);
        display.print(line);
        y += lineHeight;
        line = word + " ";
      } else {
        line += word + " ";
      }
      word = "";
    } else {
      word += c;
    }
  }
  // print remaining text
  display.setCursor(x, y);
  display.print(line);
}



void translateIR() // takes action based on IR code received
{
  switch(IrReceiver.decodedIRData.command)
  {
  case 0x45: Serial.println("power"); break;
  case 0x46: Serial.println("mode"); break;
  case 0x47: Serial.println("mute"); break;
  case 0x44: Serial.println("pause"); 
    if (currentPage ==RANDOM){
      drawRANDOM();
    }
  
  
    break;
  case 0x40: Serial.println("back");
    if(currentPage > HOME) {  // HOME is the FIRST PAGE
      currentPage = (Page)(currentPage - 1);
    }
    break;
  case 0x43: Serial.println("forward");
    if(currentPage < RANDOM) {  // RANDOM is the LAST PAGE
      currentPage = (Page)(currentPage + 1);
    }
    break;
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