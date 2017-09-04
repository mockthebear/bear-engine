#include "gamebehavior.hpp"

#include "title.hpp"
#include "../engine/defaultbehavior.hpp"
#include "../engine/gamebase.hpp"
#include "../performance/console.hpp"
#include "../framework/resourcemanager.hpp"
#include "../framework/threadpool.hpp"


#include <string>


#include "../tests/testthread.hpp"
#include "../tests/testfiles.hpp"




GameBehavior& GameBehavior::GetInstance(){
    static GameBehavior teste;
    return teste;
}


GameBehavior::GameBehavior(){
    /* Create here the default */
    DefaultBehavior::GetInstance();


}

GameBehavior::~GameBehavior(){

}


void GameBehavior::Begin(){
    Game::startFlags = 0;
    Game::startFlags |= BEAR_FLAG_START_LUA;
    Game::startFlags |= BEAR_FLAG_START_SDL;
    Game::startFlags |= BEAR_FLAG_START_SCREEN;
    Game::startFlags |= BEAR_FLAG_START_INPUT;
    //Game::startFlags |= BEAR_FLAG_LOAD_BASEFILES;
    Game::startFlags |= BEAR_FLAG_START_CONSOLE;
    //Game::startFlags |= BEAR_FLAG_START_CONSOLEGRAPHICAL;
    Game::startFlags |= BEAR_FLAG_START_TTF;
}
bool GameBehavior::OnLoad(){
    /*PointInt P = ScreenManager::GetInstance().GetDisplaySize();
    PointInt P2 = ConfigManager::GetInstance().GetScreenSize();

    bear::out << "Supported main display is "<< P.x << " x "<< P.y << "\n";
    bear::out << "Started with screen "<< P2.x << " x "<< P2.y << "\n";

    bear::out << "Starting state.\n";*/


    #ifdef THREADPOOLTEST
    Game::GetInstance()->AddState(new Test_Threadpool());
    #endif // THREADPOOLTEST

    Game::GetInstance()->AddState(new Test_Files());

    return DefaultBehavior::GetInstance().OnLoad();
}

void GameBehavior::OnClose(){
    ResourceManager::GetInstance().Erase("assets");
    DefaultBehavior::GetInstance().OnClose();
}



bool GameBehavior::OnResize(int newW,int newH){
    return DefaultBehavior::GetInstance().OnResize(newW,newH);
}


void GameBehavior::OnFocus(bool hasFocus){

    DefaultBehavior::GetInstance().OnFocus(hasFocus);
}

void GameBehavior::OnRestored(){

    DefaultBehavior::GetInstance().OnRestored();
}
void GameBehavior::OnMinimized(){

    DefaultBehavior::GetInstance().OnMinimized();
}
