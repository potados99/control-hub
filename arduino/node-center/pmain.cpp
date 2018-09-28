#include "pmain.h"

void lightButtonPushed();
void lightChanged(uint8_t feedback);
void serialRecieved(std::string recieved);

SerialWrapper serial("/dev/cu.SLAB_USBtoUART");
Button LightButton(1, INPUT_PULLUP, lightButtonPushed);
Device Light("Good light", 2);

void setup() {
    serial.registerCallback(serialRecieved);
    Light.registerFeedback(lightChanged);
}

void loop() {
    serial.loop();
    LightButton.loop();
    Light.loop();
}

void lightButtonPushed() {
    Light.togglePower();
}

void lightChanged(uint8_t feedback) {
    switch (feedback) {
        case SUCCESS:
            std::cout << "Light successfully controlled!" << std::endl;
            break;
            
        default:
            std::cout << "ERROR!!" << std::endl;
            break;
    }
}

void serialRecieved(std::string recieved) {
    std::cout << "Recieved [" << recieved << "] !" << std::endl;
}


