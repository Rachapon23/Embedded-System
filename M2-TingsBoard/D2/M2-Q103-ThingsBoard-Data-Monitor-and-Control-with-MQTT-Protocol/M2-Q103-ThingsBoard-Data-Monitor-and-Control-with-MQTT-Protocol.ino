#include <WiFi.h>
#include <Wire.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "SHTC3.h"
#define WIFI_NAME "sontaya7"
#define WIFI_PASS "123456789"
#define SERVER "demo.thingsboard.io"
#define DEVICE_NAME "M2-Q103"
#define DEVICE_TOKEN "mQAh4pixVTYEoN9M8OCX"

SHTC3 sht(Wire);
WiFiClient esp32Client;
PubSubClient client(esp32Client);
byte LED[] = {19, 21, 22, 23};
bool LED_Status[] = {0, 0, 0, 0};
#include "_HandOnMQTT.h"

void setup() {
  Serial.begin(115200);
  Wire.begin(15, 18);
  for(int i = 0 ; i < 4 ; i++) {
    pinMode(LED[i], OUTPUT);
  }
  WiFi.begin(WIFI_NAME, WIFI_PASS);
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  randomSeed(millis());
  Serial.println("\nWiFi Connected");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP()); 
  client.setServer(SERVER, 1883); 
  client.setCallback(on_message);
  xTaskCreatePinnedToCore(
    sendRandomValue,    // Task function
    "sendRandomValue",  // name of task
    2000,     // Stack size of task
    NULL,     // parameter of task
    1,        // priority od the task
    NULL,     // Task handle to keep track of created task
    1         // pin task to core 1
  );
  
}

void sendRandomValue(void *pvParameterrs) {
  while(true) {
    client.publish("v1/devices/me/telemetry", getRandomValue().c_str());
    vTaskDelay(5000/portTICK_PERIOD_MS);
  }
}

void reconnectTB() {
  while(!client.connected()) {
    if(client.connect(DEVICE_NAME, DEVICE_TOKEN, NULL)) {
      Serial.println( "[DONE]" ); 
      client.subscribe("v1/devices/me/rpc/request/+");
      client.publish("v1/devices/me/attributes", get_gpio_status().c_str());
    }
    else {
      Serial.print("[FAILED] [ rc = "); 
      Serial.print(client.state()); 
      Serial.println(" : retrying in 5 seconds]"); 
      delay(5000); 
    }
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
  if(!client.connected()) reconnectTB();
  client.loop();
}
