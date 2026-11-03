// Arduino MKR WiFi 1010: 18-second reversible virtual hourglass
// Sensor: VCC -> 3.3V, GND -> GND, DO -> D2
const uint8_t SENSOR_PIN = 2;
const unsigned long DEBOUNCE_MS = 200;
const unsigned long PRINT_MS = 250;
const float FULL_TRANSFER_MS = 18000.0f;

int candidateState;
int stableState = -1;
unsigned long candidateSince = 0;
unsigned long previousUpdate = 0;
unsigned long lastPrint = 0;
float amountAtB = 0.0f; // 0 = all at A; 1 = all at B

void setup() {
  Serial.begin(115200);
  pinMode(SENSOR_PIN, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
  candidateState = digitalRead(SENSOR_PIN);
  candidateSince = millis();
  previousUpdate = millis();
  // Initial sand distribution: all at A. Keep orientation fixed when starting.
}

void loop() {
  const unsigned long now = millis();
  const int raw = digitalRead(SENSOR_PIN);
  if (raw != candidateState) {
    candidateState = raw;
    candidateSince = now;
  }
  if (now - candidateSince >= DEBOUNCE_MS) {
    stableState = candidateState;
  }

  const unsigned long elapsed = now - previousUpdate;
  previousUpdate = now;
  // Assumption for initial test: 0 means B is lower, 1 means A is lower.
  // Swap the branches if physical orientation is opposite.
  if (stableState == 0) {
    amountAtB += elapsed / FULL_TRANSFER_MS;
  } else if (stableState == 1) {
    amountAtB -= elapsed / FULL_TRANSFER_MS;
  }
  amountAtB = constrain(amountAtB, 0.0f, 1.0f);
  digitalWrite(LED_BUILTIN, stableState == 0 ? HIGH : LOW);

  if (now - lastPrint >= PRINT_MS) {
    lastPrint = now;
    Serial.print("RAW="); Serial.print(raw);
    Serial.print(" | STABLE="); Serial.print(stableState);
    Serial.print(" | A="); Serial.print((1.0f - amountAtB) * 100.0f, 1);
    Serial.print("% | B="); Serial.print(amountAtB * 100.0f, 1);
    Serial.println("%");
  }
}
