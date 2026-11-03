#include <SPI.h>
#include <Wire.h>
#include "MMA7660.h"
#include <math.h>
#include <WiFiNINA.h>
#include <PubSubClient.h>
#include "arduino_secrets.h"

// ======================================
// 1. WiFi Configuration
// ======================================

const char* ssid = SECRET_SSID;
const char* password = SECRET_PASS;

// ======================================
// 2. MQTT Configuration
// ======================================

const char* mqtt_server = "mqtt.cetools.org";
const int mqtt_port = 1884;

const char* mqtt_username = SECRET_MQTTUSER;
const char* mqtt_password = SECRET_MQTTPASS;

// Luminaire ID: 29
const char* mqtt_topic =
  "student/CASA0014/luminaire/29";

// ======================================
// 3. WiFi and MQTT Clients
// ======================================

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

// ======================================
// 4. LED Configuration
// ======================================

// 72 LEDs, each with R, G, B
const int num_leds = 72;
const int payload_size = num_leds * 3;

// 216-byte RGB buffer
byte RGBpayload[payload_size] = {0};

// Animation speed (milliseconds)
const unsigned long updateInterval = 100;

// ======================================
// 5. Connect to WiFi
// ======================================

void startWifi() {

  Serial.println("Connecting to WiFi...");

  while (WiFi.status() != WL_CONNECTED) {

    WiFi.begin(ssid, password);

    delay(5000);

    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("WiFi retrying...");
    }
  }

  Serial.println("WiFi Connected!");

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

// ======================================
// 6. Connect to MQTT
// ======================================

void reconnectMQTT() {

  while (!mqttClient.connected()) {

    // Make sure WiFi is connected
    if (WiFi.status() != WL_CONNECTED) {
      startWifi();
    }

    Serial.println("Connecting to MQTT...");

    // Generate MQTT Client ID
    // Does not use WiFi.macAddress()
    String clientId =
      "MKR1010_Random_" +
      String(random(0xffff), HEX);

    if (mqttClient.connect(
          clientId.c_str(),
          mqtt_username,
          mqtt_password
        )) {

      Serial.println("MQTT Connected!");

      Serial.print("Publishing to: ");
      Serial.println(mqtt_topic);

    } else {

      Serial.print("MQTT Failed. State: ");
      Serial.println(mqttClient.state());

      delay(5000);
    }
  }
}


// Virtual display: 12 columns, 6 rows. LED = column*6 + row.
MMA7660 sensor;
const int TOUCH_PIN=2; // Touch SIG -> D2, powered at 3.3V.
bool touchRaw=false, touchStable=false, touchPressedEvent=false;
bool paused=false;
unsigned long touchChangedAt=0, breathStartedAt=0;
unsigned long animationNow=0, animationTick=0;

void readTouch(unsigned long now) {
  touchPressedEvent=false;
  bool raw=digitalRead(TOUCH_PIN)==HIGH;
  if (raw!=touchRaw) { touchRaw=raw; touchChangedAt=now; }
  if (now-touchChangedAt>=(touchRaw ? 40UL : 60UL) && touchStable!=touchRaw) {
    touchStable=touchRaw;
    // One event per stable press; release only rearms the next tap.
    if (touchStable) touchPressedEvent=true;
    Serial.println(touchStable ? "TOUCH INPUT: ON" : "TOUCH INPUT: OFF");
  }
}

const float DURATION_SECONDS = 30.0f;
const int GRAINS = 30;
const float FLOWING_GRAINS = 12.0f; // About 12 moving grains during mid-flow.
byte displayBrightness = 255;      // 0..255; serial + / - adjusts by 20.
float sandA = 1.0f;
float ax = 0, ay = 0, az = 1;
float prevX = 0, prevY = 0, prevZ = 1;
float disturbance = 0;
float gravityX=0, gravityY=0;
float lateralX=0, lateralY=0, sway=0;
float peakX=0, peakY=0;
const float SHAKE_THRESHOLD = 0.28f;
bool autoShakeAxis=true;
unsigned long shakeWindowStart=0, lastShakeHit=0;
int shakeHits=0, shakeReversals=0, lastShakeSign=0;
bool shakeActive=false;
bool shakeAllowed=false;
unsigned long orientationGuardSince=0;
const unsigned long SHAKE_REARM_MS=250;
bool useYAxis=false; // Send x or y to select the controller's horizontal axis.
int candidate = 0, direction = 0;
bool started = false, done = false, haveReading = false;
unsigned long candidateSince = 0, quietSince = 0;
unsigned long lastSensor = 0, lastFrame = 0, lastStatus = 0;
unsigned long lastClock = 0;
int jitter[GRAINS];

void pixel(int col, int row, byte r, byte g, byte b) {
  if (col < 0 || col >= 12 || row < 0 || row >= 6) return;
  int i = (col * 6 + row) * 3;
  RGBpayload[i] = (unsigned int)r * displayBrightness / 255;
  RGBpayload[i+1] = (unsigned int)g * displayBrightness / 255;
  RGBpayload[i+2] = (unsigned int)b * displayBrightness / 255;
}

// Smooth RGB colour wheel for the falling grain and shake ripple.
void rainbowPixel(int col, int row, unsigned long phase, float brightness) {
  int hue = phase % 768;
  int segment = hue / 256;
  int t = hue % 256;
  int r = 0, g = 0, b = 0;
  if (segment == 0) { r = 255-t; g = t; }
  else if (segment == 1) { g = 255-t; b = t; }
  else { b = 255-t; r = t; }
  pixel(col, row, (byte)(r*brightness),
        (byte)(g*brightness), (byte)(b*brightness));
}

void readMotion(unsigned long now) {
  if (now-lastSensor<40) return;
  lastSensor=now;
  sensor.getAcceleration(&ax,&ay,&az);
  if (!haveReading) { gravityX=ax; gravityY=ay; }
  gravityX+=0.10f*(ax-gravityX);
  gravityY+=0.10f*(ay-gravityY);
  lateralX=ax-gravityX; lateralY=ay-gravityY;
  peakX=fmaxf(peakX,fabsf(lateralX));
  peakY=fmaxf(peakY,fabsf(lateralY));
  haveReading=true;

  // Only trust orientation while acceleration resembles resting gravity.
  // Saturated or rapidly changing samples may still drive shake animation.
  float dx=ax-prevX, dy=ay-prevY, dz=az-prevZ;
  float change=sqrtf(dx*dx+dy*dy+dz*dz);
  float magnitude=sqrtf(ax*ax+ay*ay+az*az);
  prevX=ax; prevY=ay; prevZ=az;
  bool orientationValid=magnitude>0.75f && magnitude<1.30f &&
    fabsf(ax)<1.40f && fabsf(ay)<1.40f && fabsf(az)<1.40f && change<0.40f;
  int pose=az>0.45f ? 1 : (az < -0.45f ? -1 : 0);
  // Continue collecting live diagnostics while paused, without acting on motion.
  if (paused) {
    candidate=0; candidateSince=now;
    return;
  }
  if (!orientationValid) {
    candidate=0; candidateSince=now;
  } else if (pose!=candidate) {
    candidate=pose; candidateSince=now;
  }
  if (orientationValid && pose!=0 && pose!=direction && now-candidateSince>=450) {
    bool directionChanged=pose!=direction;
    direction=pose;
    orientationGuardSince=now;
    disturbance=0; sway=0; shakeActive=false;
    shakeHits=0; shakeReversals=0; lastShakeSign=0;
    if (!started) {
      sandA=direction==1 ? 1.0f : 0.0f;
      started=true;
      Serial.println("START");
    }
    if (directionChanged) done=false;
    Serial.println(direction==1 ? "Direction: DOWN" : "Direction: UP");
  }

  // A plausible, sustained new pose blocks shake. A brief acceleration
  // spike alone must not close the gate or reset the animation.
  bool turning=orientationValid && candidate!=direction &&
               now-candidateSince>=160;
  bool aligned=started && direction!=0 && !turning;
  if (!aligned || done) {
    orientationGuardSince=now;
    disturbance=0; sway=0; shakeActive=false;
  }
  shakeAllowed=aligned && !done && now-orientationGuardSince>=SHAKE_REARM_MS;
  float selected=autoShakeAxis ?
    (fabsf(lateralX)>fabsf(lateralY) ? lateralX : lateralY) :
    (useYAxis ? lateralY : lateralX);
  if (!shakeAllowed) {
    shakeHits=0; shakeReversals=0; lastShakeSign=0;
    shakeActive=false;
  } else {
    // Rolling evidence: strong back-and-forth samples need not fit a fixed window.
    if (now-shakeWindowStart>450) {
      shakeHits=0; shakeReversals=0; lastShakeSign=0;
    }
    if (fabsf(selected)>SHAKE_THRESHOLD) {
      shakeWindowStart=now;
      if (shakeHits<20) shakeHits++;
      int sign=selected>=0 ? 1 : -1;
      if (lastShakeSign!=0 && sign!=lastShakeSign) shakeReversals++;
      lastShakeSign=sign;
      if (shakeHits>=3 && shakeReversals>=1) {
        shakeActive=true; lastShakeHit=now;
      }
    }
    if (now-lastShakeHit>400) shakeActive=false;
  }
  disturbance*=0.95f;
  if (shakeActive) {
    disturbance=fminf(1.0f,disturbance+0.30f);
    sway=0.65f*sway+0.35f*constrain(selected*5.0f,-4.0f,4.0f);
  } else { sway*=0.80f; }
  if (disturbance<0.03f) disturbance=0;
}

void updateClock(unsigned long now) {
  unsigned long dt=now-lastClock;
  lastClock=now;
  if (!started || direction==0 || done || paused) return;
  // Continue the countdown through shaking/sideways movement.
  float step=(float)dt/(DURATION_SECONDS*1000.0f);
  sandA=constrain(sandA-direction*step,0.0f,1.0f);
  done=(direction==1 && sandA<=0) || (direction==-1 && sandA>=1);
  if (done) {
    breathStartedAt=animationNow;
    Serial.println("DONE: breathing. Flip to run back.");
  }
}

// Packed mound: 12 bottom cells, 10 middle cells, 8 upper cells.
void moundSlot(int slot, int &col, int &height) {
  const int centreOut[12]={5,6,4,7,3,8,2,9,1,10,0,11};
  if (slot<12) { col=centreOut[slot]; height=0; }
  else if (slot<22) { col=centreOut[slot-12]; height=1; }
  else { col=centreOut[slot-22]; height=2; }
}

void drawFrame(unsigned long now) {
  // Keep the last complete frame, including colours and shake offsets.
  if (!done && (paused)) return;
  memset(RGBpayload,0,sizeof(RGBpayload));
  if (!started) {
    pixel(5,2,0,25,35); pixel(6,3,0,25,35);
    return;
  }
  // Completion automatically breathes; touch is no longer the breathing switch.
  float wave=0.5f+0.5f*cosf((now-breathStartedAt)*6.2831853f/3000.0f);
  float breath=0.25f+0.75f*wave;
  // One shared transfer coordinate preserves each trajectory on reversal.
  float transfer=(1.0f-sandA)*(GRAINS+FLOWING_GRAINS);
  // Draw settled grains before moving grains so colour remains visible.
  for (int pass=0; pass<2; pass++) {
    for (int i=0;i<GRAINS;i++) {
      float q=constrain((transfer-i)/FLOWING_GRAINS,0.0f,1.0f);
      bool falling=q>0.0f && q<1.0f;
      if ((pass==0 && falling) || (pass==1 && !falling)) continue;
      int aCol,aHeight,bCol,bHeight;
      // Remove exposed upper grains first; fill the lower mound bottom-first.
      moundSlot(GRAINS-1-i,aCol,aHeight);
      moundSlot(i,bCol,bHeight);
      // Mirror target horizontally: trajectories cover the whole display.
      bCol=11-bCol;
      float x=aCol+(bCol-aCol)*q;
      float y=aHeight+(5-bHeight-aHeight)*q;
      if (falling) {
        x+=sinf(q*3.14159265f)*sinf(i*2.4f)*1.5f;
        // Deterministic waves move smoothly rather than randomly blinking.
        x+=sway+disturbance*4.2f*sinf(now/150.0f+i*1.7f);
        y+=disturbance*0.55f*sinf(now/130.0f+i);
        int col=constrain((int)roundf(x),0,11);
        int row=constrain((int)roundf(y),0,5);
        rainbowPixel(col,row,now/5+i*117,155.0f / 255.0f);
      } else {
        if (disturbance>0.12f && !done) {
          x+=sway+disturbance*3.5f*sinf(now/170.0f+i*1.3f);
        }
        int col=constrain((int)roundf(x),0,11);
        int row=constrain((int)roundf(y),0,5);
        bool receivingPile=direction==1 ? q>=1.0f : q<=0.0f;
        float level=receivingPile && done ? breath : 1.0f;
        pixel(col,row,(byte)(255*level),
              (byte)((115+(i%3)*15)*level),(byte)(8*level));
      }
    }
  }
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  pinMode(TOUCH_PIN, INPUT_PULLDOWN);
  Serial.begin(115200);
  delay(1500);
  Wire.begin();
  Wire.beginTransmission(0x4C);
  if (Wire.endTransmission()!=0) {
    Serial.println("MMA7660 not found: check wiring.");
    while (true) delay(1000);
  }
  sensor.init();
  randomSeed(micros());
  startWifi();
  mqttClient.setServer(mqtt_server,mqtt_port);
  mqttClient.setBufferSize(512);
  mqttClient.setKeepAlive(30);
  reconnectMQTT();
  lastClock=millis();
  animationTick=lastClock;
  quietSince=lastClock;
  Serial.println("Sand clock ready. Hold sensor still, Z facing up or down.");
  Serial.println("FULL TEST: 30s | falling 155 | orange 255 | touch FREEZE | DONE breathes 3s");
  Serial.println("r=restart, a=auto shake axis, x/y=manual axis, +/-=brightness");
  Serial.println("Tap once: PAUSE. Release. Tap again: RESUME. Holding does not repeat.");
}

void loop() {
  // Pause the clock across reconnection, rather than skipping the animation.
  if (WiFi.status()!=WL_CONNECTED || !mqttClient.connected()) {
    reconnectMQTT();
    lastClock=millis();
    animationTick=lastClock;
    candidateSince=lastClock;
    quietSince=lastClock;
  }
  mqttClient.loop();
  unsigned long now=millis();
  while (Serial.available()) {
    char c=Serial.read();
    if (c=='x' || c=='X' || c=='y' || c=='Y') {
      autoShakeAxis=false;
      useYAxis=(c=='y' || c=='Y');
      orientationGuardSince=now;
      shakeActive=false; disturbance=0; sway=0;
      shakeHits=0; shakeReversals=0; lastShakeSign=0;
      Serial.println(useYAxis ? "Horizontal axis: Y" : "Horizontal axis: X");
    }
    if (c=='a' || c=='A') { autoShakeAxis=true; Serial.println("Shake axis: AUTO"); }
    if (c=='+' || c=='-') {
      int next = (int)displayBrightness + (c=='+' ? 20 : -20);
      displayBrightness = (byte)constrain(next, 0, 255);
      Serial.print("Brightness (0..255): ");
      Serial.println(displayBrightness);
    }
    if (c=='r' || c=='R') {
      started=false; done=false; paused=false; direction=0; candidate=0;
      disturbance=0; sway=0; peakX=0; peakY=0; shakeActive=false;
      shakeHits=0; shakeReversals=0; lastShakeSign=0; shakeWindowStart=now; candidateSince=now; quietSince=now;
      orientationGuardSince=now; shakeAllowed=false;
      haveReading=false; breathStartedAt=animationNow;
      lastClock=now; animationTick=now;
      Serial.println("RESET: hold sensor still.");
    }
  }
  bool wasHeld=paused;
  readTouch(now);
  if (touchPressedEvent && started && !done) paused=!paused;
  if (touchPressedEvent && (!started || done))
    Serial.println("Touch ignored: start by flipping; finished pile keeps breathing.");
  digitalWrite(LED_BUILTIN, paused ? HIGH : LOW);
  if (wasHeld!=paused) {
    // Do not charge the edge's elapsed interval to the countdown.
    lastClock=now;
    candidate=0; candidateSince=now;
    if (!paused && !done) {
      haveReading=false;
      Serial.println("RESUMED immediately. Flip confirmation runs independently.");
    }
    if (paused) Serial.println(done ? "DONE breathing continues" : "PAUSED: frame and timer frozen");
  }
  if (!paused && !wasHeld)
    animationNow+=now-animationTick;
  animationTick=now;
  readMotion(now);
  updateClock(now);
  if (now-lastFrame>=100) {
    lastFrame=now;
    drawFrame(animationNow);
    if (!mqttClient.publish(mqtt_topic,RGBpayload,payload_size))
      Serial.println("Publish failed");
  }
  if (now-lastStatus>=1000) {
    lastStatus=now;
    Serial.print("X="); Serial.print(ax,2);
    Serial.print(" Y="); Serial.print(ay,2);
    Serial.print(" Z="); Serial.print(az,2);
    Serial.print(" X_PEAK="); Serial.print(peakX,2);
    Serial.print(" Y_PEAK="); Serial.print(peakY,2);
    peakX=0; peakY=0;
    Serial.print(" TOUCH_INPUT="); Serial.print(touchStable ? "ON" : "OFF");
    Serial.print(" PAUSED="); Serial.print(paused ? "YES" : "NO");
    float remaining=!started ? DURATION_SECONDS :
      (direction==1 ? sandA : 1.0f-sandA)*DURATION_SECONDS;
    Serial.print(" LEFT_s="); Serial.print(remaining,1);
    Serial.print(" DIR="); Serial.print(direction==1 ? "DOWN" : (direction==-1 ? "UP" : "WAIT"));
    Serial.print(" A%="); Serial.print(sandA*100,0);
    Serial.print(" SHAKE="); Serial.print(disturbance,2);
    Serial.print(" SHAKE_GATE="); Serial.print(shakeAllowed ? "READY" : "BLOCKED");
    Serial.print(" SHAKE_ACTIVE="); Serial.print(shakeActive ? "YES" : "NO");
    Serial.print(" STATE=");
    Serial.println(done ? "DONE_BREATHING" : (paused ? "PAUSED" :
      (!started ? "WAIT" : "FLOWING")));
  }
}
