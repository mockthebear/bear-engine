#include "shooteffect.hpp"
#include "../engine/renderhelp.hpp"
#include <math.h>
#include <iostream>
#define PI 3.1415926
ShootEffect::ShootEffect(GameObject *b,Point p2_,Sprite *spr,float timeLimit_,bool ends){
    sp = spr;
    Follow=b;
    p2 = p2_;
    OH=sp->GetHeight();
    OW=sp->GetWidth();
    box = Rect(Follow->box.x,Follow->box.y,sp->GetHeight(),sp->GetWidth());
    float angle = atan2(p2.y-box.GetYCenter(),p2.x-box.GetXCenter());
    distance = box.GetCenter().getDistance(&p2);
    angle = (angle > 0 ? angle : (2*PI + angle)) * 360 / (2*PI);
    rotation = 180;
    Depth = 1;
    timeLimit = timeLimit_;
    oneTimeOnly = ends;
    sp->Update(0);
    endTime.Update(0);
    leang = 180;
}
ShootEffect::~ShootEffect(){
    delete sp;
}
void ShootEffect::Update(float dt){
    p = Point(Follow->box.GetXCenter()+8,Follow->box.GetYCenter()+8);
    box.x = (p.x+p2.x)/2.0;
    box.y = (p.y+p2.y)/2.0;

    float angle = atan2(p.y-p2.y,p.x-p2.x);
    distance = p.getDistance(&p2);
    angle = (angle > 0 ? angle : (2.0*PI + angle)) * 360.0 / (2.0*PI);
    rotation = angle;

    sp->Update(dt);
    endTime.Update(dt);
}

void ShootEffect::Render(){

    sp->SetClip(0,0,sp->GetHeight(),std::min(std::max(distance,(float)sp->GetWidth() ),distance ) );
    sp->Render(Camera::AdjustPosition(box,- distance/2,- sp->GetHeight()/2),rotation);
    //sp->Render(box.x-Camera::pos.x- distance/2,box.y-Camera::pos.y- sp->GetHeight()/2,rotation);
    /*RenderHelp::DrawSquareColorA(p.x-Camera::pos.x,p.y-Camera::pos.y,8,8,255,0,0,255);
    RenderHelp::DrawSquareColorA(p2.x-Camera::pos.x,p2.y-Camera::pos.y,8,8,255,0,0,255);
    RenderHelp::DrawSquareColorA(box.x-Camera::pos.x,box.y-Camera::pos.y,8,8,255,0,0,255);*/
}

bool ShootEffect::IsDead(){
    return (!oneTimeOnly || endTime.Get() >= timeLimit);
}
bool ShootEffect::Is(int p){
    return p == OBJ_STILLANIMATION;
}
