#include "configmanager.hpp"
#include "../engine/gamebase.hpp"
#include "../performance/console.hpp"

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

void ConfigManager::DisplayArgs(){
    if (args.size() == 0){
        Console::GetInstance().AddText("There are no args\n");
    }else{
        int counter = 0;
        for (auto &it : args){
            Console::GetInstance().AddText(utils::format("Argument [%d]: %s",counter,it.c_str()));
            counter++;
        }
    }
}
void ConfigManager::RegisterArgs(int argc,char *argv[]){
    for (int i=0;i<argc;i++){
        args.emplace_back(std::string(argv[i]));
    }
}
