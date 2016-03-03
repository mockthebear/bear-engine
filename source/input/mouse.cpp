#include "mouse.hpp"
#include SDL_LIB_HEADER
#include <iostream>

Mouse::Mouse(){
    lastKey = 0;
    lastRKey = 0;
    isInside = true;
}


Mouse::~Mouse(){

}

void Mouse::Update(float dt){
    lastKey = 0;
    lastRKey = 0;
    for ( int i=0;i<3;i++ ){
          if (mouseState[i] == JUST_PRESSED){
                mouseState[i] = PRESSED;
          }
          if (mouseState[i] == JUST_RELEASED){
                mouseState[i] = RELEASED;
          }
    }

}
void Mouse::ButtonDown(int button){
    if (mouseState[button] != PRESSED){
        lastKey = button;
        mouseState[button] = JUST_PRESSED;
    }
}
void Mouse::ButtonUp(int button){
    if (mouseState[button] != RELEASED){
        lastRKey = button;
        mouseState[button] = JUST_RELEASED;
    }

}

void Mouse::Wheel(int x,int y){
    wheel.x = x;
    wheel.y = y;
}

void Mouse::Motion(int x,int y){
    position.x = x;
    position.y = y;
}

void Mouse::Enter(int window){
    isInside = true;
}

void Mouse::Leave(int window){
    isInside = false;
}

