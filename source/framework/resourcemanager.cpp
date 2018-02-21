#include "resourcemanager.hpp"
#include "dirmanager.hpp"
#include <iostream>
#include "utils.hpp"
#include "../performance/console.hpp"
std::unordered_map<std::string, ResourceFile*> ResourceManager::resources;


ResourceManager::ResourceManager(){

}
void ResourceManager::ClearAll(){
    for (auto &it : resources){
       delete it.second;
    }
    resources.clear();
}

ResourceManager& ResourceManager::GetInstance(){
    static ResourceManager Singleton;
    return Singleton;
}

char *ResourceManager::GetFileBuffer(SDL_RWops* rw,uint64_t &size){
	if (!rw){
        return nullptr;
	}
	uint64_t res_size = SDL_RWsize(rw);
    char* res =new char[res_size + 1];
    uint64_t nb_read_total = 0, nb_read = 1;
    char* buf = res;
    while (nb_read_total < res_size && nb_read != 0) {
            nb_read = SDL_RWread(rw, buf, 1, (res_size - nb_read_total));
            nb_read_total += nb_read;
            buf += nb_read;
    }
    res[nb_read_total] = '\0';
    size = (res_size + 1);
    return res;
}

void ResourceManager::ClearFileBuffer(char* buff){
    delete [] buff;
}

bool ResourceManager::Load(std::string file,std::string alias){
    if (alias == "")
        alias = file;

    if (resources[alias]){
        Console::GetInstance().AddTextInfo(utils::format("Not loading [%s] because alias already in use.",file.c_str()));
        return false;

    }

    ResourceFile *f = new ResourceFile();
    if (!f->Open(file)){
        Console::GetInstance().AddTextInfo(utils::format("Cannot load file [%s]",file.c_str()));
        delete f;
        return false;
    }

    resources[alias] = f;
    return true;
}

bool ResourceManager::IsValidResource(std::string name){
    std::size_t found = name.find(":");
    if (found==std::string::npos)
        return false;
    if (found <= 3)
        return false;
    return true;
};

SDL_RWops* ResourceManager::GetFile(std::string file){
    std::size_t found = file.find(":");
    if (found==std::string::npos)
        return NULL;
    if (found <= 3){
        Console::GetInstance().AddTextInfo(utils::format("Trying to load resourcefile named [%s]. Too shor name.\n",file.substr(0,found).c_str() ));
        return NULL;
    }
    std::string alias = file.substr(0,found);
    std::string name = file.substr(found+1);
    return GetFile(alias,name);
}

char* ResourceManager::GetFileData(std::string assetAlias,std::string fileName){
    if (!resources[assetAlias]){
        if (!Load(assetAlias)){
            Console::GetInstance().AddTextInfo(utils::format("Cannot load file %s:%s",assetAlias.c_str(),fileName.c_str()));
            return NULL;
        }
    }
    int size;
    return resources[assetAlias]->GetFileStream(fileName,size);

}
SDL_RWops* ResourceManager::GetFile(std::string assetAlias,std::string fileName){
    if (!resources[assetAlias]){
        if (!Load(assetAlias)){
            Console::GetInstance().AddTextInfo(utils::format("Cannot load file %s:%s",assetAlias.c_str(),fileName.c_str()));
            return NULL;
        }
    }
    return resources[assetAlias]->GetFile(fileName);
}

bool ResourceManager::Erase(std::string assetAlias){
    if (!resources[assetAlias]){
        return false;
    }
    delete resources[assetAlias];
    resources[assetAlias] = NULL;
    return true;
}
