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
#define INTERRUPT_CODE '4'

#define BUZZER_CONTROL_PIN 2
#define LIT_BUTTON_PIN 19
#define LIT_CONTROL_PIN 3
#define LED_CONTROL_PIN 4

#define SERIAL_BAUDRATE 9600
#define PARAM_MAX 3
#define BUZ_PREVENT_BOUNCE_TIME 120 /* millis*/

#define BTN_IS_PUSHED 0x01
#define BTN_JUST_TOGGLED 0x02

#define BUZ_IS_ON 0x01
#define BUZ_ON_TIME 15 /* millis */
#define BUZ_OFF_TIME 50
#endif


#ifdef GLOBAL_VARIABLES /* thread safe */
String Input; /* serial input */

char BtnSt = 0;
unsigned long BtnLastToggle = 0;

char BuzSt = 0;
int BuzCnt = 0;
unsigned long BuzLastToggle = 0;
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
  beep_task();
}
#endif


#ifdef TASK_FUNCTIONS
void serial_recieve_task() {
  if (! Serial.available()) return; /* nothing to do when nothing arrived. */

  char recieved = Serial.read();

  if (recieved == '\n') {
    do_action(Input);
    Input = "";
    return; /* once LF came, return. */
  }

  Input += recieved; /* else, append recieved char to input */
}

void lit_button_input_task() {
  if (! digitalRead(LIT_BUTTON_PIN)) { /* Button is pushed */
    if ((~BtnSt & BTN_IS_PUSHED) && (~BtnSt & BTN_JUST_TOGGLED))
      toggle(LIT_CONTROL_PIN);

    BtnSt |= BTN_IS_PUSHED; /* Add 0000 0001 */
    BtnSt |= BTN_JUST_TOGGLED; /* Add 0000 0010 */
    BtnLastToggle = millis();
    return;
  }
  else {
    BtnSt &= ~BTN_IS_PUSHED; /* Subtract 0000 0001 */
  }

  if (~BtnSt & BTN_JUST_TOGGLED) return;
  if (millis() - BtnLastToggle >= BUZ_PREVENT_BOUNCE_TIME) BtnSt &= ~BTN_JUST_TOGGLED;
}

void beep_task() {
  if (BuzCnt) {
    if ((! BuzSt) & BUZ_IS_ON) { /* When buzzer is off */
      // Turn on buzzer
      if (millis() - BuzLastToggle < BUZ_OFF_TIME) return;
      on(BUZZER_CONTROL_PIN);
      BuzSt |= BUZ_IS_ON;
    }
    else { /* When buzzer is on */
      // Turn off buzzer
      if (millis() - BuzLastToggle < BUZ_ON_TIME) return;
      off(BUZZER_CONTROL_PIN);
      BuzSt &= ! BUZ_IS_ON;
      BuzCnt -= 1;
    }
    BuzLastToggle = millis();
  }
}
#endif


#ifdef HW_CONTROL_FUNCTIONS
void do_action(String incommingString) {
  if (incommingString == "") return;

  String commands[PARAM_MAX];
  for (int i = 0; i < PARAM_MAX; ++ i) { commands[i] = split(incommingString, ' ', i); }

  switch (commands[0]) {
    case "LIT":
      power_control(LIT_CONTROL_PIN, commands[1]);
      break;
    case "LED":
      power_control(LED_CONTROL_PIN, commands[1]);
      break;
  }

/*
  for (int i = 2; i >= 0; -- i) {
    if (commands[i] != "") {
      // Serial.print(String(i+1) + " args.\n");
      beep(i + 1);
      Serial.write("T\n");
      return;
    }
  }
*/

}

void beep(int howMany) {
  BuzCnt += howMany;
}

void toggle(unsigned short pin) {
  digitalWrite(pin, digitalRead(pin) ? LOW : HIGH);
  beep(1);
}

void on(unsigned short pin) {
  digitalWrite(pin, HIGH);
}

void off(unsigned short pin) {
  digitalWrite(pin, LOW);
}

int read(unsigned short pin) {
  return digitalRead(pin);
}

void power_control(unsigned short pin, String command) {
  switch (command) {
    case "ON":
      on(pin);
      break;
    case "OFF":
      off(pin);
      break;
    case "":
      beep(1);
      break;
  }
}

bool check_interrupt() {
  if (! Serial.available()) return false;
  return (char)Serial.read() == INTERRUPT_CODE;
}
#endif


#ifdef HELPER_FUNCTIONS
String split(String data, char separator, int index) {
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
