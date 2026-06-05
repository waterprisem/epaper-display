/*
 * Joystick Calibration Test
 * Created: I forgot
 *
 * Test program for:
 * - Center calibration
 * - Axis normalization (-100 to 100)
 * - Deadzone verification
 */

// Variables to store the physical limits of YOUR joystick
const int VRX_PIN = 7; // Joystick X-axis
const int VRY_PIN = 16; // Joystick Y-axis
const int SW_PIN  = 15; // Joystick Switch

// Use long for the sum to avoid overflow
long xSum = 0;
long ySum = 0;
int xCenter, yCenter;
const int samples = 100;

void setup() {
  Serial.begin(115200);
  pinMode(SW_PIN, INPUT_PULLUP);

  Serial.println("CALIBRATING CENTER - DO NOT TOUCH JOYSTICK...");
  delay(500); // Wait for the stick to settle physically

  // Average 100 readings to find a "true" center
  for (int i = 0; i < samples; i++) {
    xSum += analogRead(VRX_PIN);
    ySum += analogRead(VRY_PIN);
    delay(10); 
  }
  
  xCenter = xSum / samples;
  yCenter = ySum / samples;

  Serial.printf("Center Found - X: %d, Y: %d\n", xCenter, yCenter);
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
  yVal = constrain(yVal, -100, 100);

  Serial.printf("X: %d | Y: %d\n", xVal, yVal);
  delay(100);
}