// file name: _ThingBoardRPC.h
RPC_Response setDelay(const RPC_Data &data) {
  Serial.println("Received request set delay");
  BlinkLEDdelay = data;
  Serial.print("Set new delay: ");
  Serial.print(BlinkLEDdelay);
  return RPC_Response(NULL, BlinkLEDdelay);
}

RPC_Response getDelay(const RPC_Data &data) {
  Serial.println("Received requset get delay");
  return RPC_Response(NULL, BlinkLEDdelay);
}

RPC_Response setLED(const RPC_Data &data) {
  Serial.println("Received requset set LED");
  byte pin = data["pin"];
  bool enable = data["enable"];
  if(pin < COUNT_OF(LED)) {
     Serial.print("Setting LED" );
     Serial.print(pin);
     Serial.print("to state");
     Serial.print(LED_Status[pin]);
     LED_Status[pin] = 1 - LED_Status[pin];
     digitalWrite(LED[pin], LED_Status[pin]);
  }
  return RPC_Response(data["pin"], (bool)data["enable"]);
}

RPC_Callback callbacks[] = {
  {"setValue", setDelay},
  {"getValue", getDelay},
  {"setGpioStatus", setLED}
};
