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

TexturePtr AssetMannager::makeTexture(bool forced,std::string str,int fcount,float ftime,int rep,bool hasAliasing) {
    if (not spriteMap[str] or forced){
        SDL_Texture* c =Sprite::Preload((char*)str.c_str(),true,hasAliasing);
        if (not c){
            return TexturePtr();
        }
        if (forced and spriteMap[str].get()){
            spriteMap[str].reset(c);
            Console::GetInstance().AddTextInfoF("FORCED: Made texture for [%s] in  manager %d",str,id);
        }else{
            spriteMap[str] = chain_ptr<SDL_Texture>::make(c);
            Console::GetInstance().AddTextInfoF("Made texture for [%s] in  manager %d",str,id);
        }
        return spriteMap[str];
    }
    return spriteMap[str];
}

TexturePtr AssetMannager::makeTexture(bool forced,SDL_RWops* rw,std::string str,bool hasAntiAliasign)  {
    if (not spriteMap[str] or forced){
        if (not rw){
            return TexturePtr();
        }
        SDL_Texture* c =Sprite::Preload(rw,str,hasAntiAliasign);
        if (not c){
            Console::GetInstance().AddTextInfoF("Failed to create. %s in %d",str,id);
            return TexturePtr();
        }

        if (forced and spriteMap[str].get()){
            spriteMap[str].reset(c);
            Console::GetInstance().AddTextInfoF("FORCED: Made texture RW for [%s] in  manager %d",str,id);
        }else{
            spriteMap[str] = chain_ptr<SDL_Texture>::make(c);
            Console::GetInstance().AddTextInfoF("Made texture RW for [%s] in  manager %d",str,id);
        }
        return spriteMap[str];
    }

    return spriteMap[str];
}

TexturePtr AssetMannager::makeTexture(bool forced,std::string str,ColorReplacer &r,bool HasAliasing)  {
    if (not spriteMap[str] or forced){
        SDL_Texture* c =Sprite::Preload(str,r,HasAliasing);
        if (not c){
            return TexturePtr();
        }

        if (forced and spriteMap[str].get()){
            spriteMap[str].reset(c);
            Console::GetInstance().AddTextInfoF("FORCED: Made replace texture for [%s] in  manager %d",str,id);
        }else{
            spriteMap[str] = chain_ptr<SDL_Texture>::make(c);
            Console::GetInstance().AddTextInfoF("Made replace texture for [%s] in  manager %d",str,id);
        }
        return spriteMap[str];
    }

    return spriteMap[str];
}

SoundPtr AssetMannager::makeSound(bool forced,std::string str){
    if (not soundMap[str] or forced){
        Mix_Chunk* c = Sound::Preload(str);
        if (not c){
            return SoundPtr();
        }
        if (forced and soundMap[str].get()){
            soundMap[str].reset(c);
        }else{
            soundMap[str] = chain_ptr<Mix_Chunk>::make(c);
            Console::GetInstance().AddTextInfoF("Lets alloc [%s] in %d",str,id);
        }
        return soundMap[str];
    }
    return soundMap[str];
}
SoundPtr AssetMannager::makeSound(bool forced,SDL_RWops* rw,std::string str){
    if (not soundMap[str] or forced){
        if (not rw){
            return SoundPtr();
        }
        Mix_Chunk* c = Sound::Preload(rw,str);
        if (not c){
            return SoundPtr();
        }
        if (forced and soundMap[str].get()){
            soundMap[str].reset(c);
        }else{
            soundMap[str] = chain_ptr<Mix_Chunk>::make(c);
            Console::GetInstance().AddTextInfoF("Lets alloc rw[%s] in %d",str,id);
        }
        return soundMap[str];
    }
    return soundMap[str];
}

bool AssetMannager::erase(){
    for (auto &it : spriteMap){
        SDL_DestroyTexture( it.second.get() );
    }
    return true;
}
