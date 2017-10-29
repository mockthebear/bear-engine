#pragma once
#include "inputdefinitions.hpp"
#include "joystick.hpp"
#include <map>
#include <vector>

enum BEKeyBinds{
    BE_KEY_UP=0,
    BE_KEY_DOWN=1,
    BE_KEY_LEFT=2,
    BE_KEY_RIGHT=3,
    BE_KEY_A=4,
    BE_KEY_B=5,
    BE_KEY_X=6,
    BE_KEY_Y=7,
    BE_KEY_PAUSE=8,
    BE_KEY_SELECT=9,
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
    InputMethod(std::string str,int n,int auxId = 0);
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

class TempKey{
    public:
    TempKey(){key = PRESSED;dur=0.0f;};
    TempKey(InputState s,float d){key = s;dur=d;};
    float dur;
    InputState key;
};

class BEInput{
    public:
        BEInput();
        ~BEInput();
        bool RegisterInput(BEKeyBinds key,InputMethod method);
        void clear();



        bool IsKeyDown(BEKeyBinds key);
        bool IsKeyUp(BEKeyBinds key);
        bool KeyPress(BEKeyBinds key);
        bool KeyRelease(BEKeyBinds key);
        int GetKeyBind(BEKeyBinds key,InputMethodIdentifier method=IM_KEYBOARD);
        bool IsBound(int key,InputMethodIdentifier method=IM_KEYBOARD);


        void Update(float dt);

        void ForceKeyUp(BEKeyBinds key,float duration);
        void ForceKeyPress(BEKeyBinds key,float duration);
        void ResetForcedKeys();
    private:
        InputState GetKeyStatus(InputMethod& method);
        std::map<BEKeyBinds,std::vector<InputMethod>> keyData;
        std::map<BEKeyBinds,TempKey> forcedData;
};

extern BEInput g_beinput;
