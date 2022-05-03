#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include "SHTC3.h"
#define WIFI_NAME "your-wifi-name"
#define WIFI_PASS "your-wifi-pass"
#define MQTT_SERVER "mqtt.eclipseprojects.io"
#define MQTT_PORT 1883
#define TOPIC1 "LED1_SxWlBVzDd2ufJxmIWngGzwl9MSD"
#define TOPIC2 "LED2_os28V4zQSGQOAdRTIwQJdWna2Ws"
#define TOPIC3 "LED3_IgoItb5llzdm3SQ9T2a2Vim4xHq"
#define TOPIC4 "LED4_JO4ZcZCQL0rBTsPFEosIWtQ2LrA"
#define TOPIC5 "Temp_CPQUl7Vv8AzQ0QaX1AopwhgZJMe"
#define TOPIC6 "Humid_ORic62VRNxGbKOmzgDVAICl0S5q"
#define TOPIC7 "Alarm1_AzBAF6LbSmcrUExityukWg6MEHe"
#define TOPIC8 "Alarm2_QMGLPksPUMerF7E8neUVKxIrng3"

byte LED[] = {19, 21, 22, 23};
byte button[] = {4, 5};

SHTC3 sht(Wire);
WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  Wire.begin(15, 18);
  for(int i = 0 ; i < 4 ; i++) {
    if(i < 2) pinMode(button[i], INPUT_PULLUP);
    pinMode(LED[i], OUTPUT);  
  }
  WiFi.begin(WIFI_NAME, WIFI_PASS);
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  randomSeed(micros());
  Serial.println("\nWiFi Connected");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  client.setServer(MQTT_SERVER, MQTT_PORT);
  client.setCallback(controlLED);
  xTaskCreatePinnedToCore(
    alarm,    // Task function
    "alarm",  // name of task
    1000,     // Stack size of task
    NULL,     // parameter of task
    1,        // priority od the task
    NULL,     // Task handle to keep track of created task
    1         // pin task to core 1
  );
}

void reconnect() {
  while(!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientID = "ESP32 Clinet-";
    clientID += String(random(0xffff), HEX);

    if(client.connect(clientID.c_str())) {
      Serial.println("connected");
      client.subscribe(TOPIC1);
      client.subscribe(TOPIC2);
      client.subscribe(TOPIC3);
      client.subscribe(TOPIC4);
    }
    else {
      Serial.print("\nfailed, rc=");
      Serial.print(client.state());
      Serial.println("\ntry again in 5 seconds");
      delay(5000);
    }
  }
}

void alarm(void *pvParameterrs) {
  while(true) {
    if(digitalRead(button[0]) == LOW) {
      vTaskDelay(20/portTICK_PERIOD_MS);
      client.publish(TOPIC7, "Overheat Alarm");
      Serial.println("Alarm: Overheat Alarm");
      while(digitalRead(button[0]) == LOW) vTaskDelay(20/portTICK_PERIOD_MS);
    }
    if(digitalRead(button[1]) == LOW) {
      vTaskDelay(20/portTICK_PERIOD_MS);
      client.publish(TOPIC8, "Intruders Alarm");
      Serial.println("Alarm: Intruders Alarm");
      while(digitalRead(button[1]) == LOW) vTaskDelay(20/portTICK_PERIOD_MS);
    }
  }
}

void controlLED(char *topic, byte *payload, unsigned int length) {
  char localPayload[length + 1];
  Serial.println("Message arrived ["+String(topic)+"]");
  Serial.print("-> ");
  for(int i = 0 ; i < length ; i++) {
    localPayload[i] = (char)payload[i];
    localPayload[i+1]  = '\0';
    Serial.print(localPayload[i]);
  }
  Serial.println();

  if(String(topic) == String(TOPIC1)) {
    if(String(localPayload) == "ON") digitalWrite(LED[0], HIGH);
    if(String(localPayload) == "OFF") digitalWrite(LED[0], LOW);
  }
  if(String(topic) == String(TOPIC2)) {
    if(String(localPayload) == "ON") digitalWrite(LED[1], HIGH);
    if(String(localPayload) == "OFF") digitalWrite(LED[1], LOW);
  }
  if(String(topic) == String(TOPIC3)) {
    if(String(localPayload) == "ON") digitalWrite(LED[2], HIGH);
    if(String(localPayload) == "OFF") digitalWrite(LED[2], LOW);
  }
  if(String(topic) == String(TOPIC4)) {
    if(String(localPayload) == "ON") digitalWrite(LED[3], HIGH);
    if(String(localPayload) == "OFF") digitalWrite(LED[3], LOW);
  }
}

void loop() {
  if(!client.connected()) reconnect();
  client.loop();
  sht.begin();
  sht.sample();
  float t = sht.readTempC();
  float h = sht.readHumidity();
  if(t >= -40 && t <= 125) {
    client.publish(TOPIC5, String(t).c_str());    
    client.publish(TOPIC6, String(h).c_str());
    Serial.println("temperature: " + String(t) + ", humidity: " + String(h));
  }
  delay(5000);
}
