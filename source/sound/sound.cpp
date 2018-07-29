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
float Sound::GetMasterVolume(int classType){
    return MasterVolume[classType]*MAX_VOL_SIZE;
}

bool Sound::SetMasterVolume(uint8_t vol,int classType_){
    float newValue = ((float)vol)/MAX_VOL_SIZE;
    if (classType_ == -1){
        for (int i=0;i<MAX_VOL_TYPES;i++){
            SetMasterVolume(vol,i);
        }
        return true;
    }
    for (int i=0;i<SoundPool::GetInstance().count;i++){
        if (SoundPool::GetInstance().classes[i] == classType_){
            ALuint thiSource = SoundPool::GetInstance().sources[i];
            alSourcef(thiSource, AL_MAX_GAIN ,newValue);
        }
    }
    MasterVolume[classType_] = newValue;
    return true;
}

Sound::~Sound(){

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

bool Sound::Open(std::string str){
    std::string stdnamee(str);
    std::string aux = stdnamee;
    if (ResourceManager::IsValidResource(aux)){
        snd = GlobalAssetManager::GetInstance().makeSound(false,str);
    }else{
        snd = GlobalAssetManager::GetInstance().makeSound(false,str);
    }
    file = str;
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

    std::string aux = stdnamee;
    if (ResourceManager::IsValidResource(aux)){
        SDL_RWops* rw = ResourceManager::GetInstance().GetFile(stdnamee); //safe
        if (!rw){
            bear::out << "Cannot load " << stdnamee << ". file not found\n";
            return nullptr;
        }
        BufferData* retData = Preload(rw,stdnamee);
        SDL_RWclose(rw);
        return retData;
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


int Sound::PlayOnce(const char *s,bool global,int volume,Point3 pos,int classN){
    if (!ConfigManager::GetInstance().IsWorkingAudio())
        return -1;
    std::string stdnamee(s);
    SoundPtr snd;
    std::string aux = stdnamee;
    if (ResourceManager::IsValidResource(aux)){
        if (global)
            snd = GlobalAssetManager::GetInstance().makeSound(false,stdnamee);
        else
            snd = Game::GetCurrentState().Assets.makeSound(false,stdnamee);
    }else{
        if (global)
            snd = GlobalAssetManager::GetInstance().makeSound(false,s);
        else
            snd = Game::GetCurrentState().Assets.makeSound(false,s);
    }

    if (snd.get()){
        ALuint sourceID_ = SoundPool::GetInstance().GetSource(classN);
        if (sourceID_ != 0){
            volume = std::max(volume,0);
            volume = std::min(MAX_VOL_SIZE,(float)volume);
            SoundLoader::ShowError();
            alSourcei(sourceID_, AL_BUFFER, snd.get()->buffer);
            SoundLoader::ShowError();
            alSource3f(sourceID_, AL_POSITION, pos.x, pos.y, pos.z);
            SoundLoader::ShowError();
            alSourcef(sourceID_, AL_GAIN, ((float)volume/MAX_VOL_SIZE) );
            SoundLoader::ShowError();
            alSourcef(sourceID_, AL_MAX_GAIN, MasterVolume[classN] );
            SoundLoader::ShowError();
            alSourcef(sourceID_, AL_PITCH, 1.0);
            SoundLoader::ShowError();
            alSourcei(sourceID_, AL_LOOPING, false);
            SoundLoader::ShowError();
            alSourcePlay(sourceID_);
            SoundLoader::ShowError();
            return snd.get()->buffer;
        }else{
            bear::out << "No source\n";
        }
    }else{
        bear::out << "Cannot play " << s << " because no source\n";
    }
    return -1;
}
int Sound::PlayOncePiched(const char *s,int ptch,bool global,int volume,Point3 pos,int classN){
    if (!ConfigManager::GetInstance().IsWorkingAudio())
        return -1;
    std::string stdnamee(s);
    SoundPtr snd;
    std::string aux = stdnamee;
    if (ResourceManager::IsValidResource(aux)){
        if (global)
            snd = GlobalAssetManager::GetInstance().makeSound(false,stdnamee);
        else
            snd = Game::GetCurrentState().Assets.makeSound(false,stdnamee);
    }else{
        if (global)
            snd = GlobalAssetManager::GetInstance().makeSound(false,s);
        else
            snd = Game::GetCurrentState().Assets.makeSound(false,s);
    }

    if (snd.get()){
        ALuint sourceID_ = SoundPool::GetInstance().GetSource(classN);
        if (sourceID_ != 0){
            volume = std::max(volume,0);
            volume = std::min(MAX_VOL_SIZE,(float)volume);
            SoundLoader::ShowError();
            alSourcei(sourceID_, AL_BUFFER, snd.get()->buffer);
            SoundLoader::ShowError();
            alSource3f(sourceID_, AL_POSITION, pos.x, pos.y, pos.z);
            SoundLoader::ShowError();
            alSourcef(sourceID_, AL_GAIN, ((float)volume/MAX_VOL_SIZE) );
            SoundLoader::ShowError();
            alSourcef(sourceID_, AL_MAX_GAIN, MasterVolume[classN] );
            SoundLoader::ShowError();
            alSourcef(sourceID_, AL_PITCH, 1.0);
            SoundLoader::ShowError();
            alSourcei(sourceID_, AL_LOOPING, false);
            SoundLoader::ShowError();
            float addPitch = ( (ptch*100 - rand()%(ptch*200))/10000.0f );
            alSourcef(sourceID_, AL_PITCH, 1.0f + addPitch );
            alSourcePlay(sourceID_);
            SoundLoader::ShowError();
            return snd.get()->buffer;
        }else{
            bear::out << "No source\n";
        }
    }else{
        bear::out << "Cannot play " << s << " because no source\n";
    }
    return -1;
}

void Sound::Kill(){
    Stop();
    snd.destroy();
}



void Sound::SetVolume(int vol){
    vol = std::max(vol,-1);
    vol = std::min(MAX_VOL_SIZE,(float)vol);
    if (!working || !IsOpen() || !checkSource()){
        if (vol != -1){
            volume = vol;
        }
        return;
    }
    if (vol != -1){
        volume = vol;
    }else{
        vol = volume;
    }
    alSourcef(sourceID, AL_MAX_GAIN, MasterVolume[classType] );
    alSourcef(sourceID, AL_GAIN, ((float)vol/MAX_VOL_SIZE) );
    SoundLoader::ShowError("on volume");
}

void Sound::SetPosition(float pos){
     if (!working)
        return;
    if (!IsOpen())
        return;
    if (!checkSource())
        return;
    alSourcef(sourceID,AL_SEC_OFFSET,pos);
    SoundLoader::ShowError("on offset");
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
    SoundLoader::ShowError();
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
    SoundLoader::ShowError("on position");
}
void Sound::SetRepeat(bool repeat){
    if (!working)
        return;
    if (!checkSource())
        return;
    alSourcei(sourceID, AL_LOOPING, repeat);
    SoundLoader::ShowError("on loop");
}

void Sound::PrePlay(){
    if (!snd.get()){
        return;
    }
    bufferId = snd.get()->buffer;
    ALint lastBuffer;
    alGetBufferi(sourceID, AL_BUFFER, &lastBuffer);
    if (lastBuffer != bufferId)
        alSourcei(sourceID, AL_BUFFER, bufferId);
    SetPitch(pitch);
    SetPosition(pos);
    SetVolume(-1);
}

bool Sound::Play(bool repeat){
    if (!working)
        return false;
    if (snd.get() && snd.get()->buffer){
        if (sourceID == 0 || !checkSource())
            sourceID = SoundPool::GetInstance().GetSource(classType);
        if (sourceID == 0){
            bear::out << "Cant find any source\n";
            return false;
        }
        PrePlay();
        SetRepeat(repeat);
        alSourcePlay(sourceID);
        SoundLoader::ShowError("on play");
        return true;
    }else{
        bear::out << "No buffer loaded\n";
    }
    return false;
}

void Sound::Resume(){
    if (!working || !checkSource())
        return;
    if (!IsPlaying()){
        alSourcePlay(sourceID);
        SoundLoader::ShowError();
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


bool Sound::FadeOut(float speed,float minVol){
    if (!IsPlaying()){
        return false;
    }
    if (speed > 0){
        speed = - speed;
    }
    minVol = ((float)minVol/MAX_VOL_SIZE);
    float curSound =((float)volume/MAX_VOL_SIZE);
    SoundWorker::FaderList.emplace_back( SoundFaderInstance(curSound,sourceID,snd,minVol,speed,classType));
    return true;
}

bool Sound::FadeIn(float speed,float maxVol,bool looped){
    if (IsPlaying()){
        return false;
    }
    SetVolume(0);
    Play(looped);
    maxVol = ((float)maxVol/MAX_VOL_SIZE);
    SoundWorker::FaderList.emplace_back( SoundFaderInstance(0.0f,sourceID,snd,maxVol,speed,classType));
    return true;
}


bool Sound::checkSource(){
    ALint buffid;
    if (sourceID == 0){
        return false;
    }
    if (!snd.get() || !snd.get()->buffer){
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
    alSourcei(sourceID, AL_BUFFER, 0);
}
void Sound::Pause(){
    if (!working || !checkSource())
        return;
    if (!IsPlaying())
        return;
    alSourcePause(sourceID);
    SoundLoader::ShowError("on pause");
}


bool Sound::IsOpen(){
    return snd.get()->buffer != 0;
}

