#include "resourcemanager.hpp"
#include <iostream>
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
    if (resources[alias])
        return false;
    ResourceFile *f = new ResourceFile();
    if (!f->Open(file)){
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

SDL_RWops* ResourceManager::GetFile(std::string assetAlias,std::string fileName){
    if (!resources[assetAlias]){
        if (!Load(assetAlias))
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
