#include <Wire.h>
#include "MMA7660.h"

MMA7660 accelerometer;
const int TOUCH_PIN = 2;
bool accelerometerFound = false;
bool touchCandidate = false;
bool touchStable = false;
unsigned long touchSince = 0;
unsigned long lastRead = 0;
unsigned long lastScan = 0;
float ax = 0, ay = 0, az = 0;

void checkAccelerometer() {
  Wire.beginTransmission(0x4C);
  bool found = Wire.endTransmission() == 0;
  if (found && !accelerometerFound) {
    accelerometer.init();
    Serial.println("MMA7660: FOUND at 0x4C");
  } else if (!found) {
    Serial.println("MMA7660: NOT FOUND - check power, SDA/D11, SCL/D12");
  }
  accelerometerFound = found;
}

void setup() {
  Serial.begin(115200);
  unsigned long start = millis();
  while (!Serial && millis() - start < 5000) {}
  pinMode(TOUCH_PIN, INPUT_PULLDOWN);
  pinMode(LED_BUILTIN, OUTPUT);
  Wire.begin();
  Serial.println("TWO SENSORS TEST: keep fingers off touch pad at power-up.");
  Serial.println("Touch SIG -> D2; MMA7660 SDA -> D11, SCL -> D12.");
  checkAccelerometer();
}

void loop() {
  unsigned long now = millis();
  bool raw = digitalRead(TOUCH_PIN) == HIGH;
  if (raw != touchCandidate) {
    touchCandidate = raw;
    touchSince = now;
  }
  if (now - touchSince >= 40 && touchStable != touchCandidate) {
    touchStable = touchCandidate;
    Serial.println(touchStable ? "TOUCH EVENT: ON" : "TOUCH EVENT: OFF");
  }
  digitalWrite(LED_BUILTIN, touchStable ? HIGH : LOW);

  // Recheck the address periodically; do not change wires while powered.
  if (now - lastScan >= 2000) {
    lastScan = now;
    checkAccelerometer();
  }
  if (now - lastRead >= 200) {
    lastRead = now;
    if (accelerometerFound) {
      accelerometer.getAcceleration(&ax, &ay, &az);
      Serial.print("X="); Serial.print(ax, 2);
      Serial.print(" Y="); Serial.print(ay, 2);
      Serial.print(" Z="); Serial.print(az, 2);
    } else {
      Serial.print("XYZ=UNAVAILABLE");
    }
    Serial.print(" | TOUCH_RAW="); Serial.print(raw ? 1 : 0);
    Serial.print(" | TOUCH="); Serial.println(touchStable ? "ON" : "OFF");
  }
}
