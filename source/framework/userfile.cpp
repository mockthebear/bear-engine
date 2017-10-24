#include "userfile.hpp"
#include "dirmanager.hpp"
#include <stdio.h>
#include <stdlib.h>

UserFile::UserFile(){
    fptr = NULL;
}
bool UserFile::Close(){
    if (fptr){
        fname = "";
        SDL_RWclose(fptr);
        fptr = NULL;
        return true;
    }
    return false;
}

bool UserFile::Open(std::string name){
    if (!fptr){
        fname = name;
        fptr = SDL_RWFromFile(UserFile::GetFilePath().c_str(),"wb");
        if (!fptr){
            writePos = 0;
            fname = "";
            return false;
        }
        return true;
    }
    return false;
}

std::string UserFile::GetFilePath(){
    return DirManager::AdjustUserPath(fname);
}

uint32_t UserFile::Write8(char c){
    if (fptr){
        fputc(c,fptr);
        writePos++;
        return writePos;
    }
    return 0;
}
uint32_t UserFile::WriteU8(uint8_t c){
    if (fptr){
        fputc(c,fptr);
        writePos++;
        return writePos;
    }
    return 0;
}
uint32_t UserFile::WriteU16(uint16_t c){
    if (fptr){
        fputc(c&255,fptr);
        fputc((c >> 8)&255,fptr);
    }
    return 0;
}
uint32_t UserFile::WriteU32(uint32_t c){
    if (fptr){
        fputc(c&255,fptr);
        fputc((c >> 8)&255,fptr);
        fputc((c >> 16)&255,fptr);
        fputc((c >> 24)&255,fptr);
    }
    return 0;
}

uint32_t UserFile::WriteU64(uint64_t c){
    if (fptr){
        fputc(c&255,fptr);
        fputc((c >> 8)&255,fptr);
        fputc((c >> 16)&255,fptr);
        fputc((c >> 24)&255,fptr);
        fputc((c >> 32)&255,fptr);
        fputc((c >> 40)&255,fptr);
        fputc((c >> 48)&255,fptr);
        fputc((c >> 56)&255,fptr);
    }
    return 0;
}

uint32_t UserFile::Write(char *str,int size){
    if (fptr){
        writePos += size;
        SDL_RWwrite (fptr,str,sizeof(char), size );
        return writePos;
    }
    return 0;
}
uint32_t UserFile::Write(std::string str){
    if (fptr){
        writePos += str.size();
        SDL_RWwrite (fptr,str.c_str(),sizeof(char), str.size() );
        return writePos;
    }
    return 0;
}
