#include "../engine/renderhelp.hpp"
#include "../engine/gamebase.hpp"
#include "luaobject.hpp"

LuaObject::LuaObject(){
    OBJ_REGISTER(LuaObject);
    box         = Rect(0,0,1,1);
    ball        = Circle(0,0,1);
    rotation    = 0;
    Flags       = 0;
    round       = false;
    solid       = false;
    Depth       = 0;
    Active      = false;
}

LuaObject::~LuaObject(){

}


void LuaObject::Start(){
    Active = true;
}
void LuaObject::Stop(){
    Active = false;
}
void LuaObject::Update(float dt){

}
void LuaObject::Render(){

}

bool LuaObject::IsInScreen(Point& p){
    return true;
};

bool LuaObject::Is(int is){
    return IsHash(is);
}
bool LuaObject::IsDead(){
    return !Active;
}
void LuaObject::NotifyCollision(GameObject *obj){

}
void LuaObject::NotifyDamage(GameObject *bj,int n){

}
