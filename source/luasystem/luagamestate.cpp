#include "luagamestate.hpp"
#include "luaobject.hpp"
#include "luatools.hpp"
#include "../engine/gamebase.hpp"
#include "../settings/definitions.hpp"


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
   #ifndef DISABLE_LUAINTERFACE
   LuaCaller::CallOtherField(LuaManager::L,other,this,"OnUpdate",dt);
   #endif // DISABLE_LUAINTERFACE
   requestDelete = canClose;

   UpdateInstances(dt);

   UpdateWindowses(dt);

}
void LuaGameState::Render(){
    #ifndef DISABLE_LUAINTERFACE
    LuaCaller::CallOtherField(LuaManager::L,other,this,"OnRender");
    #endif // DISABLE_LUAINTERFACE
    RenderInstances();

    RenderWindowses();
}
void LuaGameState::Begin(){
   Pool.Register<LuaObject>(1500);
   #ifndef DISABLE_LUAINTERFACE
   LuaCaller::CallOtherField(LuaManager::L,other,this,"OnBegin");
   #endif // DISABLE_LUAINTERFACE
}
void LuaGameState::End(){
    #ifndef DISABLE_LUAINTERFACE
    LuaCaller::CallOtherField(LuaManager::L,other,this,"OnFinish");
    #endif // DISABLE_LUAINTERFACE
}
void LuaGameState::Resume(GenericState *){
    #ifndef DISABLE_LUAINTERFACE
    LuaCaller::CallOtherField(LuaManager::L,other,this,"OnResume");
    #endif // DISABLE_LUAINTERFACE
}
void LuaGameState::Pause(GenericState *){
    #ifndef DISABLE_LUAINTERFACE
    LuaCaller::CallOtherField(LuaManager::L,other,this,"OnPause");
    #endif // DISABLE_LUAINTERFACE
}

