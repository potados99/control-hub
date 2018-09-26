#ifndef PPPacket_h
#define PPPacket_h

#include "includes.h"

typedef uint8_t byte;

typedef enum Type
{
  ORDER = '!',
  ASK = '?',
  REPLY = ':',
} PACKET_TYPE;

typedef enum PacketError
{
  WRONG_DATA_LENGTH = 1,
  BROKEN = 2
} PACKET_ERROR;

class PPPacket
{
private:
  String            fillZeroHexString(byte val, byte digits = 2);

public:
  char              type;
  byte              senderID;
  byte              recieverID;
  byte              dataLength; // number of digits of hex data

  String            data;

  PPPacket(char type, byte sender, byte reciever, byte dataLen, String data = "");

  String            toString();
};

#endif
