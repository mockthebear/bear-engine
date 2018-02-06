#pragma once
#include "sounddef.hpp"
#include SDL_LIB_HEADER
#define BUFFER_SIZE     32768       // 32 KB buffers
class SoundLoader{
    public:
        static BufferData* loadWavFile(const char *c);
        static BufferData* loadOggFile(const char *c);
        static BufferData* loadWavFileRW(SDL_RWops* soundFile);
        static BufferData* loadOggFileRW(SDL_RWops* soundFile);

        static void ShowError(std::string where = "");
    private:
        static char array[BUFFER_SIZE];
};
