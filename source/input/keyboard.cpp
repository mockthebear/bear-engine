#include "keyboard.hpp"

Keyboard::Keyboard(){
    anyKeyPressed = 0;
}


void Keyboard::Update(float dt){
    anyKeyPressed = -1;
    for ( auto it = keyState.begin(); it != keyState.end(); ++it ){
        if (it->second == JUST_PRESSED){
            it->second = PRESSED;
        }
        if (it->second == JUST_RELEASED){
            it->second = RELEASED;
        }
    }
}


void Keyboard::KeyPress(int key){
    if (keyState[key] != PRESSED){
        anyKeyPressed = key;
        keyState[key] = JUST_PRESSED;
    }
}
void Keyboard::KeyRelease(int key){
    if (keyState[key] != RELEASED){
        keyState[key] = JUST_RELEASED;
    }
}
