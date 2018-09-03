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
#define STRUCT_DEFINITIONS
#define GLOBAL_VARIABLES
#define DEFAULT_FUNCTIONS
#define TASK_FUNCTIONS
#define HW_CONTROL_FUNCTIONS
#define HELPER_FUNCTIONS
#define INITIAL_SETUP
////////////////////////////////////

#ifdef CONSTANTS
// Serial
#define SERIAL_BAUDRATE 9600
#define INTERRUPT_CODE 27 /* esc */
#define TERMINATE '\n'
#define TERMINATE_OPTIONAL ';'
#define PARAM_MAX 3

// Pin numbers
#define BUZ_CONTROL_PIN 2
#define LIT_BUTTON_PIN 19 /* A5 */
#define LIT_CONTROL_PIN 4
#define LED_CONTROL_PIN 3
#define FAN_CONTROL_PIN 5

// Constants
#define NUMBER_OF_DEVICES 3
#define PWM_VAL_RATE 2.55
#define RAPID_DELAY 50 /* Millis */
#define FADE_SPEED 1

// Button flags
#define LIT_BTN_MODE INPUT_PULLUP
#if LIT_BTN_MODE == INPUT_PULLUP
#define LIT_BTN_ACTIVE LOW /* Active low when pull-up. */
#else
#define LIT_BTN_ACTIVE HIGH /* Active low when pull-down or anything. */
#endif
#define BTN_IS_PUSHED 0x01
#define BTN_JUST_TOGGLED 0x02
#define BTN_PREVENT_BOUNCE_TIME 120 /* millis*/

// Buzzer flags
#define BUZ_IS_ON 0x01
#define BUZ_ON_TIME 15 /* millis */
#define BUZ_OFF_TIME 50
#endif


#ifdef STRUCT_DEFINITIONS
typedef struct Device {
  String name = "";
  char pin = 0;
  char pwmVal = 100;
  bool power = false;
} Device;

typedef struct Button {
  char pin = 0;
  char states = 0;
  char pinMode = 0;
  char pinActive = 0;
  unsigned long lastToggle = 0;
} Button;

typedef struct Notifier {
  char pin = 0;
  char states = 0;
  unsigned int countRemain = 0;
  unsigned long lastToggle = 0;
} Notifier;
#endif


#ifdef GLOBAL_VARIABLES /* thread safe */
String Input; /* serial input */

Device *DeviceArray[NUMBER_OF_DEVICES];
Device LitDevice;
Device LedDevice;
Device FanDevice;

Button LitButton;
Notifier Buzzer;

bool Alarm = false;
bool Feedback = false;
#endif


#ifdef DEFAULT_FUNCTIONS
void setup() {
  initial_device_setup();
  initial_pin_setup();
  initial_serial_setup();
}

void loop() {
  serial_task();
  button_task(&LitButton, &LitDevice);
  beep_task(&Buzzer);
}
#endif


#ifdef TASK_FUNCTIONS
void serial_task() {
  if (! Serial.available()) return; /* nothing to do when nothing arrived. */

  char recieved = Serial.read();

  if ((recieved == TERMINATE) || (recieved == TERMINATE_OPTIONAL)) {
    Input.toUpperCase();
    if (do_action(Input)) {
      if (Feedback) Serial.write("T\n");
    }
    else {
      if (Feedback) Serial.write("F\n");
    }

    Input = "";
    Feedback = true;

    return; /* once LF came, return. */
  }

  Input += recieved; /* else, append recieved char to input */
}

void button_task(Button *button, Device *device) {
  if (digitalRead(device->pin) == button->pinActive) { /* Button is pushed */
    if ((~button->states & BTN_IS_PUSHED) && (~button->states & BTN_JUST_TOGGLED)) {
      toggle(device);
    }
    button->states |= BTN_IS_PUSHED; /* Add 0000 0001 */
    button->states |= BTN_JUST_TOGGLED; /* Add 0000 0010 */
    button->lastToggle = millis();
    return;
  }
  else {
    button->states &= ~BTN_IS_PUSHED; /* Subtract 0000 0001 */
  }

  if (~button->states & BTN_JUST_TOGGLED) return;
  if (millis() - button->lastToggle >= BTN_PREVENT_BOUNCE_TIME) button->states &= ~BTN_JUST_TOGGLED;
}

void beep_task(Notifier *notifier) {
  static bool alarmWas = false;
  if (Alarm) {
    // When alarm just turned on.
    if (alarmWas == false) {
      digitalWrite(notifier->pin, HIGH);  
      notifier->states |= BUZ_IS_ON;
    }
    alarmWas = true;
    return;
  }
  else {
    // When alarm just turned off.
    if (alarmWas == true) {
      digitalWrite(notifier->pin, LOW);
      notifier->states &= ~BUZ_IS_ON;
    }
    alarmWas = false;
  }

  if (notifier->countRemain > 0) {
    if ((~notifier->states) & BUZ_IS_ON) { /* When buzzer is off */
      // Turn on buzzer
      if (millis() - notifier->lastToggle > BUZ_OFF_TIME) {
        digitalWrite(notifier->pin, HIGH);
        notifier->states |= BUZ_IS_ON;
        notifier->lastToggle = millis();
      }
    }
    else { /* When buzzer is on */
      // Turn off buzzer
      if (millis() - notifier->lastToggle > BUZ_ON_TIME) {
        digitalWrite(notifier->pin, LOW);
        notifier->states &= ~BUZ_IS_ON;
        notifier->lastToggle = millis();
        notifier->countRemain -= 1;
      }
    }
  }
  else {
    notifier->countRemain = 0;
  }
}
#endif


#ifdef HW_CONTROL_FUNCTIONS
bool do_action(String incommingString) {
  if (incommingString == "") return error(3);

  String commands[PARAM_MAX];
  for (unsigned register short i = 0; i < PARAM_MAX; ++ i) { commands[i] = split(incommingString, ' ', i); }

  // Direct device control
  for (unsigned register short i = 0; i < NUMBER_OF_DEVICES; ++ i) {
    if (*commands == DeviceArray[i]->name) {
      return device_control(DeviceArray[i], commands+1);
    }
  }

  if (*commands == "ALM") {
    return alarm_control(commands+1);
  }

  return error(2);
}

bool device_control(Device *device, String *args) {
  if (*args == "") return error(3);

  if (*args == "ON") {
    return power_control(device, true);
  }
  else if (*args == "OFF") {
    return power_control(device, false);
  }
  else if (*args == "RPD") {
    return rapid_toggle(device, args+1);
  }
  else if (*args == "BRT") { // PWM only
    return pwm_control(device, args+1);
  }
  else if (*args == "SPD") { // PWM only
    return pwm_control(device, args+1);
  }
  else if (*args == "FADE") {
    return fade_control(device, args+1);
  }
  else if (*args == "ST") {
    return status_return(device, args+1);
  }
  else {
    return error(2);
  }
}

bool power_control(Device *device, bool power) {
  analogWrite(device->pin, (device->power = power) ? (device->pwmVal * PWM_VAL_RATE) : 0);
  if (device->pwmVal == 100) beep(1);
  return (device->power == power);
}

bool pwm_control(Device *device, String *args) {
  if (*args == "") return error(3);

  int inputInt = (*args).toInt(); /* 0 to 100 val */
  if ((inputInt < 0) || (inputInt > 100)) return error(2);

  device->pwmVal = inputInt;

  if (device->power) {
    return (device->pwmVal == inputInt) && power_control(device, true);
  }

  return (device->pwmVal == inputInt);
}

bool fade_control(Device *device, String *args) {
  // synchronous function. other tasks need to wait.
  if (*args == "") return error(3);

  if (*args == "IN") {
    for(register short i = 0; i <= device->pwmVal; i += FADE_SPEED) {
      if (i > device->pwmVal) {
        analogWrite(device->pin, device->pwmVal * PWM_VAL_RATE);
        break;
      }
      analogWrite(device->pin, i * PWM_VAL_RATE);
      delay(1);
    }
    device->power = true;
  }
  else if (*args == "OUT") {
    for(register short i = device->pwmVal; i >= 0; i -= FADE_SPEED) {
      if (i < 0) {
        analogWrite(device->pin, 0);
        break;
      }
      analogWrite(device->pin, i * PWM_VAL_RATE);
      delay(1);
    }
    device->power = false;
  }
  else {
    return error(2);
  }

  return true;
}

bool rapid_toggle(Device *device, String *args) {
  if (*args == "") return error(3);

  bool originState = device->power;

  unsigned long startTime = millis();
  unsigned long duration = (*args).toDouble() * 1000;

  for (;;) {
    power_control(device, true);
    delay(RAPID_DELAY);
    power_control(device, false);
    delay(RAPID_DELAY);

    if (check_interrupt()) break;
    if (millis() - startTime > duration) break;
  }

  return power_control(device, originState);
}

bool alarm_control(String *args) {
  if (*args == "") return error(3);

  if (*args == "ON") {
    return alarm(true);
  }
  else if (*args == "OFF") {
    return alarm(false);
  }
  else if (*args == "ST") {
    return alarm_return();    
  }
  else {
    return error(2);
  }
}

bool alarm(bool on) {
  Alarm = on;
  return Alarm == on;
}

bool alarm_return() {
  String outString = Alarm ? "ON" : "OFF";
  send(outString);
  Feedback = false;

  return true;
}

bool status_return(Device *device, String *args) {
  if (*args == "") return error(3);

  if (*args == "PWR") {
    return power_return(device);
  }
  else if (*args == "BRT") {
    return pwm_return(device);
  }
  else if (*args == "SPD") {
    return pwm_return(device);
  }
  else {
    return error(2);
  }
}

bool power_return(Device *device) {
  String outString = device->power ? "ON" : "OFF";
  send(outString);
  Feedback = false;

  return true;
}

bool pwm_return(Device *device) {
  String outString = String((int)device->pwmVal);
  send(outString);
  Feedback = false;

  return true;
}

bool check_interrupt() {
  if (! Serial.available()) return false;
  return Serial.read() == INTERRUPT_CODE;
}

bool toggle(Device *device) {
  return power_control(device, !(device->power));
}

void beep(int howMany) {
  Buzzer.countRemain += howMany;
}
#endif


#ifdef HELPER_FUNCTIONS
String split(String data, char separator, int index) {
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for (unsigned register short i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
      found++;
      strIndex[0] = strIndex[1]+1;
      strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }

  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void send(String message) {
  Serial.print(message + TERMINATE);
}

bool error(int beepCnt) {
  beep(beepCnt);
  return false;
}
#endif


#ifdef INITIAL_SETUP
void initial_device_setup() {
  DeviceArray[0] = &LitDevice;
  DeviceArray[1] = &LedDevice;
  DeviceArray[2] = &FanDevice;

  LitDevice.name = "LIT";
  LitDevice.pin = LIT_CONTROL_PIN;

  LedDevice.name = "LED";
  LedDevice.pin = LED_CONTROL_PIN;

  FanDevice.name = "FAN";
  FanDevice.pin = FAN_CONTROL_PIN;

  LitButton.pin = LIT_BUTTON_PIN;
  LitButton.pinMode = LIT_BTN_MODE;
  LitButton.pinActive = LIT_BTN_ACTIVE;

  Buzzer.pin = BUZ_CONTROL_PIN;
}

void initial_pin_setup() {
  pinMode(LitButton.pin, LitButton.pinMode);
  pinMode(Buzzer.pin, OUTPUT);

  for (unsigned register short i = 0; i < NUMBER_OF_DEVICES; ++ i) {
    pinMode(DeviceArray[i]->pin, OUTPUT);
  }
}

void initial_serial_setup() {
  Serial.begin(SERIAL_BAUDRATE);
  Serial.setTimeout(0);
  Serial.print("Serial ready.\n");

  Feedback = true;
  beep(5);
}
#endif
