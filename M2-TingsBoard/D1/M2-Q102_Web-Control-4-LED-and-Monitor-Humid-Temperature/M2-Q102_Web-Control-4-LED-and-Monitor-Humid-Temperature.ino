#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <Wire.h>
#include "SHTC3.h"
#include "index.h" //Our HTML webpage contents with javascripts 
#define WIFI_NAME "your-wifi-name"
#define WIFI_PASS "your-wifi-pass"

WebServer server(80); //Server on port 80
SHTC3 sht(Wire);
byte LED[] = {5, 18, 19, 21};
String LED_State[] = {"NA", "NA", "NA", "NA"};

void handleRoot() {
  String s = MAIN_page; //Read HTML contents
  server.send(200, "text/html", s); //Send web page
}

void handleADC() {
  sht.begin(); 
  sht.sample();
  float h = sht.readHumidity();
  float t = sht.readTempC();
  if(t < -40) return;
  String tmpValue = "Temp = ";
  tmpValue += String(t) + " C, Humidity = ";
  tmpValue += String(h) + " %";
  server.send(200, "text/plane", tmpValue); //Send value to client ajax request
}

void handleLED() {
  String led_state = server.arg("LEDstate"); //Refer xhttp.open("GET", "setLED?LEDstate="+led, true);
  Serial.println(led_state);
  if (led_state == "11") {
    digitalWrite(LED[0], HIGH);  //Feedback parameter
    LED_State[0] = "ON";
  }
  if (led_state == "10") {
    digitalWrite(LED[0], LOW);  //Feedback parameter
    LED_State[0] = "OFF";
  }
  if (led_state == "21") {
    digitalWrite(LED[1], HIGH);  //Feedback parameter
    LED_State[1] = "ON";
  }
  if (led_state == "20") {
    digitalWrite(LED[1], LOW);  //Feedback parameter
    LED_State[1] = "OFF";
  }
  if (led_state == "31") {
    digitalWrite(LED[2], HIGH);  //Feedback parameter
    LED_State[2] = "ON";
  }
  if (led_state == "30") {
    digitalWrite(LED[2], LOW);  //Feedback parameter
    LED_State[2] = "OFF";
  }
  if (led_state == "41") {
    digitalWrite(LED[3], HIGH);  //Feedback parameter
    LED_State[3] = "ON";
  }
  if (led_state == "40") {
    digitalWrite(LED[3], LOW);  //Feedback parameter
    LED_State[3] = "OFF";
  }
  server.send(200, "text/plane", LED_State[0]+", "+LED_State[1]+", "+LED_State[2]+", "+LED_State[3]); //Send web page
}

void getLEDstate() {
  server.send(200, "text/plane", LED_State[0]+", "+LED_State[1]+", "+LED_State[2]+", "+LED_State[3]);
}

void setup() {
  Serial.begin(115200);
  Wire.begin(22, 23);
  for(int i = 0 ; i < 4 ; i++) {
    pinMode(LED[i], OUTPUT);
  }
  Serial.print("\n\nConnect to ");
  Serial.println(WIFI_NAME);
  WiFi.begin(WIFI_NAME, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
  }
  Serial.print("\nConnected "); 
  Serial.println(WIFI_NAME);
  Serial.print("IP address: "); 
  Serial.println(WiFi.localIP());
  
  server.on("/", handleRoot);
  server.on("/setLED", handleLED);
  server.on("/readADC", handleADC);
  server.on("/getLED", getLEDstate);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient(); //Handle client requests
}
