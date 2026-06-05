#include <GxEPD2_BW.h>
#include <Fonts/FreeMonoBold18pt7b.h>
#include <Fonts/Picopixel.h>
#include <Fonts/FreeMonoBoldOblique24pt7b.h>

#define EPD_CS    10
#define EPD_RST   9
#define EPD_BUSY  8

int cursorX = 0;
int cursorY = 0;

int update_screen_cursor();

//Joystick related stuff
const int VRX_PIN = 7; // Joystick X-axis
const int VRY_PIN = 16; // Joystick Y-axis
const int SW_PIN  = 15; // Joystick Switch
void calibrateJoystick();
long xSum = 0;
long ySum = 0;
int xCenter, yCenter;
const int samples = 100;

GxEPD2_BW<GxEPD2_it60_1448x1072, GxEPD2_it60_1448x1072::HEIGHT / 8> display(GxEPD2_it60_1448x1072(/*CS=*/10, /*DC=*/-1, /*RST=*/9, /*BUSY=*/8));

void setup() {
  Serial.begin(115200);
  
  SPI.begin(12, 13, 11, 10); // SCK, MISO, MOSI, CS
  delay(100); // give the IT8951 time to power up
  display.init(115200, true, 1000, false, SPI, SPISettings(4000000, MSBFIRST, SPI_MODE0));
  display.setRotation(90);
  display.setFont(&FreeMonoBoldOblique24pt7b);
  display.setTextColor(GxEPD_BLACK);
  display.setFullWindow();
  display.firstPage();
  do {
    display.fillScreen(GxEPD_WHITE);
    display.setCursor(100, 100);
  } while (display.nextPage());
  Serial.println("Done!");


  pinMode(SW_PIN, INPUT_PULLUP); //Joystick button pin

  calibrateJoystick();

}

void loop() {
  int xRaw = analogRead(VRX_PIN);
  int yRaw = analogRead(VRY_PIN);

  // Map each side separately relative to the averaged center
  // 4095 is the max for 12-bit ADC (ESP32/etc)
  int xVal = (xRaw < xCenter) ? map(xRaw, 0, xCenter, -100, 0) : map(xRaw, xCenter, 4095, 0, 100);
  int yVal = (yRaw < yCenter) ? map(yRaw, 0, yCenter, -100, 0) : map(yRaw, yCenter, 4095, 0, 100);

  // SET DEADZONE
  if (abs(xVal) <= 5) xVal = 0;
  if (abs(yVal) <= 5) yVal = 0;

  xVal = constrain(xVal, -100, 100);
  yVal = -constrain(yVal, -100, 100);

  Serial.printf("X: %d | Y: %d\n", xVal, yVal);

  if (xVal != 0 || yVal != 0) { // Changed 'OR' to '||'
    update_screen_cursor(xVal, yVal);
  }
  delay(300);
}

void update_screen_cursor(int x_change, int y_change) { // Added 'int' types
  Serial.printf("Old Cursor X: %d | Old Cursor Y: %d\n", cursorX, cursorY);

  cursorX += x_change; 
  cursorY += y_change;
  
  cursorX = constrain(cursorX, 0, display.width());
  cursorY = constrain(cursorY, 0, display.height());

  Serial.printf("New Cursor X: %d | New Cursor Y: %d\n", cursorX, cursorY);
}


void calibrateJoystick() {
  xSum = 0;
  ySum = 0;

  Serial.println("CALIBRATING CENTER - DO NOT TOUCH JOYSTICK...");
  delay(500); 

  for (int i = 0; i < samples; i++) {
    xSum += analogRead(VRX_PIN);
    ySum += analogRead(VRY_PIN);
    delay(10); 
  }
  
  xCenter = xSum / samples;
  yCenter = ySum / samples;

  Serial.printf("Calibration Finished! Center X: %d | Center Y: %d\n", xCenter, yCenter);
}