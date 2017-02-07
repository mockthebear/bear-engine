#include "configmanager.hpp"
#include "../engine/gamebase.hpp"
#include "../engine/renderhelp.hpp"
#include "../framework/resourcemanager.hpp"
#include "../performance/console.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "../image/stb_image.h"


int ConfigManager::MaxFps = MAXFPS;
float ConfigManager::MinimumDT = 10.32f;
ConfigManager::ConfigManager(){
    m_screenSize.x=SCREEN_SIZE_W;
    m_screenSize.y=SCREEN_SIZE_H;
    pause = false;
    m_rbheavior = RESIZE_BEHAVIOR_SCALE;

    MinimumDT = 10.33f;

}

void ConfigManager::SetWindowIcon(std::string icon,ColorReplacer &r){

}
void ConfigManager::SetWindowIcon(std::string icon){
    SDL_Surface *surf = nullptr;
    unsigned char *imageData = nullptr;
    uint64_t rsize;
    int sizeX,sizeY,comp;
    if (icon.find(":")!=std::string::npos){
        SDL_RWops* rw = ResourceManager::GetInstance().GetFile(icon);
        if (rw){
            char *res = ResourceManager::GetFileBuffer(rw,rsize);
            imageData = stbi_load_from_memory((stbi_uc*)res,rsize,&sizeX,&sizeY,&comp,STBI_rgb_alpha);
            ResourceManager::ClearFileBuffer(res);
        }else{
            Console::GetInstance().AddTextInfo(utils::format("Cannot open the rw file %s",icon));
        }
    }else{
        SDL_RWops* rw = SDL_RWFromFile(icon.c_str(), "rb");
        if (!rw){
            Console::GetInstance().AddTextInfo(utils::format("Cannot preload sprite [%s] because: %s",icon.c_str(),SDL_GetError()));
            return;
        }
        char *res = ResourceManager::GetFileBuffer(rw,rsize);
        imageData = stbi_load_from_memory((stbi_uc*)res,rsize,&sizeX,&sizeY,&comp,STBI_rgb_alpha);
        ResourceManager::ClearFileBuffer(res);
        SDL_RWclose(rw);
    }
    if (!imageData){
        Console::GetInstance().AddTextInfo(utils::format("The surface %s cannot be loaded because %s",icon,SDL_GetError()));
        return;
    }else{
        #if SDL_BYTEORDER == SDL_BIG_ENDIAN
        surf = SDL_CreateRGBSurface(0, sizeX,sizeY, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
        #else
        surf = SDL_CreateRGBSurface(0, sizeX,sizeY, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
        #endif
        int pixelCount = (sizeX*sizeY);
        for( int i = 0; i < pixelCount; ++i ){
            ((Uint32*)surf->pixels)[i] = ((Uint32*)imageData)[i];
        }
        if (!surf){
            Console::GetInstance().AddTextInfo(utils::format("The surface %s cannot be loaded because %s",icon,SDL_GetError()));
        }else{
            SDL_SetWindowIcon(Game::GetInstance()->GetWindow(), surf);
        }
        stbi_image_free(imageData);
    }



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
