//
//  control-hub.ino
//  control/sketch
//
//  Created by POTADOS on 2018. 8. 24.
//  Copyright © 2018 POTADOS. All rights reserved.
//
//


///////// Compile options //////////
#define CONSTANTS
#define GLOBAL_VARIABLES
#define DEFAULT_FUNCTIONS
#define TASK_FUNCTIONS
#define HW_CONTROL_FUNCTIONS
#define HELPER_FUNCTIONS
////////////////////////////////////


#ifdef CONSTANTS
#define LIT_BUTTON_PIN 2 /* D2 */
#define LIT_CONTROL_PIN LED_BUILTIN /* D3 */
#define BUZZER_CONTROL_PIN 4 /* D4 */
#define LED_CONTROL_PIN 5 /* D3 */

#define SERIAL_BAUDRATE 9600
#define PARAM_MAX 3
#define IGNORE_UNTIL 50000 /* ignore toggling for (10/1000) * 30 seconds */
#endif


#ifdef GLOBAL_VARIABLES
String input;
bool isPushed = false;
bool justToggled = false;
unsigned int elapsedAfterToggle = 0;
#endif


#ifdef DEFAULT_FUNCTIONS
void setup() {
  pinMode(LIT_BUTTON_PIN, INPUT);
  digitalWrite(LIT_BUTTON_PIN, HIGH);
  pinMode(LIT_CONTROL_PIN, OUTPUT);
  pinMode(BUZZER_CONTROL_PIN, OUTPUT);
  pinMode(LED_CONTROL_PIN, OUTPUT);

  Serial.begin(SERIAL_BAUDRATE);
  Serial.setTimeout(0);

  Serial.println("Waiting for Raspberry Pi to send a signal...\n");
}

void loop() {
  serial_recieve_task();
  lit_button_input_task();
}
#endif

#ifdef TASK_FUNCTIONS
void serial_recieve_task() {
  if (! Serial.available()) return; /* nothing to do when nothing arrived. */

  char recieved = Serial.read();

  if (recieved == '\n') {
    do_action(input);
    input = "";
    return; /* once LF came, return. */
  }

  input += recieved; /* else, append recieved char to input */
}

void lit_button_input_task() {
  // Fundamental behavior
  if (! digitalRead(LIT_BUTTON_PIN)) { /* Button is pushed */
    if (isPushed == false) {
      if (justToggled == false) {
        toggle(LIT_CONTROL_PIN);
        justToggled = true;
      }
    }
    isPushed = true;
  }
  else {
    isPushed = false;
  }

  if (justToggled == true) {
    elapsedAfterToggle ++;

    if (elapsedAfterToggle >= IGNORE_UNTIL) {
      justToggled = false;
      elapsedAfterToggle = 0;
    }
  }
}
#endif


#ifdef HW_CONTROL_FUNCTIONS
void do_action(String incommingString) {
  if (incommingString == "") return;

  String commands[PARAM_MAX];
  for (int i = 0; i < PARAM_MAX; ++ i) { commands[i] = split(incommingString, ' ', i); }

  for (int i = 2; i >= 0; -- i) {
    if (commands[i] != "") {
      // Serial.print(String(i+1) + " args.\n");
      beep(i + 1);
      Serial.write("T\n");
      return;
    }
  } /* end of for */

}

void beep(int howMany) {
  for (int i = 0; i < howMany; i ++) {
    on();
    delay(10);
    off();
    delay(50);
  }
}

void toggle(int pin) {
  digitalWrite(pin, digitalRead(pin) ? LOW : HIGH);
  beep(1);
}

void on() {
  digitalWrite(BUZZER_CONTROL_PIN, HIGH);
}

void off() {
  digitalWrite(BUZZER_CONTROL_PIN, LOW);
}

bool check_interrupt() {
  return (char)Serial.read() == '4';
}
#endif


#ifdef HELPER_FUNCTIONS
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
#endif
