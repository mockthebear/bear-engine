#include "soundloader.hpp"
#include "../engine/bear.hpp"
#include "../framework/dirmanager.hpp"
#include <vorbis/vorbisfile.h>
char SoundLoader::array[BUFFER_SIZE];


size_t AR_readOgg(void* dst, size_t size1, size_t size2, void* fh)
{
    SDL_RWops* of = reinterpret_cast<SDL_RWops*>(fh);
    size_t len = SDL_RWread(of,dst, size1, size2);
    return len;
}

int AR_seekOgg( void *fh, ogg_int64_t to, int type ) {
    SDL_RWops* of = reinterpret_cast<SDL_RWops*>(fh);
    SDL_RWseek(of, to, type);
    return 0;
}

int AR_closeOgg(void* fh)
{
    return 0;
}

long AR_tellOgg( void *fh )
{
    SDL_RWops* of = reinterpret_cast<SDL_RWops*>(fh);
    return SDL_RWtell(of);
}


void SoundLoader::ShowError(std::string where){
    ALCenum error = alGetError();
    if (error != AL_NO_ERROR){
        if(error == AL_INVALID_NAME)
        {
            bear::out << "[AL ERROR] Invalid name ["<<where<<"]\n";
        }
        else if(error == AL_INVALID_ENUM)
        {
            bear::out << "[AL ERROR] Invalid enum ["<<where<<"]\n";
        }
        else if(error == AL_INVALID_VALUE)
        {
            bear::out << "[AL ERROR] Invalid value ["<<where<<"]\n";
        }
        else if(error == AL_INVALID_OPERATION)
        {
            bear::out << "[AL ERROR] Invalid operation ["<<where<<"]\n";
        }
        else if(error == AL_OUT_OF_MEMORY)
        {
            bear::out << "[AL ERROR] Out of memory ["<<where<<"]\n";
        }else{

            bear::out << "[AL ERROR] Don't know ["<<where<<"]\n";
        }
    }
}
BufferData *SoundLoader::loadOggFileRW(SDL_RWops* soundFile){
    BufferData *ret = new BufferData();
    int endian = 0;                         // 0 for Little-Endian, 1 for Big-Endian
    int bitStream;
    long bytes;
    std::vector<char> bufferData;

    if (soundFile == NULL)
        {
        bear::out << "Cannot open rw for reading...\n";
        delete ret;
        return nullptr;
    }
    // end if

    vorbis_info *pInfo;
    OggVorbis_File oggFile;
    // Try opening the given file
    ov_callbacks callbacks;
    callbacks.read_func = AR_readOgg;
    callbacks.seek_func = AR_seekOgg;
    callbacks.close_func = AR_closeOgg;
    callbacks.tell_func = AR_tellOgg;

    if (ov_open_callbacks((void *)soundFile, &oggFile, NULL, -1, callbacks) != 0){
        bear::out << "Error opening rw for decoding...\n";
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
            bear::out << "Error decoding rw...\n";
            delete ret;
            return nullptr;
            }
        // end if

        // Append to end of buffer
        //bear::out << "S: " << (uint64_t)bufferData.size() << " + "<<(uint64_t)bytes<<"\n";
        bufferData.insert(bufferData.end(), array, array + bytes);
        }
    while (bytes > 0);

    // Clean up!
    ov_clear(&oggFile);
    alGenBuffers(1, &ret->buffer);
    alBufferData(ret->buffer, ret->format, &bufferData[0], static_cast<ALsizei>(bufferData.size()), ret->freq);
    SoundLoader::ShowError("gen ogg rw");
    bufferData.clear();
    //alSourcei(sourceID, AL_BUFFER, bufferID);
    return ret;
}
BufferData *SoundLoader::loadOggFile(const char *str){
    std::string name(str);
    name = DirManager::AdjustAssetsPath(name);
    SDL_RWops *rw = SDL_RWFromFile(name.c_str(), "rb");
    if (!rw){
        bear::out << "Error: " << SDL_GetError() << "\n";
        return nullptr;
    }
    BufferData *sndBffer = loadOggFileRW(rw);
    SDL_RWclose(rw);
    return sndBffer;
}

BufferData *SoundLoader::loadWavFile(const char *filename){
    std::string name(filename);
    name = DirManager::AdjustAssetsPath(name);
    SDL_RWops *rw = SDL_RWFromFile(name.c_str(), "rb");
    if (!rw){
        bear::out << "Error: " << SDL_GetError() << "\n";
        return nullptr;
    }
    BufferData *sndBffer = loadWavFileRW(rw);
    SDL_RWclose(rw);
    return sndBffer;
}

BufferData *SoundLoader::loadWavFileRW(SDL_RWops* soundFile){
  BufferData *ret = new BufferData();
  WAVE_Format wave_format;
  RIFF_Header riff_header;
  WAVE_Data wave_data;
  unsigned char* data = nullptr;

  try {
    SDL_RWread(soundFile,&riff_header.chunkID, 4, 1);
    SDL_RWread(soundFile,&riff_header.chunkSize, 4, 1);
    SDL_RWread(soundFile,&riff_header.format, 4, 1);

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
    SDL_RWread(soundFile,&wave_format.subChunkID, 4, 1);
    SDL_RWread(soundFile,&wave_format.subChunkSize, 4, 1);
    SDL_RWread(soundFile,&wave_format.audioFormat, 2, 1);
    SDL_RWread(soundFile,&wave_format.numChannels, 2, 1);
    SDL_RWread(soundFile,&wave_format.sampleRate, 4, 1);
    SDL_RWread(soundFile,&wave_format.byteRate , 4, 1);
    SDL_RWread(soundFile,&wave_format.blockAlign , 2, 1);
    SDL_RWread(soundFile,&wave_format.bitsPerSample , 2, 1);
    if (wave_format.subChunkID[0] != 'f' ||
        wave_format.subChunkID[1] != 'm' ||
        wave_format.subChunkID[2] != 't' ||
        wave_format.subChunkID[3] != ' ')
             throw ("Invalid Wave Format");
            //unix wtf dont accept this
    if ((int32_t)wave_format.subChunkSize > 16){
       SDL_RWseek(soundFile, sizeof(short), SEEK_CUR);
    }

    //SDL_RWseek(soundFile, -2, SEEK_CUR);
    SDL_RWread(soundFile,&wave_data.subChunkID, 4, 1);
    SDL_RWread(soundFile,&wave_data.subChunk2Size, 4, 1);
    //bear::out << wave_data.subChunkID << "\n";
    if (wave_data.subChunkID[0] != 'd' ||
        wave_data.subChunkID[1] != 'a' ||
        wave_data.subChunkID[2] != 't' ||
        wave_data.subChunkID[3] != 'a')
             throw ("Invalid data header");
    data = new unsigned char[wave_data.subChunk2Size];
    if (!SDL_RWread(soundFile,data, wave_data.subChunk2Size, 1))
        throw ("error loading WAVE data into struct!");
    uint64_t size = wave_data.subChunk2Size;
    ret->freq = wave_format.sampleRate;
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

    alGenBuffers(1, &ret->buffer);
    SoundLoader::ShowError("gen b");
    alBufferData(ret->buffer, ret->format, (void*)data,size, ret->freq);
    SoundLoader::ShowError("bd");
    delete []data;
    SoundLoader::ShowError("gen wav f");
    return ret;
  } catch(const char * error) {
    if (data != nullptr)
        delete []data;
    bear::out << "Error: " << error << "\n";
    return nullptr;
  }
}
