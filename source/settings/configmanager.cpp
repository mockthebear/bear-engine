#include "configmanager.hpp"
#include "../engine/gamebase.hpp"
#include "../engine/renderhelp.hpp"
#include "../framework/resourcemanager.hpp"
#include "../performance/console.hpp"

int ConfigManager::MaxFps = MAXFPS;
float ConfigManager::MinimumDT = 10.32;
ConfigManager::ConfigManager(){
    m_screenSize.x=SCREEN_SIZE_W;
    m_screenSize.y=SCREEN_SIZE_H;
    pause = false;
    m_rbheavior = RESIZE_BEHAVIOR_SCALE;

    MinimumDT = 10.33;

}

void ConfigManager::SetWindowIcon(std::string icon,ColorReplacer &r){
    SDL_Surface *surf = nullptr;
    if (icon.find(":")!=std::string::npos){
        SDL_RWops* rw = ResourceManager::GetInstance().GetFile(icon);
        if (rw){
            surf = IMG_Load_RW(rw, 1);
        }else{
            Console::GetInstance().AddTextInfo(utils::format("Cannot open the rw file %s",icon));
        }
    }else{
        surf = IMG_Load(icon.c_str());
    }
    if (not surf){
        Console::GetInstance().AddTextInfo(utils::format("The surface %s cannot be loaded because %s",icon,SDL_GetError()));
    }else{
        Uint32* pixels = (Uint32*)surf->pixels;
        int pixelCount = ( surf->pitch / 4 ) * surf->h;
        Uint8 R,G,B,A;
        std::map<uint32_t,std::tuple<bool,uint32_t>> replacer;
        SDL_PixelFormat* ScreenPixelFormat = SDL_GetWindowSurface( Game::GetInstance()->GetWindow() )->format;
        for( int i = 0; i < pixelCount; ++i ){
            SDL_GetRGBA(pixels[ i ],ScreenPixelFormat,&R,&G,&B,&A);
            Uint32 theColor = RenderHelp::FormatRGBA(R,B,G,A);
            pixels[i] = r.Get(theColor);
        }

        SDL_SetWindowIcon(Game::GetInstance()->GetWindow(), surf);
    }
}
void ConfigManager::SetWindowIcon(std::string icon){
    SDL_Surface *surf = nullptr;
    if (icon.find(":")!=std::string::npos){
        SDL_RWops* rw = ResourceManager::GetInstance().GetFile(icon);
        if (rw){
            surf = IMG_Load_RW(rw, 1);
        }else{
            Console::GetInstance().AddTextInfo(utils::format("Cannot open the rw file %s",icon));
        }
    }else{
        surf = IMG_Load(icon.c_str());

    }
    if (not surf){
        Console::GetInstance().AddTextInfo(utils::format("The surface %s cannot be loaded because %s",icon,SDL_GetError()));
    }else{
        SDL_SetWindowIcon(Game::GetInstance()->GetWindow(), surf);
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
