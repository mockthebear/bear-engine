#include "music.hpp"
#include "gamebase.hpp"
#include "../framework/dirmanager.hpp"
#include "../framework/resourcemanager.hpp"
#include "../performance/console.hpp"


std::unordered_map<std::string, Mix_Music*> Music::assetTable;

uint8_t Music::MasterVolume = MIX_MAX_VOLUME;
Music::Music(){
    music = NULL;
    working = BearEngine->AudioWorking();
}

Music::Music(char *s):Music(){
    Open((const char*)s);
}

Music::Music(const char *s):Music(){
    Open(s);
}
bool Music::SetMasterVolume(uint8_t vol){
    MasterVolume = std::min((uint8_t)MIX_MAX_VOLUME,vol);
    Mix_VolumeMusic(MasterVolume);
    return true;

}
void Music::SetVolume(int vol){
    if (BearEngine->AudioWorking())
        Mix_VolumeMusic(vol);
}
void Music::Open(SDL_RWops* file,std::string name){
    if (!ConfigManager::GetInstance().IsWorkingAudio())
        return;
    if (assetTable.find(name) == assetTable.end() ){
        #ifdef __EMSCRIPTEN__
        Console::GetInstance().AddTextInfo("Emscripten still uses mixer 1.0. Mix_LoadMUS_RW not suported.");
        std::string newName = name;
        char *c = (char*)newName.c_str();
        for (int i=0;i<newName.size();i++){
            if (c[i] == ':'){
                c[i] = '/';
                break;
            }
        }
        Console::GetInstance().AddText(utils::format("Trying instead to load [%s]",newName.c_str()));
        Open(newName.c_str());
        #else
        Mix_Music *aux_music = Mix_LoadMUS_RW(file, 1);
        if (aux_music){
            assetTable[name] = aux_music;
            music = aux_music;
        }else{
            Console::GetInstance().AddTextInfo(utils::format("Cannot load rwop music named %s",name) );
            music = nullptr;
        }
        #endif
    }else{
        std::unordered_map<std::string, Mix_Music*>::iterator it = assetTable.find(name);
        music = (*it).second;
    }
}
void Music::Open(const char *s){
    std::string stdnamee(s);
    file = stdnamee;
    if (stdnamee.find(":")!=std::string::npos){
        Open(ResourceManager::GetInstance().GetFile(stdnamee),stdnamee);
        return;
    }
    stdnamee = DirManager::AdjustAssetsPath(stdnamee);
    if (!working)
        return;
    if (assetTable[stdnamee]){
        music = assetTable[stdnamee];
    }else{
        music = Mix_LoadMUS(stdnamee.c_str());
        if (music){
            assetTable[stdnamee] = music;
        }else{
            bear::out << "[Music::Open] Cannot play "<<stdnamee<<" because : "<< Mix_GetError() <<"\n";
        }
    }
    if (IsOpen()){
        file = s;
    }
}

void Music::FadeOut(int ms){
    if (BearEngine->AudioWorking())
        Mix_FadeOutMusic(ms);
}

void Music::FadeIn(int loop,int ms){
    if (!BearEngine->AudioWorking())
        return;
    if (IsOpen())
        Mix_FadeInMusic(music,loop,ms);
}
bool Music::Rewind(){
    if (BearEngine->AudioWorking())
        Mix_RewindMusic();
	return true;
}
bool Music::Preload(const char *s){
    if (!BearEngine->AudioWorking())
        return false;
    std::string stdnamee(s);
    stdnamee = DirManager::AdjustAssetsPath(stdnamee);
    if (!assetTable[stdnamee]){
        Mix_Music * musick = Mix_LoadMUS(stdnamee.c_str());
        if (musick){
            assetTable[stdnamee] = musick;
            return true;
        }else{
            bear::out << "[Music::Open] Cannot play "<<stdnamee<<" because : "<< Mix_GetError() <<"\n";
        }
    }
    return false;
}
void Music::Play(int times){
    if (!working)
        return;
    if (IsOpen())
        Mix_PlayMusic(music, times);
        Mix_VolumeMusic(MasterVolume);
}

void Music::Stop(){
    if (!BearEngine->AudioWorking())
        return;
    Mix_FadeOutMusic(0);
}
bool Music::IsPlaying(){
    if (!BearEngine->AudioWorking())
        return false;
    if (IsOpen()){
        return Mix_PlayingMusic();
    }
    return false;
}

void Music::Pause(){
    if (!BearEngine->AudioWorking() && IsPlaying())
        return;
    Mix_PauseMusic();
}
void Music::Resume(){
    if (!BearEngine->AudioWorking() && !IsPlaying())
        return;
    Mix_ResumeMusic();
}
Music::~Music(){
    if (!working)
        return;
    Mix_FadeOutMusic(0);
}
void Music::Clear(){
    assetTable.clear();
    //Fix
    //Mix_FreeMusic(Mix_Music* music)

}





