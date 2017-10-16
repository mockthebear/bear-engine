#include "joystick.hpp"
#include "../performance/console.hpp"
#include "../engine/renderhelp.hpp"


Joystick::Joystick(){
    m_joyHandler = NULL;
}

Joystick::Joystick(int index){
    m_joyHandler = SDL_JoystickOpen(index);
    if (m_joyHandler){
        m_id        = SDL_JoystickInstanceID(m_joyHandler);
        m_name       = SDL_JoystickNameForIndex(index);
        m_axes       = SDL_JoystickNumAxes(m_joyHandler);
        m_buttons    = SDL_JoystickNumButtons(m_joyHandler);
        m_balls      = SDL_JoystickNumBalls(m_joyHandler);
        m_hats       = SDL_JoystickNumHats(m_joyHandler);
        m_haptic     = SDL_HapticOpenFromJoystick(m_joyHandler);
        anyKeyPressed= false;
        HasCallButton= false;
        HasCallAxis  = false;
        HasCallBall  = false;
        HasCallHat   = false;
        Axis = new int[m_axes];
        Hats = new JoyHat[m_hats];
        Balls = new PointInt[m_balls];

        for (int i=0;i<m_axes;i++)
            Axis[i] = 0;
        if (m_haptic){
            if (SDL_HapticRumbleSupported(m_haptic)){
                SDL_HapticRumbleInit( m_haptic );
            }else{
                SDL_HapticClose( m_haptic );
                m_haptic = nullptr;
            }
        }

    }
}

Joystick::~Joystick(){
    delete Axis;
    delete Balls;
}
bool Joystick::VibrateStop(){
    if (!m_haptic){
        return false;
    }
    return SDL_HapticRumbleStop( m_haptic) == 0;
}

bool Joystick::Vibrate(float strenght,uint32_t duration){
    if (!m_haptic){
        return false;
    }
    return SDL_HapticRumblePlay( m_haptic, strenght, duration ) == 0;
}


void Joystick::Close(){
    if (IsWorking() && SDL_JoystickGetAttached(m_joyHandler)) {
        SDL_JoystickClose(m_joyHandler);
        if (m_haptic){
            SDL_HapticClose( m_haptic );
        }
    }
    m_joyHandler = nullptr;
    m_haptic = nullptr;
}

void Joystick::Render(int id){
    int x = (id%3)*300;
    int y = (id/3)*400;
    RenderHelp::DrawSquareColorA(x,y+100,400.0f,m_axes*32,189,227,138,100);
    for (int i=0;i<m_axes;i++)
        RenderHelp::DrawSquareColorA(x+200,y+100 + i*32,(Axis[i]/32767.0f)*200.0f,16,255,i&1 ? 255 : 0,i&2 ? 255 : 0,100);
    for (int i=0;i<m_buttons;i++){
        int c = IsButtonDown(i) ? 200 : 100;
        RenderHelp::DrawSquareColorA(x + i*18,y+20,16,16,200,c,0,100);
    }
}

void Joystick::Update(float dt){
    if (!IsWorking())
        return;
    anyKeyPressed = -1;
    for ( auto it = Buttons.begin(); it != Buttons.end(); ++it ){
        if (it->second == JUST_PRESSED){
            it->second = PRESSED;
        }
        if (it->second == JUST_RELEASED){
            it->second = RELEASED;
        }
    }
    if (m_hats > 0){
        for (int i=0;i<m_hats;i++){
            Hats[i].Update(dt);
        }
    }
}

bool Joystick::ButtonPress(int key){
    return Buttons[key] == JUST_PRESSED;
}
bool Joystick::ButtonRelease(int key){
    return Buttons[key] == JUST_RELEASED;
}
bool Joystick::IsButtonDown(int key){
    return Buttons[key] == JUST_PRESSED || Buttons[key] == PRESSED;
}

bool Joystick::IsButtonUp(int key){
    return Buttons[key] == JUST_RELEASED || Buttons[key] == RELEASED;
}

bool Joystick::IsValidHat(int hatId,int button){
    if (hatId >= m_hats){
        return false;
    }
    if (button >= JOYHAT_BUTTONS_COUNT){
        return false;
    }
    return true;
}
InputState Joystick::GetHatState(int hatId,int button){
    if (!IsValidHat(hatId,button)){
        return RELEASED;
    }
    return Hats[hatId].keys[button];
}

bool Joystick::HatButtonPress(int hatId,int button){
    if (!IsValidHat(hatId,button)){
        return false;
    }
    return Hats[hatId].keys[button] == JUST_PRESSED;
}
bool Joystick::HatButtonRelease(int hatId,int button){
    if (!IsValidHat(hatId,button)){
        return false;
    }
    return Hats[hatId].keys[button] == JUST_RELEASED;
}
bool Joystick::HatIsButtonPressed(int hatId,int button){
    if (!IsValidHat(hatId,button)){
        return false;
    }
    return Hats[hatId].keys[button] == JUST_PRESSED || Hats[hatId].keys[button] == PRESSED;
}
bool Joystick::HatIsButtonReleased(int hatId,int button){
    if (!IsValidHat(hatId,button)){
        return false;
    }
    return Hats[hatId].keys[button] == JUST_RELEASED || Hats[hatId].keys[button] == RELEASED;

}

void Joystick::Button(int button,int state){
    if (state == SDL_PRESSED){
        if (Buttons[button] != PRESSED){
            anyKeyPressed = button;
            Buttons[button] = JUST_PRESSED;
        }
    }else{
        if (Buttons[button] != RELEASED){
            Buttons[button] = JUST_RELEASED;
        }
    }
    if (HasCallButton){
        ButtonCallBack(m_id,button,Buttons[button]);
    }
}


void Joystick::MoveAxis(int axisId,int value){

    if (HasCallAxis){
        AxisCallBack(m_id,axisId,value);
    }
    Axis[axisId] = value;

}
void Joystick::MoveHat(int hatId,int value){
    if (HasCallHat){
        HatCallBack(m_id,hatId,value);
    }
    Hats[hatId].keyState = value;
}
void Joystick::MoveBall(int ballId,int vx,int vy){
    if (HasCallBall){
        BallCallBack(m_id,ballId,vx,vy);
    }
    Balls[ballId] = PointInt(vx,vy);
}


void Joystick::AddHatCallBack(std::function<void(int joyId,int hatId,int Value)> CallbackF){
    HasCallHat = true;
    HatCallBack = CallbackF;
};
void Joystick::ClearHatCallBack(){
    HasCallHat = false;
};
void Joystick::AddButtonCallBack(std::function<void(int joyId,int Button,InputState Value)> CallbackF){
    HasCallButton = true;
    ButtonCallBack = CallbackF;
}
void Joystick::ClearButtonCallBack(){
    HasCallButton = false;
};
void Joystick::AddAxisCallBack(std::function<void(int joyId,int axisId,int Value)> CallbackF){
    HasCallAxis = true;
    AxisCallBack = CallbackF;
};
void Joystick::ClearAxisCallBack(){
    HasCallAxis  = false;
};
void Joystick::AddBallCallBack(std::function<void(int joyId,int ballId,int ValueX,int ValueY)> CallbackF){
    HasCallBall = true;
    BallCallBack = CallbackF;
};
void Joystick::ClearBallCallBack(){
    HasCallBall = false;
};

void Joystick::ClearAllCallBacks(){
    ClearHatCallBack();
    ClearButtonCallBack();
    ClearAxisCallBack();
    ClearBallCallBack();
};

