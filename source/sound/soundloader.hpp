#pragma once
#include "sounddef.hpp"
#include SDL_LIB_HEADER
#define BUFFER_SIZE     32768       // 32 KB buffers
#include <vorbis/vorbisfile.h>
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


long AR_tellOgg( void *fh );
int AR_closeOgg(void* fh);
int AR_seekOgg( void *fh, ogg_int64_t to, int type );
size_t AR_readOgg(void* dst, size_t size1, size_t size2, void* fh);
