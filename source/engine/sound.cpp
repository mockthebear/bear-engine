#include "sound.hpp"
#include "../settings/definitions.hpp"
#include "../framework/resourcemanager.hpp"
#include "../framework/dirmanager.hpp"
#include "../performance/console.hpp"
#include "assetmanager.hpp"
#include <stdio.h>
#include <iostream>

bool Sound::working = false;

Sound::Sound(){
    music = NULL;
    channel = -1;
    working = ConfigManager::GetInstance().IsWorkingAudio();
}

Sound::Sound(char *s):Sound(){
    music = NULL;
    channel = -1;
    working = ConfigManager::GetInstance().IsWorkingAudio();
    Open((const char *)s);
    file = s;
}



Sound::Sound(const char *s):Sound(){
    music = NULL;
    channel = -1;
    working = ConfigManager::GetInstance().IsWorkingAudio();
    Open(s);
    file = s;
}

bool Sound::Open(const char *str){
    std::string stdnamee(str);
    if (stdnamee.find(":")!=std::string::npos){
        music = GlobalAssetManager::GetInstance().makeSound(ResourceManager::GetInstance().GetFile(stdnamee),str);
    }else{
        music = GlobalAssetManager::GetInstance().makeSound(str);
    }
    if (music and music.get() and *music.get()){
        return true;
    }
    return false;
}

Mix_Chunk* Sound::Preload(SDL_RWops* file,std::string name){
    if (!ConfigManager::GetInstance().IsWorkingAudio())
        return NULL;
    Mix_Chunk* aux_chunk = Mix_LoadWAV_RW(file, 1);
    if (aux_chunk){
        return aux_chunk;
    }else{
        Console::GetInstance().AddText(utils::format("Cannot load rwop sound [%s]",name));
        return NULL;
    }
}
Mix_Chunk* Sound::Preload(std::string stdnamee){
    if (!ConfigManager::GetInstance().IsWorkingAudio())
        return NULL;
    if (stdnamee.find(":")!=std::string::npos){
        return Preload(ResourceManager::GetInstance().GetFile(stdnamee),stdnamee);
    }
    Mix_Chunk* r_music=NULL;
    stdnamee = DirManager::AdjustAssetsPath(stdnamee);

    r_music = Mix_LoadWAV(stdnamee.c_str());
    if (r_music){
          return r_music;
    }else{
        Console::GetInstance().AddText(utils::format("Cannot preload sound [%s]",stdnamee));
    }
    return r_music;
}

int Sound::PlayOnce(const char *s){
    if (!ConfigManager::GetInstance().IsWorkingAudio())
        return -1;
    std::string stdnamee(s);
    SoundPtr snd;
    if (stdnamee.find(":")!=std::string::npos){
        snd = GlobalAssetManager::GetInstance().makeSound(ResourceManager::GetInstance().GetFile(stdnamee),stdnamee);
    }else{
        snd = GlobalAssetManager::GetInstance().makeSound(s);
    }
    if (snd and snd.get() and *snd.get()){
        return Mix_PlayChannel(-1,*snd.get(), 0);
    }
    return -1;
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
    channel = Mix_PlayChannel(-1,(*music.get()), times);
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



bool Sound::IsOpen(){
    return music and music.get() and (*music.get());
}

