#include "title.hpp"
#include <sstream>
#include <cmath>

#include "title.hpp"

#include "../luasystem/luacaller.hpp"
#include "../luasystem/luatools.hpp"
#include "../engine/renderhelp.hpp"


#include "../tests/testthread.hpp"
#include "../tests/testfiles.hpp"


Title::Title(){
    requestQuit = requestDelete = false;
    /*
        Loading main lua script
    */
    #ifndef __EMSCRIPTEN__
    LuaCaller::LoadFile(LuaManager::L,"lua/test.lua");
    LuaCaller::Pcall(LuaManager::L);
    #endif // __EMSCRIPTEN__

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
    #ifndef __EMSCRIPTEN__
    LuaCaller::CallGlobalField(LuaManager::L,"onLoad");
    #endif // __EMSCRIPTEN__



}

Title::~Title(){
    Pool.ErasePools();
}


void Title::Update(float dt){
    /*
        Reset
    */
    if( InputManager::GetInstance().KeyPress(SDLK_t) ){

        #ifdef THREADPOOLTEST
        Game::GetInstance()->AddState(new Test_Threadpool());
        #endif // THREADPOOLTEST

        Game::GetInstance()->AddState(new Test_Files());


        //requestDelete = true;
        return;
    }

    Input();

    Pool.Update(dt);
    Map.clear();
    Pool.PreRender(Map);


    Camera::Update(dt);

    UpdateWindowses(dt);


}

void Title::Render(){
    RenderHelp::DrawSquareColor(0,0,SCREEN_SIZE_W,SCREEN_SIZE_H,255,255,255,255);


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
        requestDelete = true;
    }
}
