#include "luagamestate.hpp"
#include "luaobject.hpp"
#include "luatools.hpp"
#include "../engine/gamebase.hpp"
#include "../settings/definitions.hpp"



LuaGameState::LuaGameState(){
    requestDelete = canClose = false;
    requestQuit = false;
}
LuaGameState::~LuaGameState(){

}

void LuaGameState::Setup(){
    BearEngine->AddState((DefinedState*)this,-1);
}
void LuaGameState::Update(float dt){
   LuaCaller::CallSelfField(LuaManager::L,this,"update",dt);
   requestDelete = canClose;
}
void LuaGameState::Render(){
    LuaCaller::CallSelfField(LuaManager::L,this,"render");
}
void LuaGameState::Begin(){
   Pool.Register<LuaObject>(1500);
   LuaCaller::CallSelfField(LuaManager::L,this,"begin");
}
void LuaGameState::End(){
    LuaCaller::CallSelfField(LuaManager::L,this,"finish");
}
void LuaGameState::Resume(GenericState *){
    LuaCaller::CallSelfField(LuaManager::L,this,"resume");
}
void LuaGameState::Pause(GenericState *){
    LuaCaller::CallSelfField(LuaManager::L,this,"pause");
}
