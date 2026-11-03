const int SENSOR_PIN = 2;
const unsigned long DEBOUNCE_MS = 200;
int candidate;
int stableState = -1;
unsigned long changedAt = 0;
unsigned long lastPrint = 0;

void setup() {
  Serial.begin(115200);
  pinMode(SENSOR_PIN, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
  candidate = digitalRead(SENSOR_PIN);
  changedAt = millis();
}

void loop() {
  unsigned long now = millis();
  int raw = digitalRead(SENSOR_PIN);
  if (raw != candidate) {
    candidate = raw;
    changedAt = now;
  }
  if (now - changedAt >= DEBOUNCE_MS) {
    stableState = candidate;
  }

  digitalWrite(LED_BUILTIN, stableState == LOW ? HIGH : LOW);

  if (now - lastPrint >= 250) {
    lastPrint = now;
    Serial.print("RAW=");
    Serial.print(raw);
    Serial.print(" | STABLE=");
    if (stableState < 0) {
      Serial.println("WAIT");
    } else {
      Serial.println(stableState);
    }
  }
}

