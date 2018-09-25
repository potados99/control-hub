#include "PotadosProtocol.h"

PPPacket::PPPacket(char type, uint8_t sender, uint8_t reciever, String data = "")
{
  this.type = type;
  this.senderID = sender;
  this.recieverID = reciever;
  this.dataLength = data.length();
  this.data = data;
}

String PPPacket::fillZeroHexString(uint8_t val, uint8_t digits = 2)
{
  String hexString = String(val, HEX);
  String filledString;

  for (uint8_t i = hexString.length(); i < digits; ++ i)
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


PPSerial::PPSerial(uint8_t rx_pin, uint8_t tx_pin, uint8_t id)
{
  this.serial = SoftwareSerial(rx_pin, tx_pin);
  this.id = id;
}


void PPSerial::send(char type, uint8_t dest, String data)
{
  PPPacket packet(type, this.id, dest, data);

  this.serial.print(packet.toString() + TERMINATE);
}

void PPSerial::listen(callback feedbackBehavior)
{
  // Pass when not waiting for response.
  if (~this.states & WAITING_FOR_RESPONSE) {
    // not waiting for input. return.
    return;
  }
  else if (millis() - this.lastSentTime > this.readTimeOut) {
      // waiting but no response for 2 seconds. TIMEOUT!
      this.states &= ~WAITING_FOR_RESPONSE;
      this.states &= ~SUCCEEDED;
      send("F");
      beep(5);
      return;
  }

  if (! this.serial.available()) return; /* nothing to do when nothing arrived. */

  char recieved = this.serial.read();

  if ((recieved == TERMINATE) || (recieved == TERMINATE_OPTIONAL)) {
    if (verify(serial)) {
      // if (Feedback) send("T");
      feedbackBehavior("T");
    }
    else {
      // if (Feedback) send("F");
      feedbackBehavior("F");
    }

    this.inputBuffer = "";
    // Feedback = true;

    return; /* once LF came, return. */
  }

  this.inputBuffer += recieved;
}

bool
