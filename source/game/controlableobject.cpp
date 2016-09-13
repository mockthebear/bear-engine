#include "controlableobject.hpp"


#include "../engine/light.hpp"
#include "../engine/renderhelp.hpp"
#include "../engine/gamebase.hpp"


ControlableObject::ControlableObject(){
    OBJ_REGISTER(ControlableObject);
    box.x = 0;
    box.y = 0;
    box.w = 64;
    box.h = 64;

    isCreated = false;
}
ControlableObject::ControlableObject(int x,int y){
    OBJ_REGISTER(ControlableObject);
    rotation = 0;
    box.x = x;
    box.y = y;
    box.w = 64;
    box.h = 64;
    isCreated = true;
    solid=true;
    speed = Point(8-rand()%16,8-rand()%16);
    sp = Game::GetCurrentState().Assets.make<Sprite>("data/bear.png",true);
    wobble = 0;
}

ControlableObject::~ControlableObject(){

}


void ControlableObject::Update(float dt){
    Rect aux = box;
    aux.x += speed.x*dt;
    aux.y += speed.y*dt;
    wobble += 0.7f*dt;
    if (aux.x <= 0 or aux.x >= 800 - box.w){
        speed.x *= -1;
        aux = box;
        aux.x += speed.x*dt;
    }
    if (aux.y <= 0 or aux.y >= 600 - box.h){
        speed.y *= -1;
        aux = box;
        aux.y += speed.y*dt;
    }
    float si = sin(wobble);
    float co = cos(wobble);
    si = si < 0 ? -si : si;
    co = co < 0 ? -co : co;
    sp.SetScale(Point(1 + si*0.2,1 + co*0.2));
    sp.SetCenter(Point(sp.GetWidth()/2,sp.GetHeight()/2));
    box = aux;
    Light::GetInstance()->AddLightM(box.x+32,box.y+32,170);
}

void ControlableObject::Render(){
    sp.Render(Camera::AdjustPosition(box));
}

bool ControlableObject::Is(int is){
    return is == GetHash();
}
bool ControlableObject::IsDead(){
    return !isCreated;
}
void ControlableObject::NotifyCollision(GameObject *obj){
    /*
        I know this is useless because of the groups.
        But its an example of dynamic checking
    */
    if (obj != NULL && TypeChecker::GetInstance().checkType(obj,TYPEOF(ControlableObject)) ){
        speed.x *= -1;
        speed.y *= -1;
    }
}
void ControlableObject::NotifyDamage(GameObject *bj,int n){
    isCreated = false;
}

