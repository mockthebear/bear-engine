#include "assetmanager.hpp"
#include "../performance/console.hpp"
#include <iostream>


uint32_t AssetMannager::ManagerId = 0;
std::map<uint32_t,AssetMannager*> AssetMannager::AssetMannagerMap;


GlobalAssetManager &GlobalAssetManager::GetInstance(){
    static GlobalAssetManager mngr;
    return mngr;
}
UIAssetManager &UIAssetManager::GetInstance(){
    static UIAssetManager mngr;
    return mngr;
}

AssetMannager::AssetMannager(){
    ManagerId++;
    id = ManagerId;
    AssetMannagerMap[id] = this;
    Console::GetInstance().AddTextInfoF("Created with %d.",id);
}



AssetMannager::~AssetMannager(){

}
/*
TexturePtr AssetMannager::resource(SDL_RWops* rw,std::string str,bool hasAntiAliasign){

}
TexturePtr AssetMannager::resource(std::string str,bool hasAntiAliasign){

}
*/

TexturePtr AssetMannager::makeTexture(std::string str,bool hasAntiAliasign) {
    if (not spriteMap[str]){
        SDL_Texture* c =Sprite::Preload((char*)str.c_str(),true,hasAntiAliasign);
        if (not c){
            return TexturePtr();
        }
        spriteMap[str] = std::make_shared<SDL_Texture*>(c);
        Console::GetInstance().AddTextInfoF("Made texture for [%s] in  manager %d",str,id);
        return spriteMap[str];
    }
    return spriteMap[str];
}

TexturePtr AssetMannager::makeTexture(SDL_RWops* rw,std::string str,bool hasAntiAliasign)  {
    if (not spriteMap[str]){
        if (not rw){
            return TexturePtr();
        }
        SDL_Texture* c =Sprite::Preload(rw,str,hasAntiAliasign);
        if (not c){
            return TexturePtr();
        }
        spriteMap[str] = std::make_shared<SDL_Texture*>(c);
        Console::GetInstance().AddTextInfoF("Made RW texture for [%s] in  manager %d",str,id);
        return spriteMap[str];
    }

    return spriteMap[str];
}
TexturePtr AssetMannager::makeTexture(std::string str,ColorReplacer &r,bool HasAliasing)  {
    if (not spriteMap[str]){
        SDL_Texture* c =Sprite::Preload(str,r,HasAliasing);
        if (not c){
            return TexturePtr();
        }
        spriteMap[str] = std::make_shared<SDL_Texture*>(c);
        Console::GetInstance().AddTextInfoF("Made replace texture for [%s] in  manager %d",str,id);
        return spriteMap[str];
    }

    return spriteMap[str];
}

SoundPtr AssetMannager::makeSound(std::string str){
    if (not soundMap[str]){
        Mix_Chunk* c = Sound::Preload(str);
        if (not c){
            return SoundPtr();
        }
        soundMap[str] = std::make_shared<Mix_Chunk*>(c);
        Console::GetInstance().AddTextInfoF("Lets alloc [%s] in %d",str,id);
        return soundMap[str];
    }
    return soundMap[str];
}
SoundPtr AssetMannager::makeSound(SDL_RWops* rw,std::string str){
    if (not soundMap[str]){
        if (not rw){
            return SoundPtr();
        }
        Mix_Chunk* c = Sound::Preload(rw,str);
        if (not c){
            return SoundPtr();
        }
        soundMap[str] = std::make_shared<Mix_Chunk*>(c);
        Console::GetInstance().AddTextInfoF("Lets alloc [%s] in %d",str,id);
        return soundMap[str];
    }
    return soundMap[str];
}

bool AssetMannager::erase(){
    for (auto &it : spriteMap){
        SDL_DestroyTexture( *it.second.get() );
    }
    return true;
}
