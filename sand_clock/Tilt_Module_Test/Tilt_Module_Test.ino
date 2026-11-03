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

  // 原始读数改变时，重新开始计算稳定时间
  if (raw != candidate) {
    candidate = raw;
    changedAt = now;
  }

  // 保持同一个读数 200ms 后，才接受它
  if (now - changedAt >= DEBOUNCE_MS) {
    stableState = candidate;
  }

  // 开发板内置灯仅用来辅助观察状态
  digitalWrite(
    LED_BUILTIN,
    stableState == LOW ? HIGH : LOW
  );

  // 每 250ms 显示一次数据
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