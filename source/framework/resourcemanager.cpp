#include "resourcemanager.hpp"
#include "dirmanager.hpp"
#include <iostream>
#include "utils.hpp"
#include "../performance/console.hpp"
std::unordered_map<std::string, ResourceFile*> ResourceManager::resources;


ResourceManager::ResourceManager(){

}

ResourceManager& ResourceManager::GetInstance(){
    static ResourceManager Singleton;
    return Singleton;
}


bool ResourceManager::Load(std::string file,std::string alias){
    if (alias == "")
        alias = file;
    if (resources[alias]){
        Console::GetInstance().AddTextInfo(utils::format("Cannot load file 2[%s]",file.c_str()));
        return false;

    }
    ResourceFile *f = new ResourceFile();
    if (!f->Open(file)){
        Console::GetInstance().AddTextInfo(utils::format("Cannot load file 1[%s]",file.c_str()));
        delete f;
        return false;
    }
    resources[alias] = f;
    return true;
}
SDL_RWops* ResourceManager::GetFile(std::string file){
    std::size_t found = file.find(":");
    if (found==std::string::npos)
        return NULL;
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
        if (!Load(assetAlias))
            Console::GetInstance().AddTextInfo(utils::format("Cannot load file %s:%s",assetAlias.c_str(),fileName.c_str()));
            return NULL;
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
