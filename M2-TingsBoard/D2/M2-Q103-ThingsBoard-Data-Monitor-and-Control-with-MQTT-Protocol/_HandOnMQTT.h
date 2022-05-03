// file name: _HandOnMQTT.h
String get_gpio_status() {
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject & data = jsonBuffer.createObject();
  data[String(LED[0])] = LED_Status[0];
  data[String(LED[1])] = LED_Status[1];
  data[String(LED[2])] = LED_Status[2];
  data[String(LED[3])] = LED_Status[3];
  char payload[256];
  data.printTo(payload, sizeof(payload));
  String strPayload = String(payload);
  Serial.print("Get GPIO Status: ");
  Serial.println(strPayload);
  return strPayload;
}

String getRandomValue() {
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject & data = jsonBuffer.createObject();
  data["randomValue"] = random(0, 50);
  char payload[256];
  data.printTo(payload, sizeof(payload));
  String strPayload = String(payload);
  Serial.print("Get Random Value: ");
  Serial.println(strPayload);
  return strPayload;
}

void set_gpio_status(int pin, boolean enabled) {
  if (pin == LED[0]) {
    LED_Status[0] = 1 - LED_Status[0];
    digitalWrite(LED[0], LED_Status[0]);
  }
  if (pin == LED[1]) {
    LED_Status[1] = 1 - LED_Status[1];
    digitalWrite(LED[1], LED_Status[1]);
  }
  if (pin == LED[2]) {
    LED_Status[2] = 1 - LED_Status[2];
    digitalWrite(LED[2], LED_Status[2]);
  }
  if (pin == LED[3]) {
    LED_Status[3] = 1 - LED_Status[3];
    digitalWrite(LED[3], LED_Status[3]);
  }
}

void on_message(const char* topic, byte* payload, unsigned int length) {
  Serial.println("\nOn message");
  char json[length + 1];
  strncpy (json, (char*)payload, length);
  json[length] = '\0';
  Serial.print("Topic: "); Serial.println(topic);
  Serial.print("Message: "); Serial.println(json);
  // Decode JSON request
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& data = jsonBuffer.parseObject((char*)json);
  if (!data.success()) {
    Serial.println("parseObject() failed");
    return;
  }
  
  String methodName = String((const char*)data["method"]);

  if (methodName.equals("getGpioStatus")) {
    String responseTopic = String(topic);
    responseTopic.replace("request", "response");
    client.publish(responseTopic.c_str(), get_gpio_status().c_str());
  }

  if (methodName.equals("setGpioStatus")) {
    set_gpio_status(data["params"]["pin"], data["params"]["enabled"]);
    String responseTopic = String(topic);
    responseTopic.replace("request", "response");
    client.publish(responseTopic.c_str(), get_gpio_status().c_str());
    client.publish("v1/devices/me/attributes", get_gpio_status().c_str());
  }
}
