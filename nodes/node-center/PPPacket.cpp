#include "PPPacket.h"

PPPacket::PPPacket() {
}

PPPacket::PPPacket(char type, uint8_t sender, uint8_t reciever, std::string data)
{
    this->type = type;
    this->senderID = sender;
    this->recieverID = reciever;
    this->dataLength = data.length();
    this->data = data;
}

std::string PPPacket::fillZeroHexString(uint8_t val, uint8_t digits)
{
    // std::string hexString = String(val, HEX);
    std::stringstream stream;
    stream << std::hex << (int)val;
    std::string result( stream.str() );
    
    std::string hexString = result;
    std::string filledString;
    
    for (uint8_t i = hexString.length(); i < digits; ++ i)
    {
        filledString += '0';
    }
    filledString += hexString;
    
    for (auto & c: filledString) c = toupper(c);
    
    return filledString;
}

std::string PPPacket::toString()
{
    std::string stringOut;
    
    stringOut += this->type;
    stringOut += fillZeroHexString(this->senderID, 2);
    stringOut += fillZeroHexString(this->recieverID, 2);
    stringOut += fillZeroHexString(this->dataLength, 2);
    stringOut += data;
    
    return stringOut;
}

