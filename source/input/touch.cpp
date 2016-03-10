#include "touch.hpp"
#include "../engine/renderhelp.hpp"
#include "../engine/screenmanager.hpp"

Finger::Finger(){
    m_state = RELEASED;
    m_tapState = RELEASED;

}

void Finger::Update(float dt){
    if (tapping){
        tapDelay -= dt;
    }
    m_tapState = RELEASED;
    if (m_state == JUST_PRESSED){
        tapping = true;
        tapDelay = 2.0;
        m_state = PRESSED;
    }
    if (m_state == JUST_RELEASED){
        if (tapping && tapDelay > 0){
            m_tapState = PRESSED;
        }
        m_state = RELEASED;
    }
}
void Finger::Render(){
    //RenderHelp::DrawSquareColorUnscaled(m_position.x-4,m_position.y-4,8,8,0,255,m_state == PRESSED ? 255 : 0,200);
    RenderHelp::DrawSquareColorA(m_position.x-4,m_position.y-4,8,8,0,255,m_state == PRESSED ? 255 : 0,200);
}

Touch::Touch(){
    m_touchedCount = 1;
    m_offset = Point(0,0);

}


Touch::~Touch(){

}




void Touch::Update(float dt){
    m_hasTap = 0;
    for ( int i=0;i<MAX_TOUCHSCREEN_FINGERS;i++ ){
        fingers[i].Update(dt);
        if (fingers[i].m_tapState == PRESSED){
            m_hasTap = i+1;
        }
    }




}

void Touch::UpdateTouchPosition(int id,float x,float y,float pressure){
    if (id >= MAX_TOUCHSCREEN_FINGERS)
        return;
    m_lastFinger = id;
    m_touchedCount = std::max(1,id);
    fingers[id].m_truePosition.x = x;
    fingers[id].m_truePosition.y = y;
    PointInt size = ScreenManager::GetInstance().GetScreenSize();
    float sh = ScreenManager::GetInstance().GetScaleRatioH();
    float sw = ScreenManager::GetInstance().GetScaleRatioW();
    fingers[id].m_position.x = (x * (size.x/sw) ) - ScreenManager::GetInstance().GetOffsetW() + m_offset.x;
    fingers[id].m_position.y = (y * (size.y/sh) ) - ScreenManager::GetInstance().GetOffsetH() + m_offset.y;
    fingers[id].m_ressure = pressure;
}

void Touch::UpdateTouchStatus(int id,bool isDown){
    if (id >= MAX_TOUCHSCREEN_FINGERS)
        return;
    m_touchedCount = std::max(1,id);
    fingers[id].m_state = (isDown ? JUST_PRESSED : JUST_RELEASED);
}
PointInt Touch::GetFingerPosition(int id){
    if (id == -1){
        id = m_lastFinger;
        if (id == -1){
            return PointInt(0,0);
        }
    }
    return fingers[id].m_position;
}

TouchInfo Touch::GetFinger(int id){
    TouchInfo info;
    if (id == -1){
        id = m_lastFinger;
        if (id == -1){
            return info;
        }
    }
    info.x          =  fingers[id].m_position.x;
    info.y          =  fingers[id].m_position.y;
    info.finger     =  id;
    info.state      =  fingers[id].m_state;
    info.pressure   =  fingers[id].m_ressure;
    info.x_f        =  fingers[id].m_truePosition.x;
    info.y_f        =  fingers[id].m_truePosition.y;
    return info;
}
void Touch::Render(){
    for ( int i=0;i<m_touchedCount;i++ ){
        fingers[i].Render();
    }
}
