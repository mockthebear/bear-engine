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
    #ifndef DISABLE_LUAINTERFACE
    if (obj){
        return LuaCaller::CallSelfField(LuaManager::L,this,"OnIsSolid",obj);
    }else{
        return LuaCaller::CallSelfField(LuaManager::L,this,"OnIsSolid");
    }
    #endif // DISABLE_LUAINTERFACE
}
void LuaObject::Update(float dt){
    #ifndef DISABLE_LUAINTERFACE
    LuaCaller::CallSelfField(LuaManager::L,this,"OnUpdate",dt);
    #endif // DISABLE_LUAINTERFACE
}
void LuaObject::Render(){
    #ifndef DISABLE_LUAINTERFACE
    LuaCaller::CallSelfField(LuaManager::L,this,"OnRender");
    #endif // DISABLE_LUAINTERFACE
}


bool LuaObject::Is(int is){
    #ifndef DISABLE_LUAINTERFACE
    return IsHash(is) || LuaCaller::CallSelfField(LuaManager::L,this,"OnIs",is);
    #else
    return IsHash(is);
    #endif
}
bool LuaObject::IsDead(){
    #ifndef DISABLE_LUAINTERFACE
    return !Active || LuaCaller::CallSelfField(LuaManager::L,this,"OnIsDead");
    #else
    return !Active;
    #endif // DISABLE_LUAINTERFACE
}
void LuaObject::NotifyCollision(GameObject *obj){
    #ifndef DISABLE_LUAINTERFACE
    LuaCaller::CallSelfField(LuaManager::L,this,"OnNotifyCollision",obj);
    #endif // DISABLE_LUAINTERFACE
}
void LuaObject::NotifyInPool(void*){
    #ifndef DISABLE_LUAINTERFACE
    LuaCaller::CallSelfField(LuaManager::L,this,"OnNotifyInPool",GetPoolIndex());
    #endif // DISABLE_LUAINTERFACE
}
void LuaObject::NotifyDamage(GameObject *bj,int n){
    #ifndef DISABLE_LUAINTERFACE
    LuaCaller::CallSelfField(LuaManager::L,this,"OnNotifyDamage",bj);
    #endif // DISABLE_LUAINTERFACE

}
