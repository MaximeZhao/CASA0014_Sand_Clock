#include "Hourglass.h"

const int TILT_PIN = 2;
// After mounting: if contact CLOSED really means B is up, change true to false.
const bool CLOSED_MEANS_A_UP = true;
Hourglass hourglass(CLOSED_MEANS_A_UP, 200, 18.0f);
unsigned long lastPrint = 0;

void setup() {
  pinMode(TILT_PIN, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  // No infinite while(!Serial): it must also start without a computer.
  hourglass.begin(millis(), digitalRead(TILT_PIN));
}

void loop() {
  unsigned long now = millis();
  int raw = digitalRead(TILT_PIN);
  hourglass.update(now, raw);
  digitalWrite(LED_BUILTIN,
               hourglass.ready() && hourglass.aUp() ? HIGH : LOW);

  if (now - lastPrint >= 500) {
    lastPrint = now;
    Serial.print("RAW=");
    Serial.print(raw);
    if (!hourglass.ready()) {
      Serial.println(" | waiting for stable orientation");
    } else {
      Serial.print(hourglass.aUp() ? " | A UP" : " | B UP");
      Serial.print(" | A=");
      Serial.print(hourglass.amountA() * 100.0f, 1);
      Serial.print("% | B=");
      Serial.print(hourglass.amountB() * 100.0f, 1);
      Serial.println("%");
    }
  }
}

