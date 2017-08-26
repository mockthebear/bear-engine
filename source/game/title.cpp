#include "title.hpp"
#include <sstream>
#include <cmath>

#include "title.hpp"

#include "../luasystem/luacaller.hpp"
#include "../luasystem/luatools.hpp"
#include "../engine/renderhelp.hpp"



Title::Title(){
    requestQuit = requestDelete = false;
    /*
        Loading main lua script
    */
    LuaCaller::LoadFile(LuaManager::L,"lua/test.lua");
    LuaCaller::Pcall(LuaManager::L);

    /*
        Starting camera
    */

    Camera::Initiate(Rect(0,0,SCREEN_SIZE_W,SCREEN_SIZE_H),128,200);
    Camera::speed = 0;
}


void Title::Begin(){
    /*
        Callin an lua function
    */

    LuaCaller::CallGlobalField(LuaManager::L,"onLoad");

}

Title::~Title(){
    Pool.ErasePools();
}


void Title::Update(float dt){
    /*
        Reset
    */
    if( InputManager::GetInstance().KeyPress(SDLK_r) ){
        requestDelete = true;
        Game::GetInstance()->AddState(new Title());
        return;
    }
    /*
        Process some inputs
    */
    Input();

    Pool.Update(dt);
    Map.clear();
    Pool.PreRender(Map);


    Camera::Update(dt);

    UpdateWindowses(dt);


}

void Title::Render(){
    RenderHelp::DrawSquareColorA(0,0,SCREEN_SIZE_W,SCREEN_SIZE_H,255,255,255,255);


    RenderInstances();

    RenderWindowses();
    Console::GetInstance().Render();

}

void Title::Input(){
    if (InputManager::GetInstance().ShouldQuit()){
        Console::GetInstance().AddText("DEVE QITAR");
        requestQuit = true;
    }


    if( InputManager::GetInstance().KeyPress(SDLK_ESCAPE) ) {
        Console::GetInstance().AddText("SCAPE");
        requestQuit = true;
    }
}
