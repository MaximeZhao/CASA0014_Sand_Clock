#include <Wire.h>
#include "MMA7660.h"

MMA7660 accelerometer;

// 1：Z 正面朝上；-1：Z 反面朝上；0：侧放
int candidate = 0;
int confirmed = 0;

unsigned long candidateSince = 0;
unsigned long lastRead = 0;
unsigned long lastPrint = 0;

void setup() {
  Serial.begin(9600);

  unsigned long start = millis();
  while (!Serial && millis() - start < 5000) {
  }

  Wire.begin();

  // 先确认能找到传感器
  Wire.beginTransmission(0x4C);
  if (Wire.endTransmission() != 0) {
    Serial.println("Sensor not found. Check wiring.");
    while (true) {
      delay(1000);
    }
  }

  accelerometer.init();
  Serial.println("Ready. Slowly turn the sensor over.");
}

void loop() {
  unsigned long now = millis();

  if (now - lastRead < 50) {
    return;
  }
  lastRead = now;

  float ax, ay, az;
  accelerometer.getAcceleration(&ax, &ay, &az);

  int current;

  if (az > 0.65f) {
    current = 1;
  } else if (az < -0.65f) {
    current = -1;
  } else {
    current = 0;
  }

  if (current != candidate) {
    candidate = current;
    candidateSince = now;
  }

  if (now - candidateSince >= 300) {
    confirmed = candidate;
  }

  if (now - lastPrint >= 250) {
    lastPrint = now;

    Serial.print("X=");
    Serial.print(ax, 2);
    Serial.print("  Y=");
    Serial.print(ay, 2);
    Serial.print("  Z=");
    Serial.print(az, 2);

    Serial.print("  POSITION=");

    if (confirmed == 1) {
      Serial.println("Z_UP");
    } else if (confirmed == -1) {
      Serial.println("Z_DOWN");
    } else {
      Serial.println("SIDE");
    }
  }
}