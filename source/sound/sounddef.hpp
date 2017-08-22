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

