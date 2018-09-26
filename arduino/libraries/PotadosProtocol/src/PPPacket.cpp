#include "PPPacket.h"

PPPacket::PPPacket(char type, byte sender, byte reciever, String data = "")
{
  this.type = type;
  this.senderID = sender;
  this.recieverID = reciever;
  this.dataLength = data.length();
  this.data = data;
}

String PPPacket::fillZeroHexString(byte val, byte digits = 2)
{
  String hexString = String(val, HEX);
  String filledString;

  for (byte i = hexString.length(); i < digits; ++ i)
  {
    filledString += ' ';
  }
  filledString += hexString;

  return filledString;
}

String PPPacket::toString()
{
  String stringOut;

  stringOut += this.type;
  stringOut += fillZeroHexString(this.senderID, 2);
  stringOut += fillZeroHexString(this.recieverID, 2);
  stringOut += fillZeroHexString(this.dataLength, 2);
  stringOut += data;
}
