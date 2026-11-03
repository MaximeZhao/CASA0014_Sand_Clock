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

// Luminaire ID: 17
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
const float DURATION_SECONDS = 18.0f;
const int GRAINS = 24;
float sandA = 1.0f;
float ax = 0, ay = 0, az = 1;
float prevX = 0, prevY = 0, prevZ = 1;
float disturbance = 0;
int candidate = 0, direction = 0;
bool started = false, done = false, haveReading = false;
unsigned long candidateSince = 0, quietSince = 0;
unsigned long lastSensor = 0, lastFrame = 0, lastStatus = 0;
unsigned long lastClock = 0;
int jitter[GRAINS];

void pixel(int col, int row, byte r, byte g, byte b) {
  if (col < 0 || col >= 12 || row < 0 || row >= 6) return;
  int i = (col * 6 + row) * 3;
  RGBpayload[i] = r;
  RGBpayload[i+1] = g;
  RGBpayload[i+2] = b;
}

void readMotion(unsigned long now) {
  if (now - lastSensor < 40) return;
  lastSensor = now;
  sensor.getAcceleration(&ax, &ay, &az);
  float dx = ax-prevX, dy = ay-prevY, dz = az-prevZ;
  float delta = haveReading ? sqrtf(dx*dx+dy*dy+dz*dz) : 0;
  float mag = sqrtf(ax*ax+ay*ay+az*az);
  prevX=ax; prevY=ay; prevZ=az; haveReading=true;
  bool moving = delta > 0.25f || fabsf(mag-1.0f)>0.30f;
  disturbance *= 0.90f;
  if (moving) {
    disturbance = fminf(1.0f, disturbance+delta*0.55f+0.10f);
    quietSince=now;
  }
  int pose = az>0.65f ? 1 : (az < -0.65f ? -1 : 0);
  if (moving || pose != candidate) {
    candidate=pose;
    candidateSince=now;
  }
  // Commit orientation only after movement has settled.
  if (!moving && pose != 0 && now-candidateSince>=450 &&
      now-quietSince>=450 && pose != direction) {
    direction=pose;
    if (!started) {
      sandA=direction==1 ? 1.0f : 0.0f;
      started=true;
      Serial.println("START");
    }
    done=false;
    Serial.println(direction==1 ? "Direction: DOWN" : "Direction: UP");
  }
}

void updateClock(unsigned long now) {
  unsigned long dt=now-lastClock;
  lastClock=now;
  if (!started || direction==0 || done) return;
  // Continue the countdown through shaking/sideways movement.
  float step=(float)dt/(DURATION_SECONDS*1000.0f);
  sandA=constrain(sandA-direction*step,0.0f,1.0f);
  done=(direction==1 && sandA<=0) || (direction==-1 && sandA>=1);
  if (done) Serial.println("DONE: flashing. Flip to run back.");
}

void drawFrame(unsigned long now) {
  memset(RGBpayload,0,sizeof(RGBpayload));
  if (!started) {
    pixel(5,2,0,25,35); pixel(6,3,0,25,35);
    return;
  }
  int upper=constrain((int)roundf(sandA*GRAINS),0,GRAINS);
  int lower=GRAINS-upper;
  int source=direction==1 ? upper : lower;
  bool transit=!done && source>0;
  // Reserve one grain for the falling trail: keep total grain count 24.
  if (transit) {
    if (direction==1) upper--; else lower--;
  }
  bool lit=!done || (now/600)%2==0;
  if (!lit) return;

  // Centre-out packing makes each reservoir collect towards its centre.
  const int order[12]={5,6,4,7,3,8,2,9,1,10,0,11};
  int shift=(int)roundf(constrain(ax,-1.0f,1.0f)*disturbance*2.0f);
  for (int i=0;i<GRAINS;i++) {
    jitter[i]=disturbance>0.15f ? (int)random(-1,2) : 0;
  }
  for (int i=0;i<upper;i++) {
    int col=order[i%12];
    int row=1-i/12;
    int target=constrain(col+shift+jitter[i],0,11);
    // Find a free cell so disturbances do not merge reservoir grains.
    for (int k=0;k<12;k++) {
      int c=(target+k)%12;
      if (RGBpayload[(c*6+row)*3]==0) {
        pixel(c,row,150,65+(i%3)*15,8); break;
      }
    }
  }
  for (int i=0;i<lower;i++) {
    int col=order[i%12];
    int row=5-i/12;
    int target=constrain(col+shift+jitter[i],0,11);
    for (int k=0;k<12;k++) {
      int c=(target+k)%12;
      if (RGBpayload[(c*6+row)*3]==0) {
        pixel(c,row,150,65+(i%3)*15,8); break;
      }
    }
  }
  if (transit) {
    int row=2+(now/180)%2;
    if (direction==-1) row=5-row;
    int col=5+(now/360)%2+shift;
    if (disturbance>0.15f) col+=(int)random(-2,3);
    col=constrain(col,0,11);
    pixel(col,row,220,150,35);
  }
  // A short-lived luminous ripple makes shaking visible beyond the piles.
  if (disturbance>0.20f) {
    int row=2+(now/100)%2;
    int col=(now/100)%12;
    pixel(col,row,50*disturbance,100*disturbance,130*disturbance);
  }
}

void setup() {
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
  quietSince=lastClock;
  Serial.println("Sand clock ready. Hold sensor still, Z facing up or down.");
  Serial.println("r: reset; duration for a full transfer = 18 seconds.");
}

void loop() {
  // Pause the clock across reconnection, rather than skipping the animation.
  if (WiFi.status()!=WL_CONNECTED || !mqttClient.connected()) {
    reconnectMQTT();
    lastClock=millis();
    candidateSince=lastClock;
    quietSince=lastClock;
  }
  mqttClient.loop();
  unsigned long now=millis();
  while (Serial.available()) {
    char c=Serial.read();
    if (c=='r' || c=='R') {
      started=false; done=false; direction=0; candidate=0;
      disturbance=0; candidateSince=now; quietSince=now;
      Serial.println("RESET: hold sensor still.");
    }
  }
  readMotion(now);
  updateClock(now);
  if (now-lastFrame>=100) {
    lastFrame=now;
    drawFrame(now);
    if (!mqttClient.publish(mqtt_topic,RGBpayload,payload_size))
      Serial.println("Publish failed");
  }
  if (now-lastStatus>=1000) {
    lastStatus=now;
    Serial.print("Z="); Serial.print(az,2);
    Serial.print(" A%="); Serial.print(sandA*100,0);
    Serial.print(" SHAKE="); Serial.print(disturbance,2);
    Serial.print(" STATE=");
    Serial.println(!started ? "WAIT" : (done ? "DONE" : "FLOWING"));
  }
}
