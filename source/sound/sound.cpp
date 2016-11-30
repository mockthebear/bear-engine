#include "sound.hpp"
#include "../settings/definitions.hpp"
#include "../framework/resourcemanager.hpp"
#include "../framework/dirmanager.hpp"
#include "../framework/gamefile.hpp"
#include "../performance/console.hpp"
#include "../sound/soundsources.hpp"
#include "../engine/assetmanager.hpp"
#include "../engine/gamebase.hpp"
#include "soundloader.hpp"
#include <iostream>

#include <cstdio>

#include <vector>
using namespace std;


bool Sound::working = false;


bool Sound::SetMasterVolume(uint8_t vol,int classType){
    float newValue = ((float)vol)/127.0f;
    if (classType == -1){
        for (int i=0;i<MAX_VOL_TYPES;i++){
            SetMasterVolume(vol,i);
        }
        return true;
    }
    float oldValue = MasterVolume[classType];
    for (int i=0;i<SoundPool::GetInstance().count;i++){
        //alGetSourcei(, AL_SOURCE_STATE, &state);
        if (SoundPool::GetInstance().classes[i] == classType){
            ALuint thiSource = SoundPool::GetInstance().sources[i];
            float currVol=1.0f;
            alGetSourcef(thiSource, AL_GAIN, &currVol);
            currVol = currVol/oldValue;
            alSourcef(thiSource, AL_GAIN, newValue*currVol);
        }
    }
    MasterVolume[classType] = newValue;
    return true;
}

Sound::Sound(){
    working = ConfigManager::GetInstance().IsWorkingAudio();
    volume = 127;
    classType = 0;
    pitch = 1.0;

}

Sound::Sound(SoundPtr snda,const char *s):Sound(){
    snd = snda;
    file = s;
    sourceID = 0;
    working = ConfigManager::GetInstance().IsWorkingAudio();
}

Sound::Sound(char *s,int classType):Sound(){
    working = ConfigManager::GetInstance().IsWorkingAudio();
    Open((const char *)s);
    file = s;
    this->classType = classType;
}


Sound::Sound(const char *s,int classType):Sound(){
    working = ConfigManager::GetInstance().IsWorkingAudio();
    Open(s);
    file = s;
    this->classType = classType;
}

bool Sound::Open(const char *str,bool isWave){
    std::string stdnamee(str);
    if (stdnamee.find(":")!=std::string::npos){
        snd = GlobalAssetManager::GetInstance().makeSound(false,ResourceManager::GetInstance().GetFile(stdnamee),str);
    }else{
        snd = GlobalAssetManager::GetInstance().makeSound(false,str);
    }
    return true;
}

BufferData* Sound::Preload(SDL_RWops* file,std::string name){
    if (!ConfigManager::GetInstance().IsWorkingAudio())
        return NULL;
    if (!file){
        return nullptr;
    }
    char theName[4];
    SDL_RWread(file,&theName[0], 1, 1);
    SDL_RWread(file,&theName[1], 1, 1);
    SDL_RWread(file,&theName[2], 1, 1);
    theName[3] = 0;
    SDL_RWseek(file, 0, RW_SEEK_SET);
    if (theName[0] == 'O' && theName[1] == 'g' && theName[2] == 'g'){
        return SoundLoader::loadOggFileRW(file);

    }else{
        return SoundLoader::loadWavFileRW(file);
    }
    return nullptr;
}
BufferData* Sound::Preload(std::string stdnamee){
    if (!ConfigManager::GetInstance().IsWorkingAudio())
        return NULL;
    if (stdnamee.find(":")!=std::string::npos){
        return Preload(ResourceManager::GetInstance().GetFile(stdnamee),stdnamee);
    }
    GameFile f(stdnamee);
    if (f.IsOpen()){
        char theName[4];
        theName[0] = f.ReadByte();
        theName[1] = f.ReadByte();
        theName[2] = f.ReadByte();
        theName[3] = 0;
        if (theName[0] == 'O' && theName[1] == 'g' && theName[2] == 'g'){
            f.Close();
            return SoundLoader::loadOggFile(stdnamee.c_str());
        }else{
            return SoundLoader::loadWavFile(stdnamee.c_str());
        }
    }else{
        return nullptr;
    }
}


int Sound::PlayOnce(const char *s,bool global,int volume,int classN){
    if (!ConfigManager::GetInstance().IsWorkingAudio())
        return -1;
    std::string stdnamee(s);
    SoundPtr snd;
    if (stdnamee.find(":")!=std::string::npos){
        if (global)
            snd = GlobalAssetManager::GetInstance().makeSound(false,ResourceManager::GetInstance().GetFile(stdnamee),stdnamee);
        else
            snd = Game::GetCurrentState().Assets.makeSound(false,ResourceManager::GetInstance().GetFile(stdnamee),stdnamee);
    }else{
        if (global)
            snd = GlobalAssetManager::GetInstance().makeSound(false,s);
        else
            snd = Game::GetCurrentState().Assets.makeSound(false,s);
    }

    if (snd.get()){
        ALuint sourceID_ = SoundPool::GetInstance().GetSource(classN);
        if (sourceID_ != 0){
            alSourcei(sourceID_, AL_BUFFER, snd.get()->buffer);
            alSourcePlay(sourceID_);
            return snd.get()->buffer;
        }
    }else{
        bear::out << "Cannot play " << s << " because " << SDL_GetError() << "\n";
    }
    return -1;
}


void Sound::SetVolume(int vol){
     if (!working)
        return;
    if (!IsOpen())
        return;
    if (!checkSource())
        return;
    if (vol != -1){
        volume = vol;
    }else{
        vol = volume;
    }
    alSourcef(sourceID, AL_GAIN, MasterVolume[classType]*((float)vol/127.0f) );
}

void Sound::SetPosition(float pos){
     if (!working)
        return;
    if (!IsOpen())
        return;
    if (!checkSource())
        return;
    alSourcef(sourceID,AL_SEC_OFFSET,pos);
}

float Sound::GetPosition(){
     if (!working)
        return 0;
    if (!IsOpen())
        return 0;
    if (!checkSource())
        return 0;
    float pos=0;
    alGetSourcef(sourceID,AL_SEC_OFFSET,&pos);
    return pos;
}
void Sound::SetClassType(int n){
    classType=n;
    if (!working)
        return;
    if (!IsOpen())
        return;
    if (!checkSource())
        return;
    SoundPool::GetInstance().UpdateClassType(sourceID,n);
};
float Sound::GetDuration(){
    if (!working)
        return -1;
    if (!IsOpen())
        return -1;
    ALint frequency;
    ALint sizeInBytes;
    ALint channels;
    ALint bits;
    alGetBufferi(snd.get()->buffer, AL_SIZE, &sizeInBytes);
    alGetBufferi(snd.get()->buffer, AL_CHANNELS, &channels);
    alGetBufferi(snd.get()->buffer, AL_BITS, &bits);
    alGetBufferi(snd.get()->buffer, AL_FREQUENCY, &frequency);
    ALint lengthInSamples = sizeInBytes * 8 / (channels * bits);
    float durationInSeconds = (float)lengthInSamples / (float)frequency;
    return durationInSeconds;
}
void Sound::SetPosition(Point3 p){
    pos = p;
    if (!working)
        return;
    if (!checkSource())
        return;
    alSource3f(sourceID, AL_POSITION, pos.x, pos.y, pos.z);
}
void Sound::SetRepeat(bool repeat){
    if (!working)
        return;
    if (!checkSource())
        return;
    alSourcei(sourceID, AL_LOOPING, repeat);
}
bool Sound::Play(bool repeat){
    if (!working)
        return false;
    if (snd.get()->buffer){
        sourceID = SoundPool::GetInstance().GetSource(classType);
        alSourcei(sourceID, AL_BUFFER, snd.get()->buffer);

        SetPitch(pitch);
        SetRepeat(repeat);
        SetPosition(pos);
        SetVolume(-1);
        alSourcePlay(sourceID);
        return true;
    }
    return false;
}

void Sound::Resume(){
    if (!working || !checkSource())
        return;
    if (!IsPlaying()){
        alSourcePlay(sourceID);
    }
}
void Sound::SetPitch(float f){
    pitch = f;
    if (!working|| !checkSource())
        return;
    alSourcef(sourceID, AL_PITCH, pitch);
}
void Sound::Toggle(){
    if (IsPlaying()){
        Pause();
    }else{
        Resume();
    }
}
bool Sound::checkSource(){
    ALint buffid;
    if (sourceID == 0){
        return false;
    }
    alGetSourcei(sourceID, AL_BUFFER, &buffid);
    if ((ALuint)buffid != snd.get()->buffer){
        return false;
    }
    return true;
}
bool Sound::IsPlaying(){
    if (!working || !checkSource())
        return false;
    ALenum state;
    alGetSourcei(sourceID, AL_SOURCE_STATE, &state);
    return state == AL_PLAYING;
}

bool Sound::IsPaused(){
    if (!working || !checkSource())
        return false;
    ALenum state;
    alGetSourcei(sourceID, AL_SOURCE_STATE, &state);
    return state == AL_PAUSED;
}
void Sound::Stop(){
    if (!working || !checkSource())
        return;
    alSourceStop(sourceID);
        //Mix_HaltChannel(channel);
}
void Sound::Pause(){
    if (!working || !checkSource())
        return;
    if (!IsPlaying())
        return;
    alSourcePause(sourceID);
}


bool Sound::IsOpen(){
    return snd.get()->buffer != 0;
}

