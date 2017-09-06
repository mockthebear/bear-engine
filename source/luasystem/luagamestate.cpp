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
   LuaCaller::CallOtherField(LuaManager::L,other,this,"update",dt);
   requestDelete = canClose;

   UpdateInstances(dt);

   UpdateWindowses(dt);

}
void LuaGameState::Render(){

    LuaCaller::CallOtherField(LuaManager::L,other,this,"render");
    RenderInstances();

    RenderWindowses();
}
void LuaGameState::Begin(){
   Pool.Register<LuaObject>(1500);
   LuaCaller::CallOtherField(LuaManager::L,other,this,"begin");
}
void LuaGameState::End(){
    LuaCaller::CallOtherField(LuaManager::L,other,this,"finish");
}
void LuaGameState::Resume(GenericState *){
    LuaCaller::CallOtherField(LuaManager::L,other,this,"resume");
}
void LuaGameState::Pause(GenericState *){
    LuaCaller::CallOtherField(LuaManager::L,other,this,"pause");
}
