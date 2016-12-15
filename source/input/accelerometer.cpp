#include "accelerometer.hpp"
#include "../engine/renderhelp.hpp"

Accelerometer::Accelerometer(){
    m_joy = NULL;
    m_factor = 0.2f;
}
void Accelerometer::Set(Joystick *j){
    m_joy = j;
}
void Accelerometer::Render(PointInt pos){
    if (m_joy != NULL){
        RenderHelp::DrawSquareColorA(pos.x-100,pos.y,200,16 + 17*2,255,255,255,100);
        RenderHelp::DrawSquareColorA(pos.x,pos.y,GetXPercent() * 200.0f,16,255,0,0,100);
        RenderHelp::DrawSquareColorA(pos.x,pos.y+17,GetYPercent() * 200.0f,16,0,255,0,100);
        RenderHelp::DrawSquareColorA(pos.x,pos.y+17+17,GetZPercent() * 200.0f,16,0,0,255,100);
    }
}
void Accelerometer::Update(float dt){
    if (m_joy != NULL){
        value.x = (1-m_factor)*value.x  + m_joy->GetAxis(0)*m_factor;
        value.y = (1-m_factor)*value.y  + m_joy->GetAxis(1)*m_factor;
        value.z = (1-m_factor)*value.z  + m_joy->GetAxis(2)*m_factor;
    }
}

void Accelerometer::SetFilterFactor(float factor){
    m_factor = std::max(0.0f,factor);
    m_factor = std::min(1.0f,m_factor);
}
Point3 Accelerometer::GetPercent(){
    Point3 ret;
    if (m_joy == NULL){
        return ret;
    }
    ret.x = value.x/32767.0f;
    ret.y = value.y/32767.0f;
    ret.z = value.z/32767.0f;
    return ret;
}
float Accelerometer::GetXPercent(){
    if (m_joy == NULL){
        return 0;
    }
    return value.x/32767.0f;
}
float Accelerometer::GetYPercent(){
    if (m_joy == NULL){
        return 0;
    }
    return value.y/32767.0f;
}
float Accelerometer::GetZPercent(){
    if (m_joy == NULL){
        return 0;
    }
    return value.z/32767.0f;
}

uint16_t Accelerometer::GetX(){
    if (m_joy == NULL){
        return 0;
    }
    return value.x;
}
uint16_t Accelerometer::GetY(){
    if (m_joy == NULL){
        return 0;
    }
    return value.y;
}
uint16_t Accelerometer::GetZ(){
    if (m_joy == NULL){
        return 0;
    }
    return value.z;
}
