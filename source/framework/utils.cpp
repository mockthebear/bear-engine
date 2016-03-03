#include "utils.hpp"
#include <math.h>


std::string utils::methodName(const std::string& prettyFunction){
    size_t colons = prettyFunction.find("::");
    size_t begin = prettyFunction.substr(0,colons).rfind(" ") + 1;
    size_t end = prettyFunction.rfind("(") - begin;
    std::string str = prettyFunction.substr(begin,end);
    return str;
}


uint8_t  utils::GetU8 (char *str,int pos){
    return str[pos];
}
uint16_t utils::GetU16(char *str,int pos){
    uint16_t *addr = (uint16_t*)(str+pos);
    return (*addr);
}
uint32_t utils::GetU24(char *str,int pos){
    uint32_t *addr = (uint32_t*)(str+pos);
    return (*addr)&0xffffff;
}
uint32_t utils::GetU32(char *str,int pos){
    uint32_t *addr = (uint32_t*)(str+pos);
    return *addr;
}
uint64_t utils::GetU64(char *str,int pos){
    uint64_t *addr = (uint64_t*)(str+pos);
    return *addr;
}

uint8_t  utils::GetU8c (char *str,int &pos){
       uint8_t r = GetU8(str,pos);
       pos += 1;
       return r;
}
uint16_t utils::GetU16c(char *str,int &pos){
       uint16_t r = GetU16(str,pos);
       pos += 2;
       return r;
}
uint32_t utils::GetU24c(char *str,int &pos){
       uint32_t r = GetU24(str,pos);
       pos += 3;
       return r;
}
uint32_t utils::GetU32c(char *str,int &pos){
       uint32_t r = GetU32(str,pos);
       pos += 4;
       return r;
}
uint64_t utils::GetU64c(char *str,int &pos){
       uint64_t r = GetU64(str,pos);
       pos += 8;
       return r;
}

bool utils::IsInLimits(int x,int y,int sx,int sy){
    if (x < 0 or x >= sx or y < 0 or y >= sy){
        return false;
    }
    return true;
}

uint32_t utils::GetAround(uint32_t**map ,int x,int y,int sx,int sy){
    float n = 0;
    double k = 0;

    for (int ay=-1;ay<=1;ay++){
         for (int ax=-1;ax<=1;ax++){
            if (IsInLimits(x+ax,y+ay,sx,sy)){
                n += map[y+ay][x+ax]*map[y+ay][x+ax];
                k++;
            }
        }
    }
    return sqrt(n/k);

}
