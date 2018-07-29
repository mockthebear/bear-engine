#ifndef SOUNDH_
#define SOUNDH_
#include "../settings/definitions.hpp"
#include SDL_LIB_HEADER
#include "../settings/configmanager.hpp"
#include "../framework/chainptr.hpp"
#include "sounddef.hpp"
#include <unordered_map>
#include <string>
#include <memory>
#include <iostream>
#include <AL/al.h>
#include <AL/alc.h>


/**
    @brief Basic sound class
*/

class Sound{
    public:
        Sound():classType(0),volume(MAX_VOL_SIZE),pitch(1.0f),pos(0.0f,0.0f,0.0f),file(""),sourceID(0),bufferId(0){
            working = ConfigManager::GetInstance().IsWorkingAudio();
        };
        ~Sound();
        Sound(char *s,int classType=0);
        Sound(const char *s,int classType=0);
        Sound(SoundPtr snd,const char *s);
        Sound(SoundPtr snd):Sound(){};
        bool Open(std::string str);
        void SetVolume(int vol);
        bool Play (bool repeat=false);
        void SetRepeat(bool repeat);
        void Stop();
        void Pause();
        void Resume();
        void Toggle();

        void PrePlay();

        bool FadeOut(float speed=1.0f,float minVol = 0.0f);
        bool FadeIn(float speed=1.0f,float maxVol = MAX_VOL_SIZE,bool repeat=false);

        void SetPosition(float pos);
        float GetPosition();
        float GetDuration();

        bool IsOpen();
        bool IsPlaying();
        bool IsPaused();

        std::string GetFileName(){ return file;};


        void SetClassType(int n);
        int GetClassType(){return classType;};
        void SetPosition(Point3 p);
        void SetPosition(float x,float y,float z){
            SetPosition(Point3(x,y,z));
        }
        void SetPitch(float f);


        static int PlayOnce(const char *s,bool useGlobalAssetManager=false,int volume=128,Point3 pos=Point3(),int classN=0);
        static int PlayOncePiched(const char *s,int pch=1,bool useGlobalAssetManager=false,int volume=128,Point3 pos=Point3(),int classN=0);

        static BufferData* Preload (std::string name);
        static BufferData* Preload(SDL_RWops* file,std::string name);


        static bool SetMasterVolume(uint8_t vol,int classType=0);
        static float GetMasterVolume(int classType=0);

        void Kill();

    protected:
        int classType;
        bool static working;
        int volume;
    private:
        float pitch;
        Point3 pos;
        bool checkSource();

        std::string file;
        ALuint sourceID;
        ALint bufferId;
        SoundPtr snd;

};

#endif


