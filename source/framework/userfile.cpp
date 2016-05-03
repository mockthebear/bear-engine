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
        fclose(fptr);
        fptr = NULL;
        return true;
    }
    return false;
}

bool UserFile::Open(std::string name){
    if (!fptr){
        fname = name;
        fptr = fopen(UserFile::GetFilePath().c_str(),"w");
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
        char msg[2];
        *((uint16_t*)msg) = c;
        Write(msg,2);
    }
    return 0;
}
uint32_t UserFile::WriteU32(uint32_t c){
    if (fptr){
        char msg[4];
        *((uint32_t*)msg) = c;
        Write(msg,4);
    }
    return 0;
}
uint32_t UserFile::WriteU64(uint64_t c){
    if (fptr){
        char msg[8];
        *((uint64_t*)msg) = c;
        Write(msg,8);
    }
    return 0;
}

uint32_t UserFile::Write(char *str,int size){
    if (fptr){
        writePos += size;
        fwrite (str,sizeof(char), size,fptr );
        return writePos;
    }
    return 0;
}
uint32_t UserFile::Write(std::string str){
    if (fptr){
        writePos += str.size();
        fprintf(fptr,"%s",str.c_str());
        return writePos;
    }
    return 0;
}
