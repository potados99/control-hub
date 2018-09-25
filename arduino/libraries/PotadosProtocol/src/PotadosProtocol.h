#ifndef POTADOSPROTOCOL_h
#define POTADOSPROTOCOL_h

#include <Arduino.h>
#include <SoftwareSerial.h>

typedef enum States {
  WAITING_FOR_RESPONSE = 0x01,
  SUCCEEDED = 0x02
}

typedef enum Callback {
  CALLBACK_NONE = 1,
  CALLBACK_NEEDED
};

// serial wrapper class
class SerialW
{
public:
  SoftwareSerial*   port;
  uint16_t          readTimeOut = 2000;
  unsigned long     lastSentTime = 0;
  unsigned long     lastRecievedTime = 0;
  String            lastSentData; /* RF command sent through RF serial */
  String            lastRecievedData;
  String            inputBuffer; /* RF serial input */
}

class PPManager
{
private:
  SerialW*          serial;
  uint8_t           id;
  uint8_t           states;

public:
  PPManager(uint8_t, uint8_t, uint8_t); // rx, tx, id
  ~PPManager(void);

  void              begin(void);
  void              end(void);

  void              send(String);
  void              listen(void);
  bool              verify(void);
};

#endif
