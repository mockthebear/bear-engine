#include "../settings/definitions.hpp"
#include SDL_MIXER_LIB_HEADER

#ifndef SOUNDH_
#define SOUNDH_
#include "../settings/configmanager.hpp"
#include <unordered_map>
#include <string>
/**
    @brief Basic sound class [Not 100% ready] TODO: Port to openAl
*/
class Sound{
    public:
        Sound();
        Sound(char *s);
        Sound(const char *s);
        void SetVolume(int vol);
        void Play (int times);
        void Stop();
        static void StopAll();
        void FadeOut(float ms);
        void SetPosition(int pos);
        void Open (const char *s);
        bool IsOpen();
        bool IsPlaying();
        static void Clear();
        static bool PlayOnce(const char *s);
        static bool Preload(const char *s);
        std::string GetFileName(){return file;};
    private:
        bool working;
        std::string file;
        Mix_Chunk* music;
        int channel;
        static std::unordered_map<std::string, Mix_Chunk*> assetTable;

};

#endif


