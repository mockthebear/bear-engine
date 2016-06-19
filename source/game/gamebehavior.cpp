#include "gamebehavior.hpp"

#include "title.hpp"
#include "../engine/defaultbehavior.hpp"
#include "../engine/gamebase.hpp"
#include "../performance/console.hpp"
#include "../framework/resourcemanager.hpp"


#include <string>



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


bool GameBehavior::OnLoad(){
    //Console::GetInstance().Begin();
    if (!ResourceManager::GetInstance().Load("data/packedFiles.burr","assets")){
        std::cout << "Fail\n";
        getchar();
    }


    PointInt P = ScreenManager::GetInstance().GetDisplaySize();
    PointInt P2 = ConfigManager::GetInstance().GetScreenSize();

    Console::GetInstance().AddText(utils::format("Started with display %d x %d",(int)P.x,(int)P.y));
    Console::GetInstance().AddText(utils::format("Started with screen %d x %d",(int)P2.x,(int)P2.y));
    Console::GetInstance().AddText(utils::format("There is %d joysticks",InputManager::GetInstance().GetJoystickCount()));

    Console::GetInstance().AddText("Starting state.");
    Game::GetInstance()->AddState(new Title());
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
