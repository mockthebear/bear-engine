#include "sound.hpp"
#include "../settings/definitions.hpp"
#include "../framework/dirmanager.hpp"
#include <stdio.h>
#include <iostream>
std::unordered_map<std::string, Mix_Chunk*> Sound::assetTable;


Sound::Sound(){
    music = NULL;
    channel = -1;
    working = ConfigManager::GetInstance().IsWorkingAudio();
}

Sound::Sound(char *s){
    music = NULL;
    channel = -1;
    working = ConfigManager::GetInstance().IsWorkingAudio();
    Open((const char *)s);
}

Sound::Sound(const char *s){
    music = NULL;
    channel = -1;
    working = ConfigManager::GetInstance().IsWorkingAudio();
    Open(s);
}

void Sound::Open(const char *s){
    std::string stdnamee(s);
    file = stdnamee;
    stdnamee = DirManager::AdjustAssetsPath(stdnamee);
    if (!working)
        return;
    if (assetTable[stdnamee]){
        music = assetTable[stdnamee];
    }else{
        music = Mix_LoadWAV(stdnamee.c_str());
        if (music){
            assetTable[stdnamee] = music;
        }else{
            std::cout << "[Sound::PlayOnce] Cannot play "<<stdnamee<<" because : "<< Mix_GetError() <<"\n";
        }
    }
    if (IsOpen()){
        file = s;
    }
}
bool Sound::PlayOnce(const char *s){
    if (!ConfigManager::GetInstance().IsWorkingAudio())
        return false;
    Mix_Chunk* mus;
    std::string stdnamee;
    std::string ss = s;
    stdnamee = DirManager::AdjustAssetsPath(ss);
    if (assetTable[stdnamee]){
        mus = assetTable[stdnamee];
        return Mix_PlayChannel(-1,mus, 0);
    }else{
        mus = Mix_LoadWAV(stdnamee.c_str());
        if (mus==NULL){
            std::cout << "[Sound::PlayOnce] Cannot play "<<stdnamee<<" because : "<< Mix_GetError() <<"\n";
        }else{
            assetTable[stdnamee] = mus;
            return Mix_PlayChannel(-1,mus, 0);
        }
    }
    return false;

}

bool Sound::Preload(const char *s){
    if (!ConfigManager::GetInstance().IsWorkingAudio())
        return false;
    Mix_Chunk* mus;
    std::string stdnamee;
    std::string ss = s;
    stdnamee = DirManager::AdjustAssetsPath(ss);
    if (!assetTable[stdnamee]){
        mus = Mix_LoadWAV(stdnamee.c_str());
        if (mus==NULL){
            std::cout << "[Sound::Preload] Cannot play "<<stdnamee<<" because : "<< Mix_GetError() <<"\n";
        }else{
            assetTable[stdnamee] = mus;
            return true;
        }
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

void Sound::StopAll(){
    Mix_HaltChannel(-1);
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

