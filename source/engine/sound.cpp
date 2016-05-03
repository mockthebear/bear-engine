#include "sound.hpp"
#include "../settings/definitions.hpp"
#include "../framework/resourcemanager.hpp"
#include "../framework/dirmanager.hpp"
#include "../performance/console.hpp"
#include <stdio.h>
#include <iostream>
std::unordered_map<std::string, Mix_Chunk*> Sound::assetTable;
bool Sound::working = false;

Sound::Sound(){
    music = NULL;
    channel = -1;
    working = ConfigManager::GetInstance().IsWorkingAudio();
}

Sound::Sound(char *s){
    music = NULL;
    channel = -1;
    working = ConfigManager::GetInstance().IsWorkingAudio();
    music = Open((const char *)s);
    file = s;
}

Sound::Sound(const char *s){
    music = NULL;
    channel = -1;
    working = ConfigManager::GetInstance().IsWorkingAudio();
    music = Open(s);
    file = s;
}

Mix_Chunk* Sound::Open(SDL_RWops* file,std::string name){
    if (!ConfigManager::GetInstance().IsWorkingAudio())
        return NULL;
    if (assetTable.find(name) == assetTable.end() ){
        Mix_Chunk* aux_chunk = Mix_LoadWAV_RW(file, 1);
        if (aux_chunk){
            assetTable[name] = aux_chunk;
            return aux_chunk;
        }else{
            Console::GetInstance().AddText(utils::format("Cannot load rwop sound [%s]",name));
            return NULL;
        }
    }else{
        std::unordered_map<std::string, Mix_Chunk*>::iterator it = assetTable.find(name);
        Mix_Chunk* chunk = (*it).second;
        return chunk;
    }
}
Mix_Chunk* Sound::Open(const char *s){
    if (!ConfigManager::GetInstance().IsWorkingAudio())
        return NULL;
    std::string stdnamee(s);
    if (stdnamee.find(":")!=std::string::npos){
        return Open(ResourceManager::GetInstance().GetFile(stdnamee),stdnamee);
    }
    Mix_Chunk* r_music=NULL;
    stdnamee = DirManager::AdjustAssetsPath(stdnamee);
    if (assetTable[stdnamee]){
        r_music = assetTable[stdnamee];
    }else{
        r_music = Mix_LoadWAV(stdnamee.c_str());
        if (r_music){
            assetTable[stdnamee] = r_music;
        }else{
            std::cout << "[Sound::PlayOnce] Cannot play "<<stdnamee<<" because : "<< Mix_GetError() <<"\n";
        }
    }
    return r_music;

}
bool Sound::PlayOnce(const char *s){
    if (!ConfigManager::GetInstance().IsWorkingAudio())
        return false;
    Mix_Chunk* mus = Open(s);
    if (mus){
        return Mix_PlayChannel(-1,mus, 0);
    }
    return false;
}

bool Sound::Preload(const char *s){
    if (!ConfigManager::GetInstance().IsWorkingAudio())
        return false;
    Mix_Chunk* mus = Open(s);
    if (mus != NULL){
        return true;
    }
    return false;

}
void Sound::SetVolume(int vol){
     if (!working)
        return;
    if (!IsOpen())
        return;
    if (!IsPlaying())
        return;

    Mix_Volume(channel,vol);
}

void Sound::SetPosition(int vol){
     if (!working)
        return;
    if (!IsOpen())
        return;
    if (!IsPlaying())
        return;

    Mix_SetPosition(channel,0,vol);
}

void Sound::Play(int times){
    if (!working)
        return;
    channel = Mix_PlayChannel(-1,music, times);
}

void Sound::Resume(){
    if (!working)
        return;
    if (!IsPlaying())
        return;
    Mix_Resume(channel);
}
bool Sound::IsPlaying(){
    if (!working)
        return false;
    if (channel == -1)
        return false;
    return Mix_Playing(channel);
}
void Sound::Stop(){
    if (!working)
        return;
    if (!IsPlaying())
        return;
        Mix_HaltChannel(channel);
}
void Sound::Pause(){
    if (!working)
        return;
    if (!IsPlaying())
        return;
        Mix_Pause(channel);
}

void Sound::StopAll(){
    if (!working)
        return;
    Mix_HaltChannel(-1);
}
void Sound::PauseAll(){
    if (!working)
        return;
    Mix_Pause(-1);
}
void Sound::ResumeAll(){
    if (!working)
        return;
    Mix_Resume(-1);
}

void Sound::FadeOut(float ms){
    if (!working)
        return;
    if (!IsOpen())
        return;
    if (!IsPlaying())
        return;
        Mix_FadeOutChannel(channel, ms);
}

void Sound::Clear(){
    if (!ConfigManager::GetInstance().IsWorkingAudio())
        return;
    assetTable.clear();
    //Fix
    //Mix_FreeMusic(Mix_Music* music)

}

bool Sound::IsOpen(){
    return music != NULL;
}

