#include "sound.hpp"
#include "../settings/definitions.hpp"
#include "../framework/resourcemanager.hpp"
#include "../framework/dirmanager.hpp"
#include "../framework/gamefile.hpp"
#include "../performance/console.hpp"
#include "assetmanager.hpp"
#include "gamebase.hpp"
#include <iostream>

#include <cstdio>

#include <vector>
using namespace std;
#define BUFFER_SIZE     32768       // 32 KB buffers

bool Sound::working = false;
uint8_t Sound::MasterVolume = MIX_MAX_VOLUME;



bool Sound::SetMasterVolume(uint8_t vol){
    /*MasterVolume = std::min((uint8_t)MIX_MAX_VOLUME,vol);
    for (int chan=0;chan<MIX_CHANNELS;chan++){
        if (Mix_Playing(chan)){
            Mix_Volume(chan,MasterVolume);
        }
    }*/
    return true;
}

Sound::Sound(){
    working = ConfigManager::GetInstance().IsWorkingAudio();
}

Sound::Sound(SoundPtr snda,const char *s):Sound(){
    snd = snda;
    file = s;
    working = ConfigManager::GetInstance().IsWorkingAudio();
}

Sound::Sound(char *s,bool isWave):Sound(){
    working = ConfigManager::GetInstance().IsWorkingAudio();
    Open((const char *)s);
    file = s;
}


Sound::Sound(const char *s,bool isWave):Sound(){
    working = ConfigManager::GetInstance().IsWorkingAudio();
    Open(s);
    file = s;
}



BufferData *Sound::loadOggFile(const char *str){
    BufferData *ret = new BufferData();
    int endian = 0;                         // 0 for Little-Endian, 1 for Big-Endian
    int bitStream;
    long bytes;
    char array[BUFFER_SIZE];                // Local fixed size array
    FILE *f;
    std::vector<char> bufferData;

    // Open for binary reading
    f = fopen(str, "rb");

    if (f == NULL)
        {
        std::cout << "Cannot open " << str << " for reading..." << std::endl;
        delete ret;
        return nullptr;
    }
    // end if

    vorbis_info *pInfo;
    OggVorbis_File oggFile;

    // Try opening the given file
    if (ov_open(f, &oggFile, NULL, 0) != 0){
        std::cout << "Error opening " << str << " for decoding..." << std::endl;
        delete ret;
        return nullptr;
    }
    // end if

    // Get some information about the OGG file
    pInfo = ov_info(&oggFile, -1);

    // Check the number of channels... always use 16-bit samples
    if (pInfo->channels == 1)
        ret->format = AL_FORMAT_MONO16;
    else
        ret->format = AL_FORMAT_STEREO16;
    // end if

    // The frequency of the sampling rate
    ret->freq = pInfo->rate;

    // Keep reading until all is read
    do
        {
        // Read up to a buffer's worth of decoded sound data
        bytes = ov_read(&oggFile, array, BUFFER_SIZE, endian, 2, 1, &bitStream);

        if (bytes < 0)
            {
            ov_clear(&oggFile);
            std::cout << "Error decoding " << str << "..." << std::endl;
            delete ret;
            return nullptr;
            }
        // end if

        // Append to end of buffer
        bufferData.insert(bufferData.end(), array, array + bytes);
        }
    while (bytes > 0);

    // Clean up!
    ov_clear(&oggFile);

    alGenBuffers(1, &ret->buffer);

    alBufferData(ret->buffer, ret->format, &bufferData[0], static_cast<ALsizei>(bufferData.size()), ret->freq);
    //alSourcei(sourceID, AL_BUFFER, bufferID);
    return ret;
}

BufferData *Sound::loadWavFile(const char *filename){
  //Local Declarations
  BufferData *ret = new BufferData();
  FILE* soundFile = NULL;
  WAVE_Format wave_format;
  RIFF_Header riff_header;
  WAVE_Data wave_data;
  unsigned char* data;

  try {
    soundFile = fopen((const char*)filename, "rb");
    if (!soundFile)
      throw (filename);

    // Read in the first chunk into the struct
    fread(&riff_header, sizeof(RIFF_Header), 1, soundFile);

    //check for RIFF and WAVE tag in memeory
    if ((riff_header.chunkID[0] != 'R' ||
         riff_header.chunkID[1] != 'I' ||
         riff_header.chunkID[2] != 'F' ||
         riff_header.chunkID[3] != 'F') ||
        (riff_header.format[0] != 'W' ||
         riff_header.format[1] != 'A' ||
         riff_header.format[2] != 'V' ||
         riff_header.format[3] != 'E')){
             throw ("Invalid RIFF or WAVE Header");
    }

    //Read in the 2nd chunk for the wave info
    fread(&wave_format, sizeof(WAVE_Format), 1, soundFile);
    //check for fmt tag in memory
    if (wave_format.subChunkID[0] != 'f' ||
        wave_format.subChunkID[1] != 'm' ||
        wave_format.subChunkID[2] != 't' ||
        wave_format.subChunkID[3] != ' ')
             throw ("Invalid Wave Format");

    //check for extra parameters;
    if (wave_format.subChunkSize > 16)
        fseek(soundFile, sizeof(short), SEEK_CUR);

    //Read in the the last byte of data before the sound file
    fread(&wave_data, sizeof(WAVE_Data), 1, soundFile);
    //check for data tag in memory
    if (wave_data.subChunkID[0] != 'd' ||
        wave_data.subChunkID[1] != 'a' ||
        wave_data.subChunkID[2] != 't' ||
        wave_data.subChunkID[3] != 'a')
             throw ("Invalid data header");

    //Allocate memory for data
    data = new unsigned char[wave_data.subChunk2Size];

    // Read in the sound data into the soundData variable
    if (!fread(data, wave_data.subChunk2Size, 1, soundFile))
        throw ("error loading WAVE data into struct!");

    //Now we set the variables that we passed in with the
    //data from the structs
    uint64_t size = wave_data.subChunk2Size;
    ret->freq = wave_format.sampleRate;
    //The format is worked out by looking at the number of
    //channels and the bits per sample.
    if (wave_format.numChannels == 1) {
        if (wave_format.bitsPerSample == 8 )
            ret->format = AL_FORMAT_MONO8;
        else if (wave_format.bitsPerSample == 16)
            ret->format = AL_FORMAT_MONO16;
    } else if (wave_format.numChannels == 2) {
        if (wave_format.bitsPerSample == 8 )
            ret->format = AL_FORMAT_STEREO8;
        else if (wave_format.bitsPerSample == 16)
            ret->format = AL_FORMAT_STEREO16;
    }
    //create our openAL buffer and check for success
    alGenBuffers(1, &ret->buffer);
    //errorCheck();
    //now we put our data into the openAL buffer and
    //check for success
    alBufferData(ret->buffer, ret->format, (void*)data,size, ret->freq);


    //alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
    //alSource3f(sourceID, AL_POSITION, 0.0f, 0.0f, 0.0f);

    //alSourcei(sourceID, AL_BUFFER, ret);

    //errorCheck();
    //clean up and return true if successful
    fclose(soundFile);
    return ret;
  } catch(const char * error) {
    //our catch statement for if we throw a string
    std::cerr << error << " : trying to load "
              << filename << std::endl;
    //clean up memory if wave loading fails
    if (soundFile != NULL)
        fclose(soundFile);
    //return false to indicate the failure to load wave
    return nullptr;
  }
}

BufferData *Sound::loadWavFileRW(SDL_RWops* soundFile){
  //Local Declarations
  BufferData *ret = new BufferData();
  WAVE_Format wave_format;
  RIFF_Header riff_header;
  WAVE_Data wave_data;
  unsigned char* data;

  try {
    // Read in the first chunk into the struct
    SDL_RWread(soundFile,&riff_header, sizeof(RIFF_Header), 1);

    //check for RIFF and WAVE tag in memeory
    if ((riff_header.chunkID[0] != 'R' ||
         riff_header.chunkID[1] != 'I' ||
         riff_header.chunkID[2] != 'F' ||
         riff_header.chunkID[3] != 'F') ||
        (riff_header.format[0] != 'W' ||
         riff_header.format[1] != 'A' ||
         riff_header.format[2] != 'V' ||
         riff_header.format[3] != 'E')){
             throw ("Invalid RIFF or WAVE Header");
    }

    //Read in the 2nd chunk for the wave info
    SDL_RWread(soundFile,&wave_format, sizeof(WAVE_Format), 1);
    //check for fmt tag in memory
    if (wave_format.subChunkID[0] != 'f' ||
        wave_format.subChunkID[1] != 'm' ||
        wave_format.subChunkID[2] != 't' ||
        wave_format.subChunkID[3] != ' ')
             throw ("Invalid Wave Format");

    //check for extra parameters;
    if (wave_format.subChunkSize > 16)
        SDL_RWseek(soundFile, sizeof(short), SEEK_CUR);

    //Read in the the last byte of data before the sound file
    SDL_RWread(soundFile,&wave_data, sizeof(WAVE_Data), 1);
    //check for data tag in memory
    if (wave_data.subChunkID[0] != 'd' ||
        wave_data.subChunkID[1] != 'a' ||
        wave_data.subChunkID[2] != 't' ||
        wave_data.subChunkID[3] != 'a')
             throw ("Invalid data header");

    //Allocate memory for data
    data = new unsigned char[wave_data.subChunk2Size];

    // Read in the sound data into the soundData variable
    if (!SDL_RWread(soundFile,data, wave_data.subChunk2Size, 1))
        throw ("error loading WAVE data into struct!");

    //Now we set the variables that we passed in with the
    //data from the structs
    uint64_t size = wave_data.subChunk2Size;
    ret->freq = wave_format.sampleRate;
    //The format is worked out by looking at the number of
    //channels and the bits per sample.
    if (wave_format.numChannels == 1) {
        if (wave_format.bitsPerSample == 8 )
            ret->format = AL_FORMAT_MONO8;
        else if (wave_format.bitsPerSample == 16)
            ret->format = AL_FORMAT_MONO16;
    } else if (wave_format.numChannels == 2) {
        if (wave_format.bitsPerSample == 8 )
            ret->format = AL_FORMAT_STEREO8;
        else if (wave_format.bitsPerSample == 16)
            ret->format = AL_FORMAT_STEREO16;
    }
    //create our openAL buffer and check for success
    alGenBuffers(1, &ret->buffer);
    //errorCheck();
    //now we put our data into the openAL buffer and
    //check for success
    alBufferData(ret->buffer, ret->format, (void*)data,size, ret->freq);


    //alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
    //alSource3f(sourceID, AL_POSITION, 0.0f, 0.0f, 0.0f);

    //alSourcei(sourceID, AL_BUFFER, ret);

    //errorCheck();
    //clean up and return true if successful

    return ret;
  } catch(const char * error) {
    //our catch statement for if we throw a string

    //clean up memory if wave loading fails
    //return false to indicate the failure to load wave
    return nullptr;
  }
}

bool Sound::Open(const char *str,bool isWave){
    std::string stdnamee(str);
    if (stdnamee.find(":")!=std::string::npos){
        snd = GlobalAssetManager::GetInstance().makeSound(false,ResourceManager::GetInstance().GetFile(stdnamee),str);
    }else{
        snd = GlobalAssetManager::GetInstance().makeSound(false,str);
    }
}

BufferData* Sound::Preload(SDL_RWops* file,std::string name){
    if (!ConfigManager::GetInstance().IsWorkingAudio())
        return NULL;
    if (!file){
        return nullptr;
    }
    char theName[4];
    SDL_RWread(file,&theName[0], 1, 1);
    SDL_RWread(file,&theName[1], 1, 1);
    SDL_RWread(file,&theName[2], 1, 1);
    theName[3] = 0;
    SDL_RWseek(file, 0, RW_SEEK_SET);
    if (theName[0] == 'O' && theName[1] == 'g' && theName[2] == 'g'){
        //return loadOggFile(file);
    }else{
        return loadWavFileRW(file);
    }
    return nullptr;
}
BufferData* Sound::Preload(std::string stdnamee){
    if (!ConfigManager::GetInstance().IsWorkingAudio())
        return NULL;
    if (stdnamee.find(":")!=std::string::npos){
        return Preload(ResourceManager::GetInstance().GetFile(stdnamee),stdnamee);
    }
    GameFile f(stdnamee);
    if (f.IsOpen()){
        char theName[4];
        theName[0] = f.ReadByte();
        theName[1] = f.ReadByte();
        theName[2] = f.ReadByte();
        theName[3] = 0;
        if (theName[0] == 'O' && theName[1] == 'g' && theName[2] == 'g'){
            f.Close();
            return loadOggFile(stdnamee.c_str());
        }else{
            return loadWavFile(stdnamee.c_str());
        }
    }else{
        return nullptr;
    }
}


int Sound::PlayOnce(const char *s,bool global,int volume){
    if (!ConfigManager::GetInstance().IsWorkingAudio())
        return -1;
    std::string stdnamee(s);
    SoundPtr snd;
    if (stdnamee.find(":")!=std::string::npos){
        if (global)
            snd = GlobalAssetManager::GetInstance().makeSound(false,ResourceManager::GetInstance().GetFile(stdnamee),stdnamee);
        else
            snd = Game::GetCurrentState().Assets.makeSound(false,ResourceManager::GetInstance().GetFile(stdnamee),stdnamee);
    }else{
        if (global)
            snd = GlobalAssetManager::GetInstance().makeSound(false,s);
        else
            snd = Game::GetCurrentState().Assets.makeSound(false,s);
    }

    if (snd.get()){
        ALuint sourceID;
        alGenSources(1,&sourceID);
        alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
        alSource3f(sourceID, AL_POSITION, 0.0f, 0.0f, 0.0f);

        alSourcei(sourceID, AL_BUFFER, snd.get()->buffer);
        alSourcePlay(sourceID);
        return snd.get()->buffer;
    }else{
        bear::out << "Cannot play " << s << " because " << SDL_GetError() << "\n";
    }
    return -1;
}


void Sound::SetVolume(int vol){
     if (!working)
        return;
    if (!IsOpen())
        return;
    if (!IsPlaying())
        return;

    //Mix_Volume(channel,vol);
}

void Sound::SetPosition(int vol){
     if (!working)
        return;
    if (!IsOpen())
        return;
    if (!IsPlaying())
        return;

    //Mix_SetPosition(channel,0,vol);
}

void Sound::Play(int times){
    if (!working)
        return;
    ALuint sourceID;
    alGenSources(1,&sourceID);
    alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
    alSource3f(sourceID, AL_POSITION, 0.0f, 0.0f, 0.0f);

    alSourcei(sourceID, AL_BUFFER, snd.get()->buffer);
    alSourcePlay(sourceID);

}

void Sound::Resume(){
    if (!working)
        return;
    if (!IsPlaying())
        return;
    //Mix_Resume(channel);
}
bool Sound::IsPlaying(){
    if (!working)
        return false;

    return true;
}
void Sound::Stop(){
    if (!working)
        return;
    if (!IsPlaying())
        return;
        //Mix_HaltChannel(channel);
}
void Sound::Pause(){
    if (!working)
        return;
    if (!IsPlaying())
        return;
        //Mix_Pause(channel);
}

void Sound::StopAll(){
    if (!working)
        return;
    //Mix_HaltChannel(-1);
}
void Sound::PauseAll(){
    if (!working)
        return;
    //Mix_Pause(-1);
}
void Sound::ResumeAll(){
    if (!working)
        return;
    //Mix_Resume(-1);
}

void Sound::FadeOut(float ms){
    if (!working)
        return;
    if (!IsOpen())
        return;
    if (!IsPlaying())
        return;
        //Mix_FadeOutChannel(channel, ms);
}



bool Sound::IsOpen(){
    return true;
}

