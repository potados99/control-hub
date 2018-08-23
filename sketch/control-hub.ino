#define BUZZER_PIN 4
#define SERIAL_BAUDRATE 9600

String input;

void setup() {
  pinMode(BUZZER_PIN, OUTPUT);
  Serial.begin(SERIAL_BAUDRATE);
  Serial.setTimeout(0);
}

void loop() {
  if (Serial.available() > 0) {
    input = Serial.readStringUntil("\n");
    do_action(input);
  }
}

void do_action(String incommingString) {
   String commands[3];
   commands[0] = split(incommingString, ' ', 0);
   commands[1] = split(incommingString, ' ', 1);
   commands[2] = split(incommingString, ' ', 2);

   for (int i = 0; i < 3; ++ i) {
     if (commands[i] == "") {
       Serial.print(String(i) + " args.");
       return;
     }
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


String split(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }

  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}
