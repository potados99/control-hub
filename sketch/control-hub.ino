#define BUZZER_PIN 4
#define SERIAL_BAUDRATE 9600

void setup() {
  pinMode(BUZZER_PIN, OUTPUT);
  Serial.begin(SERIAL_BAUDRATE);
  Serial.setTimeout(0);
}

void loop() {
  if (Serial.available() > 0) {
    do_action((char)Serial.read());    
  }
}

void do_action(char incommingByte) {
  switch(incommingByte) {
      case '0':
        toggle();
        Serial.print("Toggle!\r\n");
        break;
        
      case '1':
        on();
        Serial.write("On!\r\n");
        break;
        
      case '2':
        off();
        Serial.write("Off!\r\n");
        break;

       case '3':
         rapidFire();
         break;
    }
}

void rapidFire() {
  for (int i = 0; i < 1000; ++ i) {
    digitalWrite(BUZZER_PIN, HIGH);
    Serial.print("YA! " + String(i) + " of " + String(1000) + "\r\n");
    digitalWrite(BUZZER_PIN, LOW);
    if (check_interrupt()) return;
    delay(15);
  }
}

void toggle() {
  if (digitalRead(BUZZER_PIN) == LOW) {
    digitalWrite(BUZZER_PIN, HIGH);
    //Serial.print("Turning ON!\r\n");
    return;
  }
  else {
    digitalWrite(BUZZER_PIN, LOW);
   //Serial.print("Turning OFF!\r\n");
   return;
  }
}

void on() {
  digitalWrite(BUZZER_PIN, HIGH);
}

void off() {
  digitalWrite(BUZZER_PIN, LOW);
}

bool check_interrupt() {
  return (char)Serial.read() == '4';
}




