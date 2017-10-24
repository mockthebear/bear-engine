#include "gamefile.hpp"
#include "dirmanager.hpp"
#include "resourcemanager.hpp"
#include "../performance/console.hpp"
#include "utils.hpp"
#include <iostream>
#include <climits>

GameFile::GameFile(){
    m_cache = NULL;
    m_size = 0;
    m_filePos = 0;
    m_filePointer = NULL;
    fromResource = false;
}

GameFile::GameFile(std::string name,bool notify){
    m_cache = NULL;
    m_size = 0;
    m_filePos = 0;
    m_filePointer = NULL;
    fromResource = false;
    Open(name,notify);
}



GameFile::GameFile(const char *name,bool notify){
    m_cache = NULL;
    m_size = 0;
    m_filePos = 0;
    m_filePointer = NULL;
    fromResource = false;
    Open(name,notify);
}
GameFile::~GameFile(){
    Close();
    ClearCache();
}

bool GameFile::Open(std::string name,bool notify){
	std::string aux = name;
	if (ResourceManager::IsValidResource(name)){
        m_filePointer = ResourceManager::GetInstance().GetFile(name); //safe
    }else{
        name = DirManager::AdjustAssetsPath(name);
        m_filePointer = SDL_RWFromFile(name.c_str(),"rb");
        ParseFile();
        if (m_size == ULONG_MAX){
            Console::GetInstance().AddText(utils::format("File %s is a dir", name ) );
            m_filePointer = nullptr;
            return false;
        }

    }
    if (m_filePointer == NULL){
        m_filePointer = SDL_RWFromFile(name.c_str(),"rb");
        if (!m_filePointer){
            if (notify){
                Console::GetInstance().AddText(utils::format("Cannost locate %s", name ) );
            }
            return false;
        }
        ParseFile();
        if (m_size == LONG_MAX){
            SDL_RWclose(m_filePointer);
            Console::GetInstance().AddText(utils::format("File %s is a dir", name ) );
            m_filePointer = nullptr;
            return false;
        }
    }
    fromResource = false;
    ParseFile();


    return true;
}

void GameFile::ParseFile(){
    if (m_filePointer){
        SDL_RWseek(m_filePointer, 0L, RW_SEEK_END);
        m_size = SDL_RWtell(m_filePointer);
        SDL_RWseek(m_filePointer, 0L, RW_SEEK_SET);
        m_filePos = 0;
    }
}

bool GameFile::ClearCache(){
    if (!IsCached()){
        return false;
    }
    delete []m_cache;
    m_cache = NULL;
    return true;
}

char *GameFile::PopCache(){
    if (!IsOpen() || !IsCached())
        return NULL;
    char *oldCache = m_cache;
    m_cache = NULL;
    return oldCache;
}
bool GameFile::Close(){
    if (!IsOpen())
        return false;
    if (m_filePointer)
        SDL_RWclose(m_filePointer);
    m_filePointer = NULL;
    return true;
}

char *GameFile::GetCache(){
    if (!IsOpen() || !IsCached())
        return NULL;
    char *l_cache = new char[m_size+1];
    for (uint32_t i=0;i<m_size+1;i++){
        l_cache[i] = m_cache[i];
    }
    return l_cache;
}

bool GameFile::Cache(){
    if (!IsOpen() || IsCached())
        return false;
    m_cache = new char[m_size+1];
    SDL_RWread(m_filePointer,m_cache, 1, m_size);
    m_cache[m_size] = '\0';
    return true;
};

char GameFile::ReadByte(){
    if (IsCached()){
        if (m_filePos > m_size){
            return 0;
        }
        char l_char = m_cache[m_filePos];
        m_filePos++;
        return l_char;
    }else{
        if (!IsOpen())
            return '\0';
        m_filePos++;
        char byt;
        SDL_RWread(m_filePointer,&byt, 1, 1);
        return byt;
    }
}
uint8_t GameFile::Read8(){
    if (IsCached()){
        if (m_filePos > m_size){
            return 0;
        }
        uint8_t l_char = (uint8_t)m_cache[m_filePos];
        m_filePos++;
        return l_char;
    }else{
        if (!IsOpen())
            return 0;
        m_filePos++;
        uint8_t byt=0;
        SDL_RWread(m_filePointer,&byt, 1, 1);
        return (byt);
    }
}

int GameFile::GetNumber(bool ignoreUntilFind){
    int ret = 0;
    bool found = false;
    while (true){
        if (m_filePos > m_size)
            return -1;
        char c = ReadByte();
        if (c >= '0' && c <= '9'){
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
    return ret;
}
std::string GameFile::ReadWord(int n,char separator){
    std::string buffer = "";
    buffer = ReadUntil(separator);
    while (n > 0){
        n--;
        buffer += separator;
        buffer += ReadUntil(separator);
    }
    return buffer;
}
std::string GameFile::ReadUntil(char rd){
    std::string buffer = "";
    while (m_filePos < m_size){
        char c = ReadByte();
        if (c == rd){
            return buffer;
        }
        buffer += c;
    }
    return buffer;
}

uint32_t GameFile::Read(char *c,uint32_t size){
    uint32_t read = 0;
    int kek = size;
    while (kek > 0 && m_filePos < m_size){
        c[read] = ReadByte();
        read++;
        kek--;
    }
    return read;
}
std::string GameFile::Read(uint16_t size){
    std::string buffer = "";
    while (m_filePos < m_size || size > 0){
        char c = ReadByte();
        buffer += c;
        size--;
    }
    buffer += '\0';
    return buffer;
}

bool GameFile::GetLine(std::string &line){
    if (!IsOpen())
        return false;
    if (m_filePos > m_size)
        return false;
    std::string buffer = "";
    bool finished = false;
    while (!finished){
        char c = ReadByte();
        if (m_filePos > m_size){
            line = buffer;
            return false;
        }
        if ((int)c == 13){
            continue;
        }
        if (c == '\n'){
            finished = true;
            line = buffer;
            return true;
        }else{
            buffer +=  c;
        }
    }
    return false;
}
bool GameFile::Seek(uint32_t pos){
    if (!IsOpen())
        return false;
    if (pos > m_size)
        return false;
    m_filePos = pos;
    return SDL_RWseek(m_filePointer, pos, RW_SEEK_SET) == 0;
}
int GameFile::FindInt(){
    int i =0;
    char by = ReadByte();
    while (by <= '0' || by >= '9'){
        by = ReadByte();
        if (Tell() > m_size){
            return -1;
        }
    }
    i = by-'0';
    by = ReadByte();
    while (by >= '0' && by <= '9'){
        i *= 10;
        i += by-'0';
        by = ReadByte();
        if (Tell() > m_size){
            return -1;
        }

    }
    Seek(Tell()-1);
    return i;
}



