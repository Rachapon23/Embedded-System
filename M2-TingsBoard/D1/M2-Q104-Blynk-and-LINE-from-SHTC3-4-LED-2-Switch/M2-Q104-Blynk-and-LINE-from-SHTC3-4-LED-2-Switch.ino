#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <Wire.h>
#include <TridentTD_LineNotify.h>
#include <BlynkSimpleEsp32.h>
#include "SHTC3.h"
#define WIFI_NAME "your-wifi-name"
#define WIFI_PASS "your-wifi-pass"
#define BLYNK_TOKEN "your-blynk-token"
#define LINE_TOKEN "your-line-token"
#define GSS_TOKEN "your-GSS-token"
#define GSS_SHEET "Sensor_Data"
#define HOST "script.google.com"
#define HOST_PORT 443

byte LED[] = {19, 21, 22, 23};
byte button[] = {4, 5};
float temperature = -45;
float humidity = 0;
SHTC3 sht(Wire);
WiFiClientSecure client;

void setup() {
  Serial.begin(115200);
  Wire.begin(15, 18);
  for(int i = 0 ; i < 4 ; i++) {
    if(i < 2) pinMode(button[i], INPUT_PULLUP);
    pinMode(LED[i], OUTPUT);  
  }
  Blynk.begin(BLYNK_TOKEN, WIFI_NAME, WIFI_PASS);
  LINE.setToken(LINE_TOKEN);
  client.setInsecure();
  xTaskCreatePinnedToCore(
    readSensorData,    // Task function
    "readSensorData",  // name of task
    1000,     // Stack size of task
    NULL,     // parameter of task
    1,        // priority od the task
    NULL,     // Task handle to keep track of created task
    1         // pin task to core 1
  );
  xTaskCreatePinnedToCore(
    sendSensorData,    // Task function
    "sendSensorData",  // name of task
    5000,     // Stack size of task
    NULL,     // parameter of task
    1,        // priority od the task
    NULL,     // Task handle to keep track of created task
    1         // pin task to core 1
  );
  xTaskCreatePinnedToCore(
    sensorAlarm,    // Task function
    "sensorAlarm",  // name of task
    5000,     // Stack size of task
    NULL,     // parameter of task
    1,        // priority od the task
    NULL,     // Task handle to keep track of created task
    1         // pin task to core 1
  );
  xTaskCreatePinnedToCore(
    switchOverHeatAlarm,    // Task function
    "switchAlarm",  // name of task
    5000,     // Stack size of task
    NULL,     // parameter of task
    1,        // priority od the task
    NULL,     // Task handle to keep track of created task
    1         // pin task to core 1
  );
  xTaskCreatePinnedToCore(
    switchIntrudersAlarm,    // Task function
    "switchIntrudersAlarm",  // name of task
    5000,     // Stack size of task
    NULL,     // parameter of task
    1,        // priority od the task
    NULL,     // Task handle to keep track of created task
    1         // pin task to core 1
  );
}

void readSensorData(void *pvParameterrs) {
  while(true) {
    sht.begin();
    sht.sample();
    temperature = sht.readTempC();
    humidity = sht.readHumidity();
    vTaskDelay(1000/portTICK_PERIOD_MS);
  }
}

void sensorAlarm(void *pvParameterrs) {
  while(true) {
    if(temperature > 28) {
      LINE.notify("\nTemperature is over 28 *C !\nCurrent temperature -> "+String(temperature)+" *C");  
      Serial.println("\nTemperature is over 28 *C !");
      Serial.println("Sending notification to line\n");
      vTaskDelay(30000/portTICK_PERIOD_MS);
    }
    vTaskDelay(1000/portTICK_PERIOD_MS);
  }
}

void switchOverHeatAlarm(void *pvParameterrs) {
  while(true) {
    if(digitalRead(button[0]) == LOW) {
      vTaskDelay(20/portTICK_PERIOD_MS);
      LINE.notify("\nOverheat Alarm");
      Serial.println("Alarm: Overheat Alarm!!!");
      while(digitalRead(button[0]) == LOW) vTaskDelay(20/portTICK_PERIOD_MS);
    }
  }
}

void switchIntrudersAlarm(void *pvParameterrs) {
  while(true) {
    if(digitalRead(button[1]) == LOW) {
      vTaskDelay(20/portTICK_PERIOD_MS);
      LINE.notify("\nIntruders Alarm");
      Serial.println("Alarm: Intruders Alarm!!!");
      while(digitalRead(button[1]) == LOW) vTaskDelay(20/portTICK_PERIOD_MS);
    }
  }
}

void logSensorData() {
  if(!client.connect(HOST, HOST_PORT)) return;
  
  String strURL;
  strURL += "/macros/s/" + String(GSS_TOKEN) + "/exec?"; 
  strURL += "id=" + String(GSS_SHEET); 
  strURL += "&Temperature=" + String(temperature); 
  strURL += "&Humidity=" + String(humidity); 
  Serial.print("requesting URL: "); Serial.println(strURL); 

  client.print(
    String("GET ") + strURL + " HTTP/1.1\r\n" + 
    "Host: " + String(HOST) + "\r\n" + 
    "User-Agent: BuildFailureDetectorESP8266\r\n" + 
    "Connection: close\r\n\r\n"
  ); 
  Serial.println("request sent"); 

  while (client.connected()) { 
     String line = client.readStringUntil('\n'); 
     if (line == "\r") {
      Serial.println("headers received"); 
      break; 
     } 
  } 
  
  String line = client.readStringUntil('\n'); 
  
  Serial.print("reply was : "); 
  Serial.println(line); 
  Serial.println("closing connection");
  client.stop(); 
  Serial.println();
}

void sendSensorData(void *pvParameterrs) {
  while(true) {
    if(temperature >= -40 && temperature <= 125) {
      logSensorData();
      Blynk.virtualWrite(V0, temperature);
      Blynk.virtualWrite(V1, humidity);
      Serial.println("Temp(*C) >> "+String(temperature));
      Serial.println("Humidity(%) >> "+String(humidity)+"\n");
    }
    vTaskDelay(5000/portTICK_PERIOD_MS);
  }
}

void loop() {
  Blynk.run();
}
