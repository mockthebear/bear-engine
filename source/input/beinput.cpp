#include "beinput.hpp"
#include "inputmanager.hpp"


InputMethod::InputMethod(std::string str,int n):InputMethod(){
    if (str == "keyboard" || str == ""){
        method = IM_KEYBOARD;
        Key = n;
    }
    if (str == "joystick0-button" ){//fix
        method = IM_JOYSTICK;
        type = IT_BUTTON;
        Key = n;
    }

}


BEInput::BEInput(){

}

BEInput::~BEInput(){

}

bool BEInput::RegisterInput(BEKeyBinds key,InputMethod method){
    keyData[key].emplace_back(method);
    return true;
}


bool BEInput::IsKeyDown(BEKeyBinds key){
    for (auto &it : keyData[key]){
        InputState state = GetKeyStatus(it);
        if (state == JUST_PRESSED || state == PRESSED){
            return true;
        }
    }
    return false;
}

bool BEInput::IsKeyUp(BEKeyBinds key){
    for (auto &it : keyData[key]){
        InputState state = GetKeyStatus(it);
        if (state == JUST_RELEASED || state == RELEASED){
            return true;
        }
    }
    return false;
}
bool BEInput::KeyPress(BEKeyBinds key){
    for (auto &it : keyData[key]){
        InputState state = GetKeyStatus(it);
        if (state == JUST_PRESSED){
            return true;
        }
    }
    return false;
}
bool BEInput::KeyRelease(BEKeyBinds key){
    for (auto &it : keyData[key]){
        InputState state = GetKeyStatus(it);
        if (state == JUST_RELEASED){
            return true;
        }
    }
    return false;
}

InputState BEInput::GetKeyStatus(InputMethod method){
    switch(method.method){
        case IM_KEYBOARD:
            return InputManager::GetInstance().GetKeyState(method.Key);
            break;
        case IM_JOYSTICK:{
            Joystick * joys = InputManager::GetInstance().GetJoystick(method.DeviceId);
            if (joys){
                if (method.type == IT_BUTTON){
                    return joys->GetButtonState(method.Key);
                }else{

                }
            }
            break;
        }
        default:
            break;

    }
    return UNKNOW_STATE;
}
