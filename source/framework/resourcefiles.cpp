#include "resourcefiles.hpp"
#include "gamefile.hpp"
#include "utils.hpp"
#include "../performance/console.hpp"
#include <iostream>


ResourceFile::ResourceFile(){
    m_isOpen = false;
    m_cached = NULL;
}
ResourceFile::ResourceFile(std::string dir){
    m_cached = NULL;
    m_isOpen = Open(dir);
}
ResourceFile::~ResourceFile(){
    Close();
}

bool ResourceFile::Open(std::string dir){
    GameFile l_file(dir,true);
    if (!l_file.IsOpen()){
        return false;
    }
    l_file.Cache();
    m_fileSize = l_file.GetSize();
    m_cached = l_file.PopCache();
    l_file.Close();
    if (m_cached == NULL)
        return false;

    int counter = 0;
    m_fileCount = utils::GetU16c(m_cached,counter);
    m_header = 2;

    for (int i=0;i<m_fileCount;i++){
        int fileSize = utils::GetU32c(m_cached,counter);
        m_header += 4;
        int filePos = utils::GetU32c(m_cached,counter);
        m_header += 4;
        uint8_t nameSize = utils::GetU8c(m_cached,counter);
        m_header += 1;
        char *name = new char[nameSize+1];
        for (uint8_t c=0;c<nameSize;c++){
            name[c] = utils::GetU8c(m_cached,counter);
        }
        m_header += nameSize;
        name[nameSize] = '\0';
        std::string lName(name);
        delete name;
        m_fileData[lName] = std::tuple<uint32_t,uint32_t>(filePos,fileSize);
    }
    return true;
}
void ResourceFile::Close(){
    if (IsOpen()){
        delete m_cached;
        m_cached = NULL;
    }
    m_isOpen = false;
}

bool ResourceFile::IsOpen(){
    return m_isOpen;
}

int ResourceFile::GetFileCount(){
    return m_fileCount;
}
std::vector<std::string> ResourceFile::ListFiles(){
    std::vector<std::string> l_list;
    for (auto &it : m_fileData){
        l_list.emplace_back(it.first);
    }
    return l_list;
}
SDL_RWops* ResourceFile::GetFile(std::string name){
    if (!std::get<1>(m_fileData[name])){
        return NULL;
    }
    int offset = std::get<0>(m_fileData[name]);
    int size = std::get<1>(m_fileData[name]);

    return SDL_RWFromMem((m_cached+offset+m_header),size);

}

char *ResourceFile::GetFileStream(std::string name,int &size){
    if (!std::get<1>(m_fileData[name])){
        Console::GetInstance().AddText(utils::format("Cannot get file stream from [%s]. Asset not in resource file.",name.c_str()));
        return NULL;
    }
    int offset = std::get<0>(m_fileData[name]);
    size = std::get<1>(m_fileData[name]);
    char *stream = new char[size+1];
    for (int i=0;i<size;i++){
        stream[i] = (m_cached+offset+m_header)[i];
    }
    stream[size] = '\0';
    return stream;
}

SDL_RWops* ResourceFile::GetFile(std::string name,int &size){
    if (!std::get<1>(m_fileData[name])){
        Console::GetInstance().AddText(utils::format("Cannot get file rwop [%s]. Asset not in resource file.",name.c_str()));
        return NULL;
    }

    int offset = std::get<0>(m_fileData[name]);
    size = std::get<1>(m_fileData[name]);
    return SDL_RWFromMem((m_cached+offset+m_header),size);
}
int ResourceFile::GetFileSize(std::string name){
    if (!std::get<1>(m_fileData[name])){
        return false;
    }
    return std::get<1>(m_fileData[name]);
}
