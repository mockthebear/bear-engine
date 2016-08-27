#include "../settings/definitions.hpp"
#include SDL_LIB_HEADER
#include SDL_MIXER_LIB_HEADER

#ifndef SOUNDH_
#define SOUNDH_
#include "../settings/configmanager.hpp"
#include <unordered_map>
#include <string>
#include <memory>
/**
    @brief Basic sound class [Not 100% ready] TODO: Port to openAl
*/

typedef std::shared_ptr<Mix_Chunk*> SoundPtr;


class Sound{
    public:
        Sound();
        Sound(char *s);
        Sound(const char *s);
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
        static int PlayOnce(const char *s);

        static Mix_Chunk* Preload (std::string name);
        static Mix_Chunk* Preload(SDL_RWops* file,std::string name);

        std::string GetFileName(){return file;};
    private:

        bool static working;
        std::string file;
        SoundPtr music;
        int channel;


};

#endif


