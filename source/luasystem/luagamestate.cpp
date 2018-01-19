#include "luagamestate.hpp"
#include "luaobject.hpp"
#include "luatools.hpp"
#include "../engine/gamebase.hpp"
#include "../settings/definitions.hpp"

#ifndef __EMSCRIPTEN__

LuaGameState::LuaGameState(){
    requestDelete = canClose = false;
    requestQuit = false;
    //This state belongs to the previous state!

    //Pool.Register<LuaObject>(1000);

    other = uint64_t(&Game::GetCurrentState()) ;

}
LuaGameState::~LuaGameState(){

}

void LuaGameState::Setup(){
    BearEngine->AddState((DefinedState*)this,-1);
    other = uint64_t(this) ;
}
void LuaGameState::Update(float dt){
   LuaCaller::CallOtherField(LuaManager::L,other,this,"OnUpdate",dt);
   requestDelete = canClose;

   UpdateInstances(dt);

   UpdateWindowses(dt);

}
void LuaGameState::Render(){

    LuaCaller::CallOtherField(LuaManager::L,other,this,"OnRender");
    RenderInstances();

    RenderWindowses();
}
void LuaGameState::Begin(){
   Pool.Register<LuaObject>(1500);
   LuaCaller::CallOtherField(LuaManager::L,other,this,"OnBegin");
}
void LuaGameState::End(){
    LuaCaller::CallOtherField(LuaManager::L,other,this,"OnFinish");
}
void LuaGameState::Resume(GenericState *){
    LuaCaller::CallOtherField(LuaManager::L,other,this,"OnResume");
}
void LuaGameState::Pause(GenericState *){
    LuaCaller::CallOtherField(LuaManager::L,other,this,"OnPause");
}

#endif
