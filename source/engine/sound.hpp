#include "../settings/definitions.hpp"
#include SDL_LIB_HEADER
#include SDL_MIXER_LIB_HEADER

#ifndef SOUNDH_
#define SOUNDH_
#include "../settings/configmanager.hpp"
#include "../framework/chainptr.hpp"
#include <unordered_map>
#include <string>
#include <memory>
/**
    @brief Basic sound class [Not 100% ready] TODO: Port to openAl
*/

typedef chain_ptr<Mix_Chunk> SoundPtr;



class Sound{
    public:
        Sound();
        Sound(char *s);
        Sound(const char *s);
        Sound(SoundPtr snd,const char *s);
        bool Open(const char *str);
        void SetVolume(int vol);
        void Play (int times);
        void Stop();
        void Pause();
        void Resume();
        static void StopAll();
        static void PauseAll();
        static void ResumeAll();


        void FadeOut(float ms);
        void SetPosition(int pos);

        bool IsOpen();
        bool IsPlaying();
        static int PlayOnce(const char *s,bool useGlobalAssetManager=false,int volume=-128);


        static Mix_Chunk* Preload (std::string name);
        static Mix_Chunk* Preload(SDL_RWops* file,std::string name);

        std::string GetFileName(){return file;};
        void SetAssetManager(int id){};
        static bool SetMasterVolume(uint8_t vol);
        static uint8_t GetMasterVolume(){ return MasterVolume;};


    private:
        static uint8_t MasterVolume;

        bool static working;
        std::string file;
        SoundPtr music;
        int channel;


};

#endif


