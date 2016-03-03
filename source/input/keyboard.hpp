#ifndef KEYBOARDH_BE
#define KEYBOARDH_BE
#include <map>
#include "inputdefinitions.hpp"

/**
    @brief Class used to manage the Keyboard trough InputManager
    *
    *Dont waste your time on this class. Its managed by the InputManager
*/
class Keyboard{
    public:

    Keyboard();
    /**
        Update internal key statuses
    */
    void Update(float dt);
    /**
        Update and key
    */
    void KeyPress(int key);
    /**
        Update and key
    */
    void KeyRelease(int key);

    /**
        Yep!
    */
    int anyKeyPressed;
    /**
        KeyMap
    */
    std::map<int, InputState> keyState;
};
#endif // KEYBOARDH_BE
