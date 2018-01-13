#pragma once
#include "../settings/configmanager.hpp"
#include "../framework/chainptr.hpp"
#include <AL/al.h>
#include <AL/alc.h>
#include <vector>
#define MAX_VOL_TYPES 16
#define MAX_VOL_SIZE 128.0f

class BufferData{
    public:
    BufferData():buffer(0),format(0),freq(0){};
    ALuint buffer;
    ALenum format;
    ALsizei freq;
};

typedef chain_ptr<BufferData> SoundPtr;

struct WAVE_Data {
  char subChunkID[4];
  uint32_t subChunk2Size;
};


struct WAVE_Format {
  char subChunkID[4];
  uint64_t subChunkSize;
  short audioFormat;
  short numChannels;
  uint32_t sampleRate;
  uint32_t byteRate;
  short blockAlign;
  short bitsPerSample;
};

struct RIFF_Header {
  char chunkID[4];
  uint32_t chunkSize;
  char format[4];
};


class SoundFaderInstance{
    public:
        SoundFaderInstance():volume(0.0f),max(0.0f),sourceID(0),snd(),increase(0.0f),dead(true){};
        SoundFaderInstance(float baseVol,ALuint source,SoundPtr p,float maxm,float increasee):volume(baseVol),max(maxm),sourceID(source),snd(p),increase(increasee),dead(false){};
        float volume;
        float max;
        ALuint sourceID;
        SoundPtr snd;
        float increase;
        bool dead;
};

class SoundWorker{
    public:
        SoundWorker();
        ~SoundWorker();
        static void Clear();
        static void Update(float dt);

        static  std::vector<SoundFaderInstance> FaderList;
};
