#include "soundsources.hpp"
#include "soundloader.hpp"
#include "../engine/bear.hpp"

SoundPool& SoundPool::GetInstance(bool autoQuantity){
    static SoundPool inst(autoQuantity);
    return inst;
}


SoundPool::SoundPool(bool autoQuantity){
    count = 16;
    sources = new ALuint[count];
    classes = new int[count];
    alGenSources(count,sources);
    for (int i=0;i<count;i++){
        classes[i] = 0;
    }
    SoundLoader::ShowError();
    alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
    SoundLoader::ShowError();

    //ALenum err = alGetError();
}
void SoundPool::UpdateClassType(ALuint src,int classType){
    for (int i=0;i<count;i++){
        if (src == sources[i]){
            classes[i] = classType;
        }
    }
}
ALuint SoundPool::GetSource(int thisClass){
    for (int i=0;i<count;i++){
        ALenum state;
        alGetSourcei(sources[i], AL_SOURCE_STATE, &state);
        if (state != AL_PAUSED && state != AL_PLAYING){
            alSource3f(sources[i], AL_POSITION, 0.0f, 0.0f, 0.0f);
            classes[i] = thisClass;
            return sources[i];
        }
    }
    bear::out << "No sound sources left \n";
    SoundLoader::ShowError();
    return 0;
}

SoundPool::~SoundPool(){
    for (int i=0;i<count;i++){
        alDeleteSources(1, &sources[i]);
    }
    SoundLoader::ShowError();
    delete [] sources;
}
