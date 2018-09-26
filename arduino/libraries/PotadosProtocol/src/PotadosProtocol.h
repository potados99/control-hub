#ifndef POTADOSPROTOCOL_h
#define POTADOSPROTOCOL_h

#include "includes.h"
#include "PPPacket.h"

#define TERMINATE '\n'
#define TERMINATE_OPTIONAL ';'

typedef void (*Callback)(PPPacket&);

typedef enum ComState
{
  WAITING_FOR_REQUEST   = 0x11,
  WAITING_FOR_RESPONSE  = 0x02,
  PROCESSING_REQUEST    = 0x24,

  REQUEST_AVAILABLE     = 0x10,
  REPLY_AVAILABLE       = 0x20
} COM_STATE;

typedef enum ListenResult
{
  TIMEOUT,
  RECIEVED_NOTHING,
  RECIEVED_REQUEST,
  RECIEVED_RESPONSE,
  RECIEVED_BAD_PACKET,
  RECIEVED_DONT_CARE,
  UNABLE_TO_RECIEVE
} LISTEN_RESULT;

class PPSerial
{
private:
  SoftwareSerial    serial;
  byte              id;
  byte              state = 0;

  uint16_t          timeOut = 2000;

  uint64_t          lastSentTime = 0;
  uint64_t          lastRecievedTime = 0;
  PPPacket          lastSentPacket = "";
  PPPacket          lastRecievedPacket = "";
  String            inputBuffer = "";

  Callback          onRequest;
  Callback          onResponse;

  byte              parsePacket(String incommingString, PPPacket& packet);

public:
  PPSerial(byte rx_pin, byte tx_pin, byte id, Callback listenCallback); // rx, tx, id
  ~PPSerial(void);

  byte              send(PPPacket& packet, bool nonBlocked);
  byte              send(char type, byte dest, String data, bool nonBlocked);
  byte              reSend(void);
  byte              listen(void);
  void              loop();
};

#endif
