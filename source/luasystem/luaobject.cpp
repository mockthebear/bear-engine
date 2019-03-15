#include "../engine/renderhelp.hpp"
#include "../engine/gamebase.hpp"
#include "luaobject.hpp"


LuaObject::LuaObject(){
    OBJ_REGISTER(LuaObject);
    box         = Rect(0,0,1,1);
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



void LuaObject::Destroy(){

    Kill();
}

void LuaObject::Kill(){
    Active=false;
}

bool LuaObject::IsSolid(GameObject *obj){
    if (obj){
        return LuaCaller::CallSelfField(LuaManager::L,this,"OnIsSolid",obj);
    }else{
        return LuaCaller::CallSelfField(LuaManager::L,this,"OnIsSolid");
    }
}
void LuaObject::Update(float dt){
    LuaCaller::CallSelfField(LuaManager::L,this,"OnUpdate",dt);
}
void LuaObject::Render(){
    LuaCaller::CallSelfField(LuaManager::L,this,"OnRender");
}


bool LuaObject::Is(int is){
    return IsHash(is) || LuaCaller::CallSelfField(LuaManager::L,this,"OnIs",is);
}
bool LuaObject::IsDead(){
    return !Active || LuaCaller::CallSelfField(LuaManager::L,this,"OnIsDead");
}
void LuaObject::NotifyCollision(GameObject *obj){
    LuaCaller::CallSelfField(LuaManager::L,this,"OnNotifyCollision",obj);
}
void LuaObject::NotifyInPool(void*){
    LuaCaller::CallSelfField(LuaManager::L,this,"OnNotifyInPool",GetPoolIndex());
}
void LuaObject::NotifyDamage(GameObject *bj,int n){

    LuaCaller::CallSelfField(LuaManager::L,this,"OnNotifyDamage",bj);

}
