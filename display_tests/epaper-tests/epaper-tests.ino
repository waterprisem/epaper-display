#include <GxEPD2_BW.h>
#include <Fonts/FreeMonoBold9pt7b.h>

#define EPD_CS    10
#define EPD_RST   9
#define EPD_BUSY  8

GxEPD2_BW<GxEPD2_it60_1448x1072, GxEPD2_it60_1448x1072::HEIGHT / 8> display(GxEPD2_it60_1448x1072(/*CS=*/10, /*DC=*/-1, /*RST=*/9, /*BUSY=*/8));
void setup() {
  Serial.begin(115200);
  SPI.begin(12, 13, 11, 10); // SCK, MISO, MOSI, CS

  delay(100); // give the IT8951 time to power up

  display.init(115200, true, 50, false, SPI, SPISettings(4000000, MSBFIRST, SPI_MODE0));

  display.setRotation(0);
  display.setFont(&FreeMonoBold9pt7b);
  display.setTextColor(GxEPD_BLACK);

  display.setFullWindow();
  display.firstPage();
  do {
    display.fillScreen(GxEPD_WHITE);
    display.setCursor(100, 100);
    display.print("Hello World!");
  } while (display.nextPage());

  Serial.println("Done!");
}

void loop() {}