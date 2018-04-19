#include "userfile.hpp"
#include "dirmanager.hpp"
#include <stdio.h>
#include <stdlib.h>


UserFile::UserFile(){
    fptr = NULL;
    m_useStream = false;

}
bool UserFile::Close(){
    if (fptr){
        WriteStream();
        fname = "";
        SDL_RWclose(fptr);
        fptr = NULL;
        return true;
    }
    return false;
}

void UserFile::WriteStream(){
    if (m_useStream){
        SDL_RWwrite(fptr,&m_stream[0],m_stream.size(),1);
        m_stream.clear();
    }
}

bool UserFile::Open(std::string name){
    if (!fptr){
        fname = name;
        fptr = SDL_RWFromFile(UserFile::GetFilePath().c_str(),"wb");
        if (!fptr){
            fptr = SDL_RWFromFile(fname.c_str(),"wb");
            if (!fptr){
                writePos = 0;
                fname = "";
                return false;
            }else{
                m_stream.clear();
            }
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
        ufputc(c);
        writePos++;
        return writePos;
    }
    return 0;
}
uint32_t UserFile::WriteU8(uint8_t c){
    if (fptr){
        ufputc(c);
        writePos++;
        return writePos;
    }
    return 0;
}
uint32_t UserFile::WriteU16(uint16_t c){
    if (fptr){
        ufputc(c&255);
        ufputc((c >> 8)&255);
        writePos += 2;
        return writePos;
    }
    return 0;
}
uint32_t UserFile::WriteU32(uint32_t c){
    if (fptr){
        ufputc(c&255);
        ufputc((c >> 8)&255);
        ufputc((c >> 16)&255);
        ufputc((c >> 24)&255);
        writePos += 4;
        return writePos;
    }
    return 0;
}

uint32_t UserFile::WriteU64(uint64_t c){
    if (fptr){
        ufputc(c&255);
        ufputc((c >> 8)&255);
        ufputc((c >> 16)&255);
        ufputc((c >> 24)&255);
        ufputc((c >> 32)&255);
        ufputc((c >> 40)&255);
        ufputc((c >> 48)&255);
        ufputc((c >> 56)&255);
        writePos += 8;
        return writePos;
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
        if (m_useStream){
            for (char & c : str){
                m_stream.emplace_back(c);
            }
        }else{
            SDL_RWwrite (fptr,str.c_str(),sizeof(char), str.size() );
        }
        return writePos;
    }
    return 0;
}

