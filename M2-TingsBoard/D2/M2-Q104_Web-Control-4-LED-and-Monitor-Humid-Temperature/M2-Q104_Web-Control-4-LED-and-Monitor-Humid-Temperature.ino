// arduinojson 6.x
#include <WiFi.h>
#include <ThingsBoard.h>
#include "SHTC3.h"
#define WIFI_NAME "your-wifi-name"
#define WIFI_PASS "your-wifi-pass"
#define SERVER "demo.thingsboard.io"
#define DEVICE_TOKEN "your-device-token"

WiFiClient esp32Client;
int loopDaly = 20;
int sendDataDelay = 5000;
int counterSendData = 0;
ThingsBoard tb(esp32Client);

void setup() {
  Serial.begin(115200);
  WiFi.begin(WIFI_NAME, WIFI_PASS);
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void reconnectTB() {
  if(!tb.connect(SERVER, DEVICE_TOKEN)) {
    Serial.println("Failed to connect to ThingsBoard server");
  }
  else {
    Serial.println("Connected to ThingsBoard server");
  }
}

void reconnectWiFi() {
  WiFi.begin(WIFI_NAME, WIFI_PASS);
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  randomSeed(millis());
  Serial.println("\nWiFi Connected");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  if(WiFi.status() != WL_CONNECTED) reconnectWiFi();
  if(!tb.connected()) reconnectTB();
  tb.loop();

  if(counterSendData > sendDataDelay) {
    float t = random(2000, 4000)/100.0;//sht.readTempC();
    float h = random(6000, 8000)/100.0;//sht.readHumidity();
    if(t >= -40 && t <= 125) {
      tb.sendTelemetryFloat("temperature", t);    
      tb.sendTelemetryFloat("humidity", h);
      Serial.println("temperature: " + String(t) + ", humidity: " + String(h));
    }
    counterSendData = 0;
  }
  delay(loopDaly);
  counterSendData += loopDaly;
}
