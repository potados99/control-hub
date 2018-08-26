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
#define INTERRUPT_CODE 27 /* esc */
#define TERMINATE '\n'
#define TERMINATE_OPTIONAL ';'

#define BUZZER_CONTROL_PIN 2
#define LIT_BUTTON_PIN 19
#define LIT_CONTROL_PIN 4
#define LED_CONTROL_PIN 3
#define FAN_CONTROL_PIN 5

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

  if ((recieved == TERMINATE) || (recieved == TERMINATE_OPTIONAL)) {
    if (do_action(Input)) {
      Serial.write("T\n");
    }
    else {
      Serial.write("F\n");
    }
    Input = "";
    return; /* once LF came, return. */
  }

  Input += recieved; /* else, append recieved char to input */
}

void lit_button_input_task() {
  if (! digitalRead(LIT_BUTTON_PIN)) { /* Button is pushed */
    if ((~BtnSt & BTN_IS_PUSHED) && (~BtnSt & BTN_JUST_TOGGLED)) {
      toggle(LIT_CONTROL_PIN);
    }
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
  if (BuzCnt > 0) {
    if ((! BuzSt) & BUZ_IS_ON) { /* When buzzer is off */
      // Turn on buzzer
      if (millis() - BuzLastToggle > BUZ_OFF_TIME) {
        digitalWrite(BUZZER_CONTROL_PIN, HIGH);
        BuzSt |= BUZ_IS_ON;
        BuzLastToggle = millis();
      }
    }
    else { /* When buzzer is on */
      // Turn off buzzer
      if (millis() - BuzLastToggle > BUZ_ON_TIME) {
        digitalWrite(BUZZER_CONTROL_PIN, LOW);
        BuzSt &= ! BUZ_IS_ON;
        BuzLastToggle = millis();

        BuzCnt -= 1;
      }
    }
  }
  else {
    BuzCnt = 0;
  }
}
#endif


#ifdef HW_CONTROL_FUNCTIONS
bool do_action(String incommingString) {
  if (incommingString == "") return false;

  int argStart = 0;
  String commands[PARAM_MAX];
  for (int i = argStart; i < PARAM_MAX; ++ i) { commands[i] = split(incommingString, ' ', i); }

  if (commands[argStart] == "LIT") {
    return power_control(LIT_CONTROL_PIN, commands, argStart + 1);
  }
  else if (commands[argStart] == "LED") {
    return power_control(LED_CONTROL_PIN, commands, argStart + 1);
  }
  else if (commands[argStart] == "FAN") {
    return power_control(FAN_CONTROL_PIN, commands, argStart + 1);
  }
  else {
    return false;
  }
}

void beep(int howMany) {
  BuzCnt += howMany;
}

bool toggle(unsigned short pin) {
  if (read(pin)) {
    return off(pin);
  }
  else {
    return on(pin);
  }
}

bool on(unsigned short pin) {
  digitalWrite(pin, HIGH);
  beep(1);
  return (read(pin));
}

bool off(unsigned short pin) {
  digitalWrite(pin, LOW);
  beep(1);
  return (!read(pin));
}

bool read(unsigned short pin) {
  return (bool)bitRead(PORTD,pin);
}

bool power_control(unsigned short pin, String *args, int argStart) {
  if (args[argStart] == "") {
    beep(2);
    return false;
  }

  if (args[argStart] == "ON") {
    return on(pin);
  }
  else if (args[argStart] == "OFF") {
    return off(pin);
  }
  else if (args[argStart] == "RPD") {
    return rapid_toggle(pin, args, argStart + 1);
  }
  else if (args[argStart] == "BRT") {
    return pwm_control(pin, args, argStart + 1);
  }
  else if (args[argStart] == "SPD") {
    return pwm_control(pin, args, argStart + 1);
  }
  else {
    return false;
  }
}

bool pwm_control(unsigned short pin, String *args, int argStart) {
  if (args[argStart] == "") {
    beep(2);
    return false;
  }

  int inputInt = args[argStart].toInt(); /* 0 to 100 val */
  if ((inputInt < 0) || (inputInt > 100)) {
    beep(2);
    return false;
  }

  int pwmVal = inputInt * 2.55;

  analogWrite(pin, pwmVal);
  return true;
}

bool rapid_toggle(unsigned short pin, String *args, int argStart) {
  if (args[argStart] == "") {
    beep(2);
    return false;
  }

  int duration = args[argStart].toDouble() * 1000;
  bool originState = read(pin);

  unsigned long startTime = millis();

  for (;;) {
    digitalWrite(pin, HIGH);
    delay(10);
    digitalWrite(pin, LOW);
    delay(10);

    if (check_interrupt()) break;
    if (millis() - startTime > duration) break;
  }

  digitalWrite(pin, originState);

  return true;
}

bool check_interrupt() {
  if (! Serial.available()) return false;
  return Serial.read() == INTERRUPT_CODE;
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
