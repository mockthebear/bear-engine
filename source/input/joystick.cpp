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
        anyKeyPressed= false;
        HasCallButton= false;
        HasCallAxis  = false;
        HasCallBall  = false;
        HasCallHat   = false;
        Axis = new int[m_axes];
        Hats = new int[m_hats];
        Balls = new PointInt[m_balls];

        for (int i=0;i<m_axes;i++)
            Axis[i] = 0;
    }
}
Joystick::~Joystick(){
    delete Axis;
    delete Balls;
}
void Joystick::Close(){
    if (IsWorking() and SDL_JoystickGetAttached(m_joyHandler)) {
        SDL_JoystickClose(m_joyHandler);
        m_joyHandler = NULL;
    }
}

void Joystick::Render(int x,int y){
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
    anyKeyPressed = false;
    for ( auto it = Buttons.begin(); it != Buttons.end(); ++it ){
        if (it->second == JUST_PRESSED){
            it->second = PRESSED;
        }
        if (it->second == JUST_RELEASED){
            it->second = RELEASED;
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
    return Buttons[key] == JUST_PRESSED or Buttons[key] == PRESSED;
}
bool Joystick::IsButtonUp(int key){
    return Buttons[key] == JUST_RELEASED or Buttons[key] == RELEASED;
}

void Joystick::Button(int button,int state){
    if (state == SDL_PRESSED){
        if (Buttons[button] != PRESSED){
            anyKeyPressed = true;
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
    Hats[hatId] = value;
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

