#pragma once
#include "inputdefinitions.hpp"
#include "joystick.hpp"
#include <map>
#include <vector>

enum BEKeyBinds{
    BE_KEY_UP,
    BE_KEY_DOWN,
    BE_KEY_LEFT,
    BE_KEY_RIGHT,
    BE_KEY_A,
    BE_KEY_B,
    BE_KEY_X,
    BE_KEY_Y,
};
enum InputMethodIdentifier{
    IM_NONE,
    IM_KEYBOARD,
    IM_JOYSTICK,
    IM_MOUSE
};
enum IM_InputType{
    IT_BUTTON,
    IT_AXIS,
    IT_HAT,
};
class InputMethod{
    public:
    InputMethod(std::string str,int n);
    InputMethod(){
        method = IM_NONE;
        Key = -1;
        DeviceId = 0;
        type = IT_BUTTON;
    }
    InputMethodIdentifier method;
    IM_InputType type;
    int Key;
    int DeviceId;


};

class BEInput{
    public:
        BEInput();
        ~BEInput();
        bool RegisterInput(BEKeyBinds key,InputMethod method);

        bool IsKeyDown(BEKeyBinds key);
        bool IsKeyUp(BEKeyBinds key);
        bool KeyPress(BEKeyBinds key);
        bool KeyRelease(BEKeyBinds key);

        void Update(float dt);
    private:
        InputState GetKeyStatus(InputMethod method);
        std::map<BEKeyBinds,std::vector<InputMethod>> keyData;
};
