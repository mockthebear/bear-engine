#pragma once

#include <AL/al.h>
#include <AL/alc.h>
//#include <vorbis/vorbisfile.h>
#include "sound.hpp"


class SoundPool{
    public:
        static SoundPool& GetInstance(bool autoQuantity=true);
        ~SoundPool();
        ALuint GetSource(int soundClass=0);
        void UpdateClassType(ALuint src,int classType);
    private:
        friend class Sound;
        SoundPool(bool autoQuantity);
        ALuint *sources;
        int *classes;
        int count;
};
