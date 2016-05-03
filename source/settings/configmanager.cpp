#include "configmanager.hpp"
#include "../engine/gamebase.hpp"

int ConfigManager::MaxFps = MAXFPS;
ConfigManager::ConfigManager(){
    m_screenSize.x=SCREEN_SIZE_W;
    m_screenSize.y=SCREEN_SIZE_H;
    pause = false;
    m_rbheavior = RESIZE_BEHAVIOR_SCALE;

}


ConfigManager::~ConfigManager(){

}


ConfigManager& ConfigManager::GetInstance(){
    static ConfigManager teste;
    return teste;
}

void ConfigManager::SetScreenSize(int w,int h){
    m_screenSize.x=w;
    m_screenSize.y=h;
    if (Game::GetInstance()->IsGameBegin()){
        ScreenManager::GetInstance().NotifyResized();
    }
};

