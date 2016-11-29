#include "../settings/definitions.hpp"
#include SDL_LIB_HEADER
#include SDL_MIXER_LIB_HEADER

#include <AL/al.h>
#include <AL/alc.h>
#include <vorbis/vorbisfile.h>

#ifndef SOUNDH_
#define SOUNDH_
#include "../settings/configmanager.hpp"
#include "../framework/chainptr.hpp"
#include <unordered_map>
#include <string>
#include <memory>
/**
    @brief Basic sound class [Not 100% ready] TODO: Port to openAl
*/
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

/*
 * Load wave file function. No need for ALUT with this
 */

 /*
* Struct to hold the data of the wave file
*/
struct WAVE_Data {
  char subChunkID[4]; //should contain the word data
  long subChunk2Size; //Stores the size of the data block
};

/*
 * Struct to hold fmt subchunk data for WAVE files.
 */
struct WAVE_Format {
  char subChunkID[4];
  long subChunkSize;
  short audioFormat;
  short numChannels;
  long sampleRate;
  long byteRate;
  short blockAlign;
  short bitsPerSample;
};

struct RIFF_Header {
  char chunkID[4];
  long chunkSize;//size not including chunkSize or chunkID
  char format[4];
};


class Sound{
    public:
        Sound();
        Sound(char *s,bool isWave=false);
        Sound(const char *s,bool isWave=false);


        Sound(SoundPtr snd,const char *s);
        bool Open(const char *str,bool isWave=false);
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
        static int PlayOnce(const char *s,bool useGlobalAssetManager=false,int volume=-128);


        static BufferData* Preload (std::string name);
        static BufferData* Preload(SDL_RWops* file,std::string name);

        std::string GetFileName(){return file;};
        void SetAssetManager(int id){};
        static bool SetMasterVolume(uint8_t vol);
        static uint8_t GetMasterVolume(){ return MasterVolume;};


    private:

        static BufferData* loadWavFile(const char *c);
        static BufferData* loadOggFile(const char *c);
        static BufferData* loadWavFileRW(SDL_RWops* soundFile);


        static uint8_t MasterVolume;

        bool static working;
        std::string file;

        SoundPtr snd;

};

#endif


