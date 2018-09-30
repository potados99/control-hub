#include "PotadosProtocol.h"

PPSerial::PPSerial(uint8_t rx_pin, uint8_t tx_pin, uint8_t address) : serial(rx_pin, tx_pin) {
    this->address = address;
}

PPSerial::~PPSerial() {
    
}

void PPSerial::loop() {
    this->serial.loop();
    
    switch (listen())
    {
        case RECIEVED_REQUEST:      requestRecieved(this->lastRecievedPacket);
            break;
            
        case RECIEVED_RESPONSE:     responseRecieved(this->lastRecievedPacket);
            break;
    }
}

uint8_t PPSerial::send(PPPacket& packet) {
    switch (this->states) {
            
        case WAITING_FOR_REQUEST:
            
            switch (packet.type) {
                case REPLY:         return SEND_FAILED;
                case REPLY_BUSY:    return SEND_FAILED;
            } break;
            
        case WAITING_FOR_RESPONSE:
            
            switch (packet.type) {
                case ORDER:         return SEND_FAILED;
                case ASK:           return SEND_FAILED;
                    
                case REPLY:         return SEND_FAILED;
            } break;

        case PROCESSING_REQUEST:
            
            switch (packet.type) {
                case ORDER:         return SEND_FAILED;
                case ASK:           return SEND_FAILED;
            } break;
    }
    
    std::cout << "goint to send!" << std::endl;
    
    this->serial.print(packet.toString() + TERMINATE);
    
    this->lastSentTime = millis();
    this->lastSentPacket = packet;
    
    switch (packet.type) {
        case ORDER:         this->states = WAITING_FOR_RESPONSE; break;
        case ASK:           this->states = WAITING_FOR_RESPONSE; break;
            
        case REPLY:         this->states = WAITING_FOR_REQUEST; break;
    }
    
    return SEND_SUCCEEDED;
}

uint8_t PPSerial::send(char type, uint8_t dest, std::string data) {
    PPPacket packet(type, this->address, dest, data);
    return send(packet);
}

uint8_t PPSerial::getState() {
    return this->states;
}

void PPSerial::onRequest(ProtocolCallback userCallback) {
    this->requestCallback = userCallback;
}

void PPSerial::onResponse(ProtocolCallback userCallback) {
    this->responseCallback = userCallback;
}


bool PPSerial::parsePacket(std::string incommingString, PPPacket& packet) {
    if (incommingString.length() < 7) {
        // 7 is neccessary (1 type + 2 sender + 2 reciever + 2 data length)
        return false;
    }
    
    char *endp;
    const char *origin = incommingString.c_str();
    
    packet.type = *(origin);
    
    char senderStr[(sizeof(uint8_t) * 2) + 1] = {0,};
    char recieverStr[(sizeof(uint8_t) * 2) + 1] = {0,};
    char dataLenStr[(sizeof(uint8_t) * 2) + 1] = {0,};
    
    uint8_t size  = (sizeof(uint8_t) * 2); // 2
    memcpy(senderStr, origin + 1, size);
    memcpy(recieverStr, origin + 1 + size, size);
    memcpy(dataLenStr, origin + 1 + size + size, size);
    
    errno = 0;
    packet.senderID   = (uint8_t)strtol(senderStr, &endp, size * 8);
    packet.recieverID = (uint8_t)strtol(recieverStr, &endp, size * 8);
    packet.dataLength = (uint8_t)strtol(dataLenStr, &endp, size * 8);
    
    if (errno) return false;
    
    std::string data =  incommingString.substr(7);
    if (packet.dataLength != data.length()) {
        return false;
    }
    
    packet.data = data;
    
    return true;
}

uint8_t PPSerial::routeIncommingPacket(PPPacket &incomming) {
    // Not mine.
    if (incomming.recieverID != this->address)
        return IS_NOT_TO_ME;
    
    // I didn't ask but reply?
    if (((incomming.type == REPLY) || incomming.type == REPLY_BUSY)
        && (incomming.senderID != this->lastSentPacket.recieverID))
        return IS_NOT_FOR_MY_REQUEST;
    
    switch (this->states) {
            
        case WAITING_FOR_REQUEST:
            
            switch (incomming.type) {
                case ORDER:         return IS_REQUEST;
                case ASK:           return IS_REQUEST;
                    
                case REPLY:         return IS_UNASKED_RESPONSE;
                case REPLY_BUSY:    return IS_UNASKED_RESPONSE;
            }
            
        case WAITING_FOR_RESPONSE:
            
            switch (incomming.type) {
                case ORDER:         return IS_REQUEST_WHEN_BUSY;
                case ASK:           return IS_REQUEST_WHEN_BUSY;
                    
                case REPLY:         return IS_RESPONSE;
                case REPLY_BUSY:    return IS_RESPONSE;
            }
            
        case PROCESSING_REQUEST:
            
            switch (incomming.type) {
                case ORDER:         return IS_REQUEST_WHEN_BUSY;
                case ASK:           return IS_REQUEST_WHEN_BUSY;
                    
                case REPLY:         return IS_UNASKED_RESPONSE;
                case REPLY_BUSY:    return IS_UNASKED_RESPONSE;
            }
            
        default:
            return 0;
    }
}

uint8_t PPSerial::listen()
{
    if (! this->serial.available()) return RECIEVED_NOTHING;
    
    char recieved = this->serial.read();
    if ((recieved == TERMINATE) || (recieved == TERMINATE_OPTIONAL))
    {
        std::string in = this->inputBuffer;
        flushInput();
        
        PPPacket recieved;
        if (! parsePacket(in, recieved)) return RECIEVED_BROKEN;
        
        uint8_t packetRouteResult = routeIncommingPacket(recieved);
        
        if (! CLASSIS(packetRouteResult, IS_VALID)) {
            switch (packetRouteResult) {
                case IS_DONT_CARE:          return RECIEVED_DONT_CARE;
                case IS_BROKEN_PACKET:      return RECIEVED_BROKEN;
                case IS_REQUEST_WHEN_BUSY:  return RECIEVED_REQUEST_WHEN_BUSY;
 
                default:                    return RECIEVED_DONT_CARE;
            }
        }
        
        /////////////// Qualified! /////////////
        
        this->lastRecievedPacket = recieved;
        
        switch (packetRouteResult)
        {
            case IS_REQUEST:
                this->states = PROCESSING_REQUEST;
                return RECIEVED_REQUEST;
                
            case IS_RESPONSE:
                this->states = WAITING_FOR_REQUEST;
                return RECIEVED_RESPONSE;
                
            default:
                return RECIEVED_NOTHING; // this should not be happened.
        }
    }
    
    this->inputBuffer += recieved;
    
    return RECIEVED_NOTHING;
}


void PPSerial::requestRecieved(PPPacket &packet) {
    if (this->requestCallback == nullptr) return;
    
    this->requestCallback(packet);
}

void PPSerial::responseRecieved(PPPacket &packet) {
    if (this->responseCallback == nullptr) return;
    
    this->responseCallback(packet);
}


void PPSerial::flushInput() {
    this->inputBuffer = "";
}
