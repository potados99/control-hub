#define BUZZER_PIN 4
#define SERIAL_BAUDRATE 9600
#define PARAM_MAX

String input;

void setup() {
  pinMode(BUZZER_PIN, OUTPUT);
  Serial.begin(SERIAL_BAUDRATE);
  Serial.setTimeout(0);

  Serial.println("Waiting for Raspberry Pi to send a signal...\n");
}

void loop() {
  if (Serial.available() > 0) {
    char recieved = Serial.read();
    if (recieved == '\n') {
        do_action(input);
	input = "";
        return;
    }

    input += recieved;
  }
}

void do_action(String incommingString) {
  if (incommingString == "") return;

  String commands[3];
  for (int i = 0; i < PARAM_MAX; ++ i) { commands[i] = split(incommingString, ' ', 0); }

  for (int i = 2; i >= 0; -- i) {
    if (commands[i] != "") {
       Serial.print(String(i+1) + " args.\n");
       beep(i + 1);
       return;
    }
  } /* end of for */

}


void beep(int howMany) {
  for (int i = 0; i < howMany; i ++) {
    on();
    delay(20);
    off();
    delay(100);
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
