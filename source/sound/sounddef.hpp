#pragma once
#include "../settings/configmanager.hpp"
#include "../framework/chainptr.hpp"
#include <AL/al.h>
#include <AL/alc.h>
class BufferData{
    public:
    BufferData(){
        buffer = 0;
        freq = 0;
    };
    ALuint buffer;
    ALenum format;
    ALsizei freq;
};

typedef chain_ptr<BufferData> SoundPtr;

struct WAVE_Data {
  char subChunkID[4];
  long subChunk2Size;
};


struct WAVE_Format {
  char subChunkID[4];
  uint64_t subChunkSize;
  short audioFormat;
  short numChannels;
  long sampleRate;
  long byteRate;
  short blockAlign;
  short bitsPerSample;
};

struct RIFF_Header {
  char chunkID[4];
  long chunkSize;
  char format[4];
};

