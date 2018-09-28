//
//  interface.hpp
//  Arduino-test
//
//  Created by POTADOS on 2018. 9. 28..
//  Copyright © 2018년 POTADOS. All rights reserved.
//

#ifndef interface_hpp
#define interface_hpp

#include <iostream>

class ILoop {
public:
    virtual void loop() = 0;
};

#endif /* interface_hpp */
