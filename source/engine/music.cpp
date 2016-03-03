#include "music.hpp"
#include "gamebase.hpp"
#include "../framework/dirmanager.hpp"


std::unordered_map<std::string, Mix_Music*> Music::assetTable;


Music::Music(){
    music = NULL;
    working = BearEngine->AudioWorking();
}

Music::Music(char *s){
    music = NULL;
    working = BearEngine->AudioWorking();
    Open((const char*)s);
}

Music::Music(const char *s){
    music = NULL;
    working = BearEngine->AudioWorking();
    Open(s);
}

void Music::SetVolume(int vol){
    if (BearEngine->AudioWorking())
        Mix_VolumeMusic(vol);
}

void Music::Open(const char *s){
    std::string stdnamee(s);
    file = stdnamee;
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
            std::cout << "[Music::Open] Cannot play "<<stdnamee<<" because : "<< Mix_GetError() <<"\n";
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
            std::cout << "[Music::Open] Cannot play "<<stdnamee<<" because : "<< Mix_GetError() <<"\n";
        }
    }
    return false;
}
void Music::Play(int times){
    if (!working)
        return;
    if (IsOpen())
        Mix_PlayMusic(music, times);
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





