#ifndef PPPacket_h
#define PPPacket_h

#include "Arduino.h"

typedef enum PacketType
{
    ORDER = '!',
    ASK = '?',
    REPLY = ':',
    REPLY_BUSY = '~'
} PACKET_TYPE;

class PPPacket
{
private:
    std::string     fillZeroHexString(uint8_t val, uint8_t digits = 2);
    
public:
    char            type;
    uint8_t         senderID;
    uint8_t         recieverID;
    uint8_t         dataLength; // number of digits of hex data
    
    std::string     data;
    
    PPPacket();
    PPPacket(char type, uint8_t sender, uint8_t reciever, std::string data = "");
    
    std::string     toString();
};

#endif



