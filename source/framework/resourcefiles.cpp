#include "resourcefiles.hpp"
#include "utils.hpp"
#include "../performance/console.hpp"
#include <iostream>
#include <string.h>


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

	l_file.Open(dir,true);
    if (!l_file.IsOpen()){

        return false;
    }

    //l_file.Cache();

    m_fileSize = l_file.GetSize();
    m_cached = l_file.PopCache();


    if (m_cached == NULL){
        m_isCached = false;
    }else{
        m_isCached = true;
        l_file.Close();
    }

    int counter = 0;
    if (m_isCached){
        m_fileCount = utils::GetU16c(m_cached,counter);
    }else{
        m_fileCount = l_file.Read16();
    }
    m_header = 2;

    for (int i=0;i<m_fileCount;i++){
        int fileSize;
        int filePos;
        uint8_t nameSize;
        if (m_isCached){
            fileSize = utils::GetU32c(m_cached,counter);
            filePos = utils::GetU32c(m_cached,counter);
            nameSize = utils::GetU8c(m_cached,counter);
        }else{
            fileSize = l_file.Read32();
            filePos = l_file.Read32();
            nameSize = l_file.Read8();
        }
        m_header += 9;
        char *name = new char[nameSize+1];
        for (uint8_t c=0;c<nameSize;c++){
            if (m_isCached){
                name[c] = utils::GetU8c(m_cached,counter);
            }else{
                name[c] = l_file.ReadByte();
            }
        }
        m_header += nameSize;
        name[nameSize] = '\0';
        std::string lName(name);
        delete [] name;
        m_fileData[lName] = std::tuple<uint32_t,uint32_t>(filePos,fileSize);

    }

    return true;
}
void ResourceFile::Close(){
    if (IsOpen()){
        if (m_isCached){
            delete m_cached;
            m_cached = NULL;
        }else{
            l_file.Close();
        }
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
    if (m_isCached){
        return SDL_RWFromMem((m_cached+offset+m_header),size);
    }else{
        l_file.Seek(offset+m_header);
        char *auxB = new char[size+1];
        l_file.Read(auxB,size);
        SDL_RWops* rw = SDL_RWFromMem(auxB,size);
        rw->hidden.unknown.data1 = auxB;
        rw->close = [](SDL_RWops * me) -> int{
            char *ptr = (char*)me->hidden.unknown.data1;
            if (ptr){
                delete []ptr;
            }
            return 0;
        };
        return rw;
    }

}

char *ResourceFile::GetFileStream(std::string name,int &size){
    if (!std::get<1>(m_fileData[name])){
        Console::GetInstance().AddText(utils::format("Cannot get file stream from [%s]. Asset not in resource file.",name.c_str()));
        return NULL;
    }
    int offset = std::get<0>(m_fileData[name]);
    size = std::get<1>(m_fileData[name]);
    char *stream = new char[size+1];

    if (m_isCached){
        for (int i=0;i<size;i++){
            stream[i] = (m_cached+offset+m_header)[i];
        }
    }else{
        l_file.Seek(offset+m_header);
        l_file.Read(stream,size);
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

    if (m_isCached){
        return SDL_RWFromMem((m_cached+offset+m_header),size);
    }else{
        l_file.Seek(offset+m_header);
        char *auxB = new char[size+1];
        l_file.Read(auxB,size);
        SDL_RWops* rw = SDL_RWFromMem(auxB,size);
        rw->hidden.unknown.data1 = auxB;
        rw->close = [](SDL_RWops * me) -> int{
            char *ptr = (char*)me->hidden.unknown.data1;
            if (ptr){
                delete ptr;
            }
            return 0;
        };
        return rw;
    }
}
int ResourceFile::GetFileSize(std::string name){
    if (!std::get<1>(m_fileData[name])){
        return false;
    }
    return std::get<1>(m_fileData[name]);
}
