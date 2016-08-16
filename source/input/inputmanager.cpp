#include "inputmanager.hpp"
#include "../engine/gamebase.hpp"
#include "../settings/configmanager.hpp"
#include "../engine/renderhelp.hpp"
#include "../performance/console.hpp"
#include "../settings/definitions.hpp"
#include __BEHAVIOR_FOLDER__
#include <stdio.h>
#include <stdlib.h>     /* abs */
#include <iostream>


#define FORCE_ACCELEROMETER


InputManager::InputManager(){
    #ifdef __ANDROID__
    SDL_SetHint(SDL_HINT_ACCELEROMETER_AS_JOYSTICK,"1");
    #endif
    quitGame = false;
    NumJoysticks = SDL_NumJoysticks();
    AccelerometerJoystick=-1;
    #ifdef __ANDROID__
    Joystick *toAccelerometer= NULL;
    #endif // __ANDROID__
    for (int i=0;i<8;i++){
        JoyPos[i] = -1;
    }
    if (NumJoysticks > 0){
        for (int i=0;i<NumJoysticks;i++){
            Joystick *joy = new Joystick(i);
            Joysticks[joy->GetInstanceId()] = joy;
            if ( (joy->GetName() == "Android Accelerometer" || (joy->GetButtonCount() == 0 && joy->GetAxisCount() == 3 && joy->GetHatsCount() == 0) ) && AccelerometerJoystick == -1){
                AccelerometerJoystick = i;
                #ifdef __ANDROID__
                toAccelerometer = joy;
                #endif
            }
            Console::GetInstance().AddText( "Joystick %d [%s] has %d B, %d A, %d H",i,joy->GetName().c_str(),joy->GetButtonCount(),joy->GetAxisCount(),joy->GetHatsCount());
        }
    }

    #ifdef __ANDROID__
    if (NumJoysticks > 0){
        HasAccelerometer=true;
        m_acceletometer.Set(toAccelerometer);
        Console::GetInstance().AddText( "[InputManager::Update]Accelerometer is working!");

    }else{
        HasAccelerometer=false;
    }
    #else
    HasAccelerometer=false;

    #endif

    for (auto  &it : Joysticks){
        if (!(it.first == AccelerometerJoystick && HasAccelerometer) )
        {
            for (int i=0;i<8;i++){
                if (JoyPos[i] == -1){
                    JoyPos[i] = it.first;
                    break;
                }
            }
        }

    }

    //CreateVirtualButton(SDLK_UP,PointInt(200,500),"Up");
}

Joystick* InputManager::GetJoystick(int id){
    if (JoyPos[id] == -1){
        return NULL;
    }
    return Joysticks[JoyPos[id]];
}
InputManager::~InputManager(){
    for ( auto &it : Joysticks ){
        if (it.second != NULL){
            it.second->Close();
            delete it.second;
        }
    }
}

InputManager& InputManager::GetInstance(){
    static InputManager teste;
    return teste;
}
void InputManager::Render(){
    //touchscreen.Render();
    for ( auto &it : TKeys ){
        it.Render();
    }
    for ( auto &it : Joysticks ){
        if (it.second != NULL)
            it.second->Render(it.first);
    }
}

void InputManager::Vibrate(uint32_t milis){
    #ifdef __ANDROID__
    JNIEnv* env = (JNIEnv*)SDL_AndroidGetJNIEnv();
    jobject activity = (jobject)SDL_AndroidGetActivity();
    jclass clazz(env->GetObjectClass(activity));



    jmethodID method_id = env->GetMethodID(clazz,"getSystemService","(Ljava/lang/String;)Ljava/lang/Object;");
    jstring method_str = env->NewStringUTF("vibrator");
    jobject vibrator_svc = env->CallObjectMethod(activity, method_id,method_str );
    jclass vib_cls = env->GetObjectClass(vibrator_svc);
    jmethodID vib_method_id = env->GetMethodID(vib_cls, "vibrate",
    "(J)V");
    jlong var1 = milis;
    env->CallVoidMethod(vibrator_svc, vib_method_id, var1);


    env->DeleteLocalRef(method_str);
    env->DeleteLocalRef(vibrator_svc);
    env->DeleteLocalRef(vib_cls);
    env->DeleteLocalRef(activity);
    env->DeleteLocalRef(clazz);


    #endif
}

void InputManager::Update(float dt){
    SDL_Event event;
    mouse.Update(dt);
    keyboard.Update(dt);
    touchscreen.Update(dt);
    for ( auto &it : Joysticks ){
        if (it.second != NULL)
            it.second->Update(dt);
    }
    while (SDL_PollEvent(&event)) {

        if(event.type == SDL_WINDOWEVENT){
            //Console::GetInstance().AddTextInfo(utils::format("%d",(int)event.window.event));
            if(event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED){
                ScreenManager::GetInstance().NotifyResized();
            }else if(event.window.event == SDL_WINDOWEVENT_HIDDEN){
                Console::GetInstance().AddText( "[InputManager::Update]Hidden");
            }else if(event.window.event == SDL_WINDOWEVENT_SHOWN){
                Console::GetInstance().AddText( "[InputManager::Update]Shown");
            }else if(event.window.event == SDL_WINDOWEVENT_RESIZED){
                /*
                    Nothing actually
                */
            }else if(event.window.event == SDL_WINDOWEVENT_MOVED){
                /*
                    Nothing actually
                */
            }else if(event.window.event == SDL_WINDOWEVENT_EXPOSED){
                /*
                    Nothing actually
                */
            }else if(event.window.event == SDL_WINDOWEVENT_CLOSE){
                Game::GetInstance()->isClosing = true;
                ScreenManager::GetInstance().NotyifyScreenClosed();
            }else if(event.window.event == SDL_WINDOWEVENT_ENTER){
                mouse.Enter(event.window.windowID);
            }else if(event.window.event == SDL_WINDOWEVENT_LEAVE){
                mouse.Leave(event.window.windowID);
            }else if(event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED){
                GameBehavior::GetInstance().OnFocus(true);
            }else if(event.window.event == SDL_WINDOWEVENT_FOCUS_LOST){
                GameBehavior::GetInstance().OnFocus(false);

            }else if(event.window.event == SDL_WINDOWEVENT_RESTORED){
                GameBehavior::GetInstance().OnRestored();
            }else{
                Console::GetInstance().AddText( "Unhooked[SDL_WINDOWEVENT] event id %d",event.window.event);
            }
        /*
            Android/ios
        */
        }else if(event.type == SDL_APP_WILLENTERBACKGROUND) {
        }else if(event.type == SDL_APP_WILLENTERFOREGROUND) {


        }else if(event.type == SDL_APP_DIDENTERBACKGROUND) {
        }else if(event.type == SDL_FINGERDOWN) {

            touchscreen.UpdateTouchPosition(event.tfinger.fingerId,event.tfinger.x,event.tfinger.y,event.tfinger.pressure);
            touchscreen.UpdateTouchStatus(event.tfinger.fingerId,true);
        }else if(event.type == SDL_FINGERUP) {

            touchscreen.UpdateTouchPosition(event.tfinger.fingerId,event.tfinger.x,event.tfinger.y,event.tfinger.pressure);
            touchscreen.UpdateTouchStatus(event.tfinger.fingerId,false);
        }else if(event.type == SDL_FINGERMOTION) {
            touchscreen.UpdateTouchPosition(event.tfinger.fingerId,event.tfinger.x,event.tfinger.y,event.tfinger.pressure);
        }else if(event.type == SDL_QUIT) {
            quitGame = true;
        }else if (event.type == SDL_KEYDOWN){
            keyboard.KeyPress(event.key.keysym.sym);
        }else if (event.type == SDL_TEXTINPUT){
            //todo
           // Console::GetInstance().AddText( "[Input text] %s",event.text.text);
        }else if (event.type == SDL_TEXTEDITING){
            //todo
            //Console::GetInstance().AddText( "[text edit] [%d : %d ]%s",event.edit.start,event.edit.length,event.text.text);
        }else if (event.type == SDL_KEYUP){
            keyboard.KeyRelease(event.key.keysym.sym);
        }else if (event.type == SDL_MOUSEWHEEL){
            mouse.Wheel(event.wheel.x,event.wheel.y);
        }else if (event.type == SDL_MOUSEBUTTONDOWN){
            mouse.ButtonDown(event.button.button);
        }else if (event.type == SDL_MOUSEBUTTONUP){
            mouse.ButtonUp(event.button.button);
        }else if (event.type == SDL_MOUSEMOTION){
            mouse.Motion(event.motion.x,event.motion.y);

        }else if (event.type == SDL_JOYDEVICEADDED  ){
            if (NumJoysticks != SDL_NumJoysticks()){
                NumJoysticks = SDL_NumJoysticks();
                Joystick *joy = new Joystick(event.jdevice.which);
                Joysticks[joy->GetInstanceId()] = joy;
                for (int i=0;i<8;i++){
                    if (JoyPos[i] == -1){
                        JoyPos[i] = joy->GetInstanceId();
                        break;
                    }
                }
            }
        }else if (event.type == SDL_JOYDEVICEREMOVED  ){
            NumJoysticks = SDL_NumJoysticks();
            Joysticks[event.jdevice.which]->Close();
            delete Joysticks[event.jdevice.which];
            Joysticks[event.jdevice.which] = NULL;
            for (int i=0;i<8;i++){
                if (JoyPos[i] == event.jdevice.which){
                    JoyPos[i] = -1;
                    break;
                }
            }
        }else if (event.type == SDL_JOYBUTTONDOWN or event.type == SDL_JOYBUTTONUP ){
            if (Joysticks[event.jbutton.which])
                Joysticks[event.jbutton.which]->Button(event.jbutton.button,event.jbutton.state);
        }else if (event.type == SDL_JOYAXISMOTION ){
            if (Joysticks[event.jaxis.which])
                Joysticks[event.jaxis.which]->MoveAxis(event.jaxis.axis,event.jaxis.value);
        }else if (event.type == SDL_JOYHATMOTION ){
            if (Joysticks[event.jhat.which])
                Joysticks[event.jhat.which]->MoveHat(event.jhat.hat,event.jhat.value);
        }else if (event.type == SDL_JOYBALLMOTION ){
            if (Joysticks[event.jball.which])
                Joysticks[event.jball.which]->MoveBall(event.jball.ball,event.jball.xrel,event.jball.xrel);
        }else{
            /*std::stringstream S;
            S << "Unhooked event id " << event.type;
            Console::GetInstance().AddText(S.str());
            */
            //std::cout << "Unhooked event id " << event.type << "\n";

        }
   }

   m_acceletometer.Update(dt);
   for ( auto &it : TKeys ){
        it.Update(dt,keyboard,touchscreen,mouse);
   }

}

void InputManager::CreateVirtualButton(int Key,PointInt size,PointInt position,std::string text){
    TKeys.emplace_back(TouchKeys(Key,position,size,text));
}
//Mouse stuff
bool InputManager::MousePress(int key){
    return mouse.mouseState[key] == JUST_PRESSED;
}
bool InputManager::MouseRelease(int key){
    return mouse.mouseState[key] == JUST_RELEASED;
}

bool InputManager::IsMousePressed(int key){
    return mouse.mouseState[key] == JUST_PRESSED or mouse.mouseState[key] == PRESSED;
}


int InputManager::GetMouseX(){
    return (mouse.position.x -   ScreenManager::GetInstance().GetOffsetW())/ScreenManager::GetInstance().GetScaleRatioW();
}
int InputManager::GetMouseY(){
    return (mouse.position.y-  ScreenManager::GetInstance().GetOffsetH())/ScreenManager::GetInstance().GetScaleRatioH() ;
}

int InputManager::GetMouseMousePressKey(){
    return mouse.lastKey;
}

int InputManager::GetMouseMouseReleaseKey(){
    return mouse.lastRKey;
}


bool InputManager::KeyPress(int key){
    return keyboard.keyState[key] == JUST_PRESSED;
}

bool InputManager::KeyRelease(int key){
    return keyboard.keyState[key] == JUST_RELEASED;
}

InputState InputManager::GetKeyState(int key){
    return keyboard.keyState[key];
}
bool InputManager::IsKeyDown(int key){
    return keyboard.keyState[key] == JUST_PRESSED or keyboard.keyState[key] == PRESSED;
}

bool InputManager::IsKeyUp(int key){
    return keyboard.keyState[key] == JUST_RELEASED or keyboard.keyState[key] == RELEASED;
}

int InputManager::IsAnyKeyPressed(bool considerTap){
    if (keyboard.anyKeyPressed != 0)
        return keyboard.anyKeyPressed;
    if (considerTap && IsScreenTapped())
        return  IsScreenTapped();
    return 0;
}
int InputManager::IsScreenTapped(){
    return touchscreen.m_hasTap;
}

