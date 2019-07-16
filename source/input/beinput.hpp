#pragma once
#include "inputdefinitions.hpp"
#include "joystick.hpp"
#include <map>
#include <vector>

enum BEKeyBinds{
    BE_KEY_NONE=-1,
    BE_KEY_UP=0,
    BE_KEY_DOWN=1,
    BE_KEY_LEFT=2,
    BE_KEY_RIGHT=3,
    BE_KEY_A=4, // jump
    BE_KEY_X=5, //attack
    BE_KEY_PAUSE=6,
    BE_KEY_SELECT=7,
    BE_KEY_B=8, //dash

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
    bool operator==(const InputMethod p2){
        return this->method == p2.method;
    }

    bool operator==(const InputMethodIdentifier p2){
        return this->method == p2;
    }
    bool IsValid(){
        return method != IM_NONE;
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
        void clearMethod(InputMethodIdentifier method);

        InputMethodIdentifier GetPressedKeyMode(BEKeyBinds key);

        bool IsKeyDown(BEKeyBinds key);
        bool IsKeyUp(BEKeyBinds key);
        bool KeyPress(BEKeyBinds key);
        bool KeyRelease(BEKeyBinds key);
        int GetKeyBind(BEKeyBinds key,InputMethodIdentifier method=IM_KEYBOARD);
        /**
            IF kb is set to UP, it check on alias to see the avaliability.
        */
        bool IsBound(int key,InputMethodIdentifier method=IM_KEYBOARD, BEKeyBinds kb=BE_KEY_NONE);


        void Update(float dt);

        void ForceKeyUp(BEKeyBinds key,float duration);
        void ForceKeyPress(BEKeyBinds key,float duration);
        void ResetForcedKeys();
        void RegisterKeyAlias(BEKeyBinds key, InputMethod method);
    private:
        InputState GetKeyStatus(InputMethod& method);
        std::map<BEKeyBinds,std::vector<InputMethod>> keyData;
        std::map<BEKeyBinds,TempKey> forcedData;
        std::map<BEKeyBinds, InputMethod> aliasMap;
};

extern BEInput g_beinput;
