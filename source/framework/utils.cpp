#include "utils.hpp"
#include <math.h>


std::string utils::methodName(const std::string& prettyFunction){
    size_t colons = prettyFunction.find("::");
    size_t begin = prettyFunction.substr(0,colons).rfind(" ") + 1;
    size_t end = prettyFunction.rfind("(") - begin;
    std::string str = prettyFunction.substr(begin,end);
    return str;
}


char utils::GetByte (char *str,int pos){
    return str[pos];
}
uint8_t  utils::GetU8 (char *str,int pos){
    return str[pos];
}
uint16_t utils::GetU16(char *str,int pos){
    #ifndef __EMSCRIPTEN__
    uint16_t *addr = (uint16_t*)(str+pos);
    return (*addr);
    #else
    uint16_t r =  (GetU8(str,pos)) + (GetU8(str,pos+1) << 8);
    return r;
    #endif
}
uint32_t utils::GetU24(char *str,int pos){
    #ifndef __EMSCRIPTEN__
    uint32_t *addr = (uint32_t*)(str+pos);
    return (*addr)&0xffffff;
    #else
    uint32_t r = (GetU8(str,pos) ) + (GetU8(str,pos+1) << 8) + (GetU8(str,pos+2) << 16);
    return r;
    #endif
}
uint32_t utils::GetU32(char *str,int pos){
    #ifndef __EMSCRIPTEN__
    uint32_t *addr = (uint32_t*)(str+pos);
    return *addr;
    #else
    uint32_t r = (GetU8(str,pos) ) + (GetU8(str,pos+1) << 8) + (GetU8(str,pos+2) << 16) + (GetU8(str,pos+3) << 24);
    return r;
    #endif

}
uint64_t utils::GetU64(char *str,int pos){
    #ifndef __EMSCRIPTEN__
    uint64_t *addr = (uint64_t*)(str+pos);
    return *addr;
    #else
    uint64_t r =  ((uint64_t)GetByte(str,pos) << 56) + ((uint64_t)GetByte(str,pos+1) << 48) +((uint64_t)GetByte(str,pos+2) << 40) +((uint64_t)GetByte(str,pos+3) << 32)
    + (GetByte(str,pos+4) << 24) + (GetByte(str,pos+5) << 16) + (GetByte(str,pos+6) << 8) + (GetByte(str,pos+7));
    return r;
    #endif
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



int utils::GetNumber(std::string &str,bool ignoreUntilFind){
    int ret = 0;
    bool found = false;
    uint32_t pos = 0;
    while (true){
        if (pos >= str.length())
            return ret;
        char c = str[pos];
        pos++;
        if (c >= '0' and c <= '9'){
            ret *= 10;
            ret += c-'0';
            found = true;
        }else{
            if (ignoreUntilFind){
                if (!found)
                    continue;
            }
            break;
        }
    }
    str = str.substr(pos);
    return ret;
}
int utils::TrimString(std::string &str,char tocut){
    std::string buffer = "";
    uint32_t pos = 0;
    uint32_t counter = 0;
    while (pos < str.length()){
        char c = str[pos];
        if (c != tocut ){
            buffer += c;
            counter++;
        }

        pos++;

    }
    str = buffer;
    return counter;
}
std::string utils::ReadWord(std::string &str,int n,char separator){
    std::string buffer = "";
    uint32_t pos = 0;
    while (pos < str.length()){
        char c = str[pos];
        if (!(c >= 'a' && c <= 'z') && !(c >= 'A' && c <= 'Z') || c == separator ){
            str = str.substr(pos);
            return buffer;
        }
        pos++;
        buffer += c;
    }
    str = str.substr(pos);
    return buffer;
}

std::string utils::ReadUntil(std::string &str,std::string until){
    std::string buffer = "";
    uint32_t pos = 0;
    while (pos < str.length()){
        char c = str[pos];
        pos++;
        if (c == until[0]){
            if (until == str.substr(pos-1,until.length())){
                str = str.substr(pos+until.length()-1);
                return buffer;
            }
        }
        buffer += c;
    }
    pos = 0;
    str = str.substr(pos);
    return buffer;
}
std::string utils::ReadUntil(std::string &str,char rd){
    std::string buffer = "";
    uint32_t pos = 0;
    while (pos < str.length()){
        char c = str[pos];
        pos++;
        if (c == rd){
            str = str.substr(pos);
            return buffer;
        }
        buffer += c;
    }
    str = str.substr(pos);
    return buffer;
}
std::string utils::GetLine(std::string &str){
    return ReadUntil(str,'\n');
}
