#include "beinput.hpp"
#include "inputmanager.hpp"


BEInput g_beinput;

InputMethod::InputMethod(std::string str,int n,int auxId):InputMethod(){
    if (str == "keyboard" || str == ""){
        method = IM_KEYBOARD;
        Key = n;
    }
    if (str == "joystick0-hat0" ){//fix
        method = IM_JOYSTICK;
        type = IT_HAT;
        Key = n;
        DeviceId = auxId;
    }
    if (str == "joystick0-button" ){//fix
        method = IM_JOYSTICK;
        type = IT_BUTTON;
        Key = n;
        DeviceId = auxId;
    }
    if (str == "joystick0-axis" ){//fix
        method = IM_JOYSTICK;
        type = IT_AXIS;
        Key = n;
        DeviceId = auxId;
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
void BEInput::ResetForcedKeys(){
    forcedData.clear();
}


void BEInput::Update(float dt){

    for (auto &it : forcedData){
        if (it.second.dur > 0){
            if (it.second.key == JUST_PRESSED){
                 it.second.key = PRESSED;
            }
            it.second.dur -= dt;
        }
    }
}

void BEInput::clear(){
    keyData.clear();
}
bool BEInput::IsKeyDown(BEKeyBinds key){
    for (auto &it : keyData[key]){
        InputState state;
        if (forcedData[key].dur > 0){
            state = forcedData[key].key;
        }else{
            state = GetKeyStatus(it);
        }
        if (state == JUST_PRESSED || state == PRESSED){
            return true;
        }
    }
    return false;
}

bool BEInput::IsKeyUp(BEKeyBinds key){
    for (auto &it : keyData[key]){
        InputState state;
        if (forcedData[key].dur > 0){
            state = forcedData[key].key;
        }else{
            state = GetKeyStatus(it);
        }
        if (state == JUST_RELEASED || state == RELEASED){
            return true;
        }
    }
    return false;
}
bool BEInput::KeyPress(BEKeyBinds key){

    InputState state = UNKNOW_STATE;
    if (forcedData[key].dur > 0){
        state = forcedData[key].key;
        if (state == JUST_PRESSED){
            return true;
        }
    }
    for (auto &it : keyData[key]){
        state = GetKeyStatus(it);
        if (state == JUST_PRESSED){
            return true;
        }
    }
    return false;
}
int BEInput::GetKeyBind(BEKeyBinds key,InputMethodIdentifier method){
    for (auto &it : keyData[key]){
        if (it.method == method){
            if (it.type ==IT_HAT){
                return it.Key+1000;
            }
            if (it.type ==IT_AXIS){
                return it.Key+2000;
            }
            return it.Key;
        }
    }
    return -1;
}

bool BEInput::IsBound(int key,InputMethodIdentifier method){
    for (auto &vec : keyData){
        for (auto &it : vec.second){
            if (it.method == method){
                if (it.type ==IT_HAT && it.Key+1000 == key){
                    return true;
                }
                if (it.type ==IT_BUTTON && it.Key == key){
                    return true;
                }
                if (it.type ==IT_AXIS && it.Key+2000 == key){
                    return true;
                }

            }
        }
    }
    return false;
}

bool BEInput::KeyRelease(BEKeyBinds key){
    for (auto &it : keyData[key]){
        InputState state;
        if (forcedData[key].dur > 0){
            state = forcedData[key].key;
        }else{
            state = GetKeyStatus(it);
        }
        if (state == JUST_RELEASED){
            return true;
        }
    }
    return false;
}

void BEInput::ForceKeyPress(BEKeyBinds key,float duration){
    forcedData[key] = TempKey(JUST_PRESSED,duration);
}

InputState BEInput::GetKeyStatus(InputMethod& method){

    switch(method.method){
        case IM_KEYBOARD:
            return InputManager::GetInstance().GetKeyState(method.Key);
            break;
        case IM_JOYSTICK:{
            Joystick * joys = InputManager::GetInstance().GetJoystick(method.DeviceId);
            if (joys){
                if (method.type == IT_BUTTON){
                    return joys->GetButtonState(method.Key);
                }else if (method.type == IT_AXIS){
                    return joys->GetAxisInputState(method.Key);
                }else if (method.type == IT_HAT){
                    return joys->GetHatState(method.DeviceId,method.Key);
                }
            }
            break;
        }
        default:
            break;

    }
    return UNKNOW_STATE;
}
