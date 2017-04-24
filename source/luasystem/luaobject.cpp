#include "../engine/renderhelp.hpp"
#include "../engine/gamebase.hpp"
#include "luaobject.hpp"



LuaObject::LuaObject(){
    OBJ_REGISTER(LuaObject);
    box         = Rect(0,0,1,1);
    ball        = Circle(0,0,1);
    rotation    = 0;
    Flags       = 0;
    isround       = false;
    solid       = false;
    Depth       = 0;
    Active      = false;
    forceUpdate = false;
    forceRender = false;
    perspective = 0;

}
LuaObject::LuaObject(int x,int y):LuaObject(){
    OBJ_REGISTER(LuaObject);
    box         = Rect(x,y,1,1);
    Active      = true;
}

LuaObject::~LuaObject(){

}



void LuaObject::Update(float dt){
    LuaCaller::CallSelfField(LuaData::L,(uint64_t)this,"Update",dt);
}
void LuaObject::Render(){
    LuaCaller::CallSelfField(LuaData::L,(uint64_t)this,"Render");
}

bool LuaObject::IsInScreen(Point& p){
    return true;
};

bool LuaObject::Is(int is){
    return IsHash(is) || LuaCaller::CallSelfField(LuaData::L,(uint64_t)this,"Is",is);
}
bool LuaObject::IsDead(){
    return !Active || LuaCaller::CallSelfField(LuaData::L,(uint64_t)this,"IsDead");
}
void LuaObject::NotifyCollision(GameObject *obj){
    LuaCaller::CallSelfField(LuaData::L,(uint64_t)this,"NotifyCollision",(uint64_t)obj);
}
void LuaObject::NotifyDamage(GameObject *bj,int n){
    LuaCaller::CallSelfField(LuaData::L,(uint64_t)this,"NotifyDamage",(uint64_t)bj,n);
}
