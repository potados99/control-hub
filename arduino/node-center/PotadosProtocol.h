#ifndef POTADOSPROTOCOL_h
#define POTADOSPROTOCOL_h

#include "PPPacket.h"

#define TERMINATE '\n'
#define TERMINATE_OPTIONAL ';'

#define RESULT(X, Y) (((X) << 4) | (Y))
#define CLASS(X) (X >> 4)
#define DETAIL(X) (X & 0x0F)

#define CLASSIS(X, Y) (CLASS(X) == (Y))
#define CLASSHAS(X, Y) (HAS(CLASS(X), Y))

typedef void (*ProtocolCallback)(PPPacket&);

typedef enum CommunicationClass {
    REQUEST_AVAILABLE = 1,
    REPLY_AVAILABLE,
    NONE_AVAILABLE
} COMMUNICATION_CLASS;

typedef enum CommunicationState
{
    WAITING_FOR_REQUEST         = RESULT(REQUEST_AVAILABLE, 1),
    WAITING_FOR_RESPONSE        = RESULT(NONE_AVAILABLE, 2),
    PROCESSING_REQUEST          = RESULT(REPLY_AVAILABLE, 3),
} COMMUNICATION_STATE;

typedef enum ResultClass {
    NONE                        = 0,
    SUCCEDDED                   = 1,
    FAILED                      = 2,
    
    IS_VALID                    = 3,
    IS_INVALID                  = 4,

    IS_IGNORABLE                = 5
} RESULT_CLASS;

typedef enum ListenResult
{
    RECIEVED_NOTHING            = RESULT(IS_IGNORABLE, 1),
    RECIEVED_REQUEST            = RESULT(IS_VALID, 2),
    RECIEVED_RESPONSE           = RESULT(IS_VALID, 3),
    
    RECIEVED_BROKEN             = RESULT(IS_IGNORABLE, 4),
    RECIEVED_DONT_CARE          = RESULT(IS_IGNORABLE, 5),
    RECIEVED_REQUEST_WHEN_BUSY  = RESULT(IS_IGNORABLE, 6)
} LISTEN_RESULT;

typedef enum PacketRouteResult {
    IS_REQUEST                  = RESULT(IS_VALID, 1),
    IS_RESPONSE                 = RESULT(IS_VALID, 2),
    
    IS_DONT_CARE                = RESULT(IS_IGNORABLE, 3),
    
    IS_NOT_TO_ME                = RESULT(IS_IGNORABLE, 4),
    IS_NOT_FOR_MY_REQUEST       = RESULT(IS_IGNORABLE, 5),
    
    IS_UNASKED_RESPONSE         = RESULT(IS_IGNORABLE, 6),
    IS_REQUEST_WHEN_BUSY        = RESULT(IS_IGNORABLE, 7),
    
    IS_BROKEN_PACKET            = RESULT(IS_IGNORABLE, 8),
} PACKET_ROUTE_RESULT;

typedef enum SendResult {
    SEND_SUCCEEDED              = RESULT(SUCCEDDED, 1),
    SEND_FAILED                 = RESULT(FAILED, 1)
} SEND_RESULT;

class PPSerial : ILoop
{
private:
    SoftwareSerial      serial;
    uint8_t             states = WAITING_FOR_REQUEST;
    uint8_t             address = 0;
    uint16_t            timeOut = 2000;
    
    uint64_t            lastSentTime = 0;
    uint64_t            lastRecievedTime = 0;
    PPPacket            lastSentPacket;
    PPPacket            lastRecievedPacket;
    std::string         inputBuffer = "";
    
    ProtocolCallback    requestCallback;
    ProtocolCallback    responseCallback;
    
    bool                parsePacket(std::string incommingString, PPPacket& packet);

    uint8_t             routeIncommingPacket(PPPacket& incomming);
    
    uint8_t             listen(void);
    
    void                requestRecieved(PPPacket& packet);
    void                responseRecieved(PPPacket& packet);
    
    void                flushInput();
    
public:
    PPSerial(uint8_t rx_pin, uint8_t tx_pin, uint8_t address); // rx, tx, id
 
    ~PPSerial(void);
    
    void                loop();

    uint8_t             send(PPPacket& packet);
    uint8_t             send(char type, uint8_t dest, std::string data);
    
    uint8_t             getState();
    
    void                onRequest(ProtocolCallback userCallback); // register
    void                onResponse(ProtocolCallback userCallback);
};

#endif

