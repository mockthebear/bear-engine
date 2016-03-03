#ifndef MUSICH
#define MUSICH
#include "../settings/definitions.hpp"
#include SDL_LIB_HEADER
#include SDL_MIXER_LIB_HEADER

#include <unordered_map>
#include <string>
/**
    @brief Basic music class [Not 100% ready] TODO: Port to openAl may be removed
*/
class Music{
    public:
        Music();
        ~Music();
        Music(char *s);
        Music(const char *s);
        void Play(int times);
        void Pause();
        void Resume();
        static void Stop();
        void Open(const char *s);
        static bool Rewind();
        static void FadeOut(int ms);
        void FadeIn(int loop,int ms);
        static bool Preload(const char *s);
        bool IsOpen(){return music != NULL;};
        bool IsPlaying();
        void SetVolume(int vol);
        static void Clear();
        std::string GetFileName(){return file;};
    private:
        bool isPlaying;
        bool working;
        std::string file;
        Mix_Music *music;
        static std::unordered_map<std::string, Mix_Music*> assetTable;
};

#endif
