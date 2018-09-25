#ifndef POTADOSPROTOCOL_h
#define POTADOSPROTOCOL_h

#include <Arduino.h>
#include <SoftwareSerial.h>

#define TERMINATE '\n'
#define TERMINATE_OPTIONAL ';'

typedef enum States {
  WAITING_FOR_RESPONSE = 0x01,
  SUCCEEDED = 0x02
} SERIAL_STATE;

typedef enum Type {
  ORDER = '!',
  ASK = '?',
  REPLY = ':',
  REQUEST_RESEND = '~'
} PACKET_TYPE;

typedef void (*callback)(String);

class PPPacket
{
private:
  String            fillZeroHexString(uint8_t val, uint8_t digits = 2);

public:
  char              type;
  uint8_t           senderID;
  uint8_t           recieverID;
  uint8_t           dataLength; // number of digits of hex data

  String            data;

  PPPacket(char type, uint8_t sender, uint8_t reciever, uint8_t dataLen, String data = "");

  String            toString();
}

class PPSerial
{
private:
  SoftwareSerial    serial;
  uint8_t           id;
  uint8_t           states = 0;
  uint16_t          readTimeOut = 2000;
  uint64_t          lastSentTime = 0;
  uint64_t          lastRecievedTime = 0;
  PPPacket          lastSentPacket = "";
  PPPacket          lastRecievedPacket = "";
  String            inputBuffer = "";

  bool              parsePacket(String incommingString, PPPacket& packet);

public:
  PPSerial(uint8_t rx_pin, uint8_t tx_pin, uint8_t id); // rx, tx, id
  ~PPSerial(void);

  void              send(uint8_t dest, String data);
  void              listen(void);
  bool              verify(void);
};

#endif
