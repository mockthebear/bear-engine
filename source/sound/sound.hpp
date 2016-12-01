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
#include <AL/al.h>
#include <AL/alc.h>
#define MAX_VOL_TYPES 16

/**
    @brief Basic sound class
*/
static float MasterVolume[MAX_VOL_TYPES] = {1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,
                                 1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f};

class Sound{
    public:
        Sound();
        Sound(char *s,int classType=0);
        Sound(const char *s,int classType=0);
        Sound(SoundPtr snd,const char *s);
        bool Open(const char *str,bool isWave=false);
        void SetVolume(int vol);
        bool Play (bool repeat=false);
        void SetRepeat(bool repeat);
        void Stop();
        void Pause();
        void Resume();
        void Toggle();

        void SetPosition(float pos);
        float GetPosition();
        float GetDuration();

        bool IsOpen();
        bool IsPlaying();
        bool IsPaused();
        static int PlayOnce(const char *s,bool useGlobalAssetManager=false,int volume=128,Point3 pos=Point3(),int classN=0);

        static BufferData* Preload (std::string name);
        static BufferData* Preload(SDL_RWops* file,std::string name);

        std::string GetFileName(){return file;};
        void SetAssetManager(int id){};
        static bool SetMasterVolume(uint8_t vol,int classType=0);
        static float GetMasterVolume(int classType=0){ return MasterVolume[classType]*127.0;};
        void SetClassType(int n);
        int GetClassType(){return classType;};
        void SetPosition(Point3 p);
        void SetPosition(float x,float y,float z){
            SetPosition(Point3(x,y,z));
        }
        void SetPitch(float f);
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
        SoundPtr snd;

};

#endif


