#include <WiFi.h>
#include <Wire.h>
#include <ThingsBoard.h>
#include "SHTC3.h"
#define WIFI_NAME "your-wifi-name"
#define WIFI_PASS "your-wifi-pass"
#define SERVER "demo.thingsboard.io"
#define DEVICE_TOKEN "your-device-token"
#define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x]))))) 
#define LED_BLINK 2
bool LED_Blink_Status = 0;
SHTC3 sht(Wire);
WiFiClient esp32Client;
byte LED[] = {19, 21, 22, 23};
bool LED_Status[] = {0, 0, 0, 0};
int loopDaly = 20;
int BlinkLEDdelay = 500;
int counterBlinkLEDdelay = 0;
int sendDataDelay = 5000;
int counterSendData = 0;
ThingsBoard tb(esp32Client);
#include "_ThingBoardRPC.h"
bool subscribeStatus = 0;
char StringEcho[] = "stsLED_1";

void setup() {
  Serial.begin(115200);
  Wire.begin(15, 18);
  pinMode(LED_BLINK, OUTPUT);
  for(int i = 0 ; i < 4 ; i++) {
    pinMode(LED[i], OUTPUT);
  }
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

void subscribeForRPC() {
  Serial.println("Subscribing for RPC...");
  if(!tb.RPC_Subscribe(callbacks, COUNT_OF(callbacks))) {
    Serial.println("Failed to subscribing for RPC");
  }
  else {
    Serial.println("Subscribed for RPC");
    subscribeStatus = 1;
  }
}

void loop() {
  if(WiFi.status() != WL_CONNECTED) reconnectWiFi();
  if(!tb.connected()) {
    subscribeStatus = 0;
    reconnectTB();
  }
  if(!subscribeStatus) subscribeForRPC();
  tb.loop();

  if(counterBlinkLEDdelay > BlinkLEDdelay) {
    digitalWrite(LED_BLINK, LED_Blink_Status);
    LED_Blink_Status = 1 - LED_Blink_Status;
    counterBlinkLEDdelay = 0;
  }

  
  if(counterSendData > sendDataDelay) {
    sht.begin();
    sht.sample();
    float t = sht.readTempC();//random(00, 50);//
    float h = sht.readHumidity();//random(51, 99);//
    if(t >= -40 && t <= 125) {
      tb.sendTelemetryFloat("temperature", t);    
      tb.sendTelemetryFloat("humidity", h);
      Serial.println("temperature: " + String(t) + ", humidity: " + String(h));
    }
    Serial.print("LED="); 
    for(size_t i = 0 ; i < COUNT_OF(LED); i++) {
      StringEcho[7] = 0x30 + i;
      tb.sendTelemetryInt(StringEcho, LED_Status[i]);
       
      Serial.print(LED_Status[i]); 
    }
    Serial.println();
    counterSendData = 0;
  }
  delay(loopDaly);
  counterSendData += loopDaly;
  counterBlinkLEDdelay += loopDaly;
}
