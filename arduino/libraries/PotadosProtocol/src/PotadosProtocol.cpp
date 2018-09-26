#include "PotadosProtocol.h"

PPSerial::PPSerial(byte rx_pin, byte tx_pin, byte id, Callback listenCallback)
{
  this.serial = SoftwareSerial(rx_pin, tx_pin);
  this.id = id;
  this.listenCallback = listenCallback;
}

byte PPSerial::send(PPPacket& packet, bool nonBlocked = false)
{
  if (! nonBlocked)
  {
    if (! HAS(this.state, REQUEST_AVAILABLE))
    {
      if (packet.type == ORDER || packet.type == ASK) return this.state;
    }
    else if (! HAS(this.state, REPLY_AVAILABLE))
    {
      if (packet.type == REPLY) return this.state;
    }
    else if (! HAS(this.state, REQUEST_AVAILABLE) && ! HAS(this.state, REPLY_AVAILABLE))
    {
      return this.state;
    }
  }

  this.serial.print(packet.toString() + TERMINATE);

  this.lastSentTime = millis();
  this.lastSentPacket = packet;

  if (this.state == PROCESSING_REQUEST && packet.type == REPLY)
  {
    this.state = WAITING_FOR_REQUEST;
  }

  return 0;
}

byte PPSerial::send(char type, byte dest, String data, bool nonBlocked = false)
{
  PPPacket packet(type, this.id, dest, data);
  return send(packet, nonBlocked);
}

byte PPSerial::reSend()
{
  return send(this.lastSentPacket);
}

byte PPSerial::listen()
{
  // When busy so that can't listen anything
  if (this.state == PROCESSING_REQUEST)
  {
    if (millis() - this.lastSentTime > this.readTimeOut)
    {
      // And I'm being busy so so long, go default!
      this.state = WAITING_FOR_REQUEST;
    }
    else
    {
      return UNABLE_TO_RECIEVE;
    }
  }

  // When waitng for response of specific node
  if (this.state == WAITING_FOR_RESPONSE)
  {
    if (millis() - this.lastSentTime > this.readTimeOut)
    {
      this.state = WAITING_FOR_REQUEST;
      return TIMEOUT;
    }
  }

  if (! this.serial.available())
  {
    return RECIEVED_NOTHING;
  }

  char recieved = this.serial.read();

  if ((recieved == TERMINATE) || (recieved == TERMINATE_OPTIONAL))
  {
    PPPacket  recieved;
    byte      packetParseResult = parsePacket(this.inputBuffer, recieved);

    this.inputBuffer = ""; // flush anyway

    // Packet bad.
    if (packetParseResult != 0)
    {
      return RECIEVED_BAD_PACKET;
    }

    // Not mine.
    if (recieved.recieverID != this.id)
    {
      return RECIEVED_DONT_CARE;
    }

    // Answer to an unspoken question.
    if ((recieved.type == REPLY) && (recieved.senderID != this.lastSentPacket.recieverID))
    {
      return RECIEVED_DONT_CARE;
    }

    // Qualified!
    this.lastRecievedData = recieved;

    switch (recieved.type)
    {
      case ORDER:
      this.onRequest(recieved);
      this.state = PROCESSING_REQUEST;
      return RECIEVED_REQUEST;

      case ASK:
      this.onRequest(recieved);
      this.state = PROCESSING_REQUEST;
      return RECIEVED_REQUEST;

      case REPLY:
      this.onResponse(recieved);
      this.state = WAITING_FOR_REQUEST;
      return RECIEVED_RESPONSE;
    }
  }

  this.inputBuffer += recieved;
}

byte PPSerial::parsePacket(String incommingString, PPPacket& packet)
{
  if (incommingString.length() < 7) {
    return BROKEN;
  }

  byte returnCode = 0;

  char *endp;
  char *origin = incommingString.toCharArray();

  packet.type = *(origin++);

  char senderStr[(sizeof(byte) * 2) + 1] = {0,};
  char recieverStr[(sizeof(byte) * 2) + 1] = {0,};
  char dataLenStr[(sizeof(byte) * 2) + 1] = {0,};

  byte size  = (sizeof(byte) * 2); // 2
  memcpy(senderStr, origin, size);
  memcpy(recieverStr, origin += size, size);
  memcpy(dataLenStr, origin += size, size);

  errno = 0;
  packet.senderID   = (byte)strtol(senderStr, &endp, size * 8);
  packet.recieverID = (byte)strtol(recieverStr, &endp, size * 8);
  packet.dataLength = (byte)strtol(dataLenStr, &endp, size * 8);

  if (errno) {
    return BROKEN;
    errno = 0;
  }

  String data = incommingString.substring(7);
  if (packet.dataLength != data.length()) {
    return WRONG_DATA_LENGTH;
  }

  packet.data = data;

  return 0;
}

void PPSerial::loop()
{
  // default action is listen
  byte result = listen();
  if (HAS(result, RECIEVED_REQUEST)
  {
    onRequest(this.lastRecievedPacket);
  }
  else if (HAS(result, RECIEVED_RESPONSE))
  {
    onResponse(this.lastRecievedPacket);
  }
}
