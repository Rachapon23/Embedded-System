#define LED1 19 
#define LED2 21
#define SW1 22
#define SW2 23
bool LED_Status[] = {0, 0};

void setup() {
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(SW1, INPUT_PULLUP);
  pinMode(SW2, INPUT_PULLUP);
}

void loop() {
  if(digitalRead(SW1) == LOW) {
    delay(20);
    LED_Status[0] = 1 - LED_Status[0]; 
    digitalWrite(LED1, LED_Status[0]);
    while(digitalRead(SW1) == LOW) delay(20);
  }

  if(digitalRead(SW2) == LOW) {
    delay(20);
    LED_Status[1] = 1 - LED_Status[1]; 
    digitalWrite(LED2, LED_Status[1]);
    while(digitalRead(SW2) == LOW) delay(20);
  }

}
