#include "pmain.h"

void lightButtonPushed();
void lightChanged(uint8_t feedback);

void req(PPPacket& packet);
void res(PPPacket& packet);
std::string getStateString(PPSerial& serialInstance);

Button LightButton(1, INPUT_PULLUP, lightButtonPushed);
Device Light("Good light", 2);
PPSerial That(1,2, 0x01);

void setup() {
    Light.registerFeedback(lightChanged);
    
    That.onRequest(req);
    That.onResponse(res);
}

void loop() {
    That.loop();
    LightButton.loop();
    Light.loop();
}

void lightButtonPushed() {
    Light.togglePower();
}

void lightChanged(uint8_t feedback) {
    switch (feedback) {
        case SUCCESS: {
            
            std::cout << "\n    Before: State is " << getStateString(That) << std::endl;
            
            std::cout << "\nLight successfully controlled!" << std::endl;
            switch (That.send(ORDER, 02, "hello!_I'm_a_test_string!")) {
                case SEND_SUCCEEDED:
                    std::cout << "\nSend succeeded!" << std::endl;
                    break;
                default:
                    std::cout << "\nSend failed.." << std::endl;
                    break;
            }
            
            std::cout << "\n    After: State is " << getStateString(That) << std::endl;
            
            break;
        }
            
        default:
            std::cout << "\nERROR!!" << std::endl;
            break;
    }
}

void req(PPPacket& packet) {
    std::cout << "Got request." << std::endl;
    std::cout << "It says...: " << std::endl;
    std::cout << packet.data << std::endl;
}

void res(PPPacket& packet) {
    std::cout << "\nGot response." << std::endl;
    std::cout << "It says...: " << std::endl;
    std::cout << packet.data << std::endl;
}


std::string getStateString(PPSerial& serialInstance) {
    std::string before;
    switch (That.getState()) {
        case WAITING_FOR_REQUEST:
            before = "Waiting for request.";
            break;
            
        case WAITING_FOR_RESPONSE:
            before = "Waiting for response.";
            break;
            
        case PROCESSING_REQUEST:
            before = "Processing request.";
            break;
            
        default:
            before = "!";
            break;
    }
    return before;
}
