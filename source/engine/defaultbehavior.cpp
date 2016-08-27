#include "defaultbehavior.hpp"
#include "../settings/configmanager.hpp"
#include "screenmanager.hpp"
#include "camera.hpp"
#include "music.hpp"
#include "sound.hpp"
#include "gamebase.hpp"


DefaultBehavior& DefaultBehavior::GetInstance(){
    static DefaultBehavior teste;
    return teste;
}

DefaultBehavior::DefaultBehavior(){

}

DefaultBehavior::~DefaultBehavior(){

}


bool DefaultBehavior::OnLoad(){
    return true;
}

void DefaultBehavior::OnClose(){
    Text::Clear();
    Music::Clear();
}

bool DefaultBehavior::OnResize(int newW,int newH){
    switch(ConfigManager::GetInstance().GetResizeAction()){
        case RESIZE_BEHAVIOR_NORESIZE:{
            if (newW != ConfigManager::GetInstance().GetScreenW() || newH != ConfigManager::GetInstance().GetScreenH()){
                ScreenManager::GetInstance().Resize(ConfigManager::GetInstance().GetScreenW(),ConfigManager::GetInstance().GetScreenH());
            }
            return false;
        }
        case RESIZE_BEHAVIOR_SCALE:
            ScreenManager::GetInstance().ResizeToScale(newW,newH,RESIZE_SCALE);
            break;
        case RESIZE_BEHAVIOR_SCALE_FREE:
            ScreenManager::GetInstance().ResizeToScale(newW,newH,RESIZE_FREE_SCALE);
            break;
        case RESIZE_BEHAVIOR_INCREASE:
            Camera::Resize(Point(newW,newH));
            break;
    }
    return true;
}

void DefaultBehavior::OnFocus(bool isFocused){

}


void DefaultBehavior::OnRestored(){

}
void DefaultBehavior::OnMinimized(){

}
