#include "assetmanager.hpp"
#include "../performance/console.hpp"
#include "../sound/soundloader.hpp"
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
    setOutput = false;
}



AssetMannager::~AssetMannager(){

}
/*
TexturePtr AssetMannager::resource(SDL_RWops* rw,std::string str,bool hasAntiAliasign){

}
TexturePtr AssetMannager::resource(std::string str,bool hasAntiAliasign){

}
*/

TexturePtr AssetMannager::makeTexture(bool forced,std::string str,int ,float,int,bool hasAliasing) {
    if (!spriteMap[str] || forced){
        SDL_Texture* c =Sprite::Preload((char*)str.c_str(),true,hasAliasing);
        if (!c){
            return TexturePtr();
        }
        if (forced && spriteMap[str].get()){
            SDL_DestroyTexture( spriteMap[str].get() );
            spriteMap[str].reset(c);
            if (setOutput)
                Console::GetInstance().AddTextInfoF("FORCED: Made texture for [%s] in  manager %d",str,id);
        }else{
            spriteMap[str] = chain_ptr<SDL_Texture>::make(c);
            if (setOutput)
                Console::GetInstance().AddTextInfoF("Made texture for [%s] in  manager %d",str,id);
        }
        return spriteMap[str];
    }
    return spriteMap[str];
}

TexturePtr AssetMannager::makeTexture(bool forced,SDL_RWops* rw,std::string str,bool hasAntiAliasign)  {
    if (!spriteMap[str] || forced){
        if (!rw){
            return TexturePtr();
        }
        SDL_Texture* c =Sprite::Preload(rw,str,hasAntiAliasign);
        if (!c){
            Console::GetInstance().AddTextInfoF("Failed to create. %s in %d",str,id);
            return TexturePtr();
        }

        if (forced && spriteMap[str].get()){
            SDL_DestroyTexture( spriteMap[str].get() );
            spriteMap[str].reset(c);
            if (setOutput)
                Console::GetInstance().AddTextInfoF("FORCED: Made texture RW for [%s] in  manager %d",str,id);
        }else{
            spriteMap[str] = chain_ptr<SDL_Texture>::make(c);
            if (setOutput)
                Console::GetInstance().AddTextInfoF("Made texture RW for [%s] in  manager %d",str,id);
        }
        return spriteMap[str];
    }

    return spriteMap[str];
}

TexturePtr AssetMannager::makeTexture(bool forced,std::string str,ColorReplacer &r,bool HasAliasing)  {
    if (!spriteMap[str] || forced){
        SDL_Texture* c =Sprite::Preload(str,r,HasAliasing);
        if (!c){
            return TexturePtr();
        }

        if (forced && spriteMap[str].get()){
            SDL_DestroyTexture( spriteMap[str].get() );
            spriteMap[str].reset(c);
            if (setOutput)
                Console::GetInstance().AddTextInfoF("FORCED: Made replace texture for [%s] in  manager %d",str,id);
        }else{
            spriteMap[str] = chain_ptr<SDL_Texture>::make(c);
            if (setOutput)
                Console::GetInstance().AddTextInfoF("Made replace texture for [%s] in  manager %d",str,id);
        }
        return spriteMap[str];
    }

    return spriteMap[str];
}

SoundPtr AssetMannager::makeSound(bool forced,std::string str){
    if (!soundMap[str] || forced){
        BufferData *c = Sound::Preload(str);
        if (!c){
            return SoundPtr();
        }
        if (forced && soundMap[str].get()){
            BufferData *oldC = soundMap[str].get();
            alDeleteBuffers(1, &oldC->buffer);
            soundMap[str].reset(c);
        }else{
            soundMap[str] = chain_ptr<BufferData>::make(c);
            if (setOutput)
                Console::GetInstance().AddTextInfoF("Lets alloc [%s] in %d",str,id);
        }
        return soundMap[str];
    }
    return soundMap[str];
}
SoundPtr AssetMannager::makeSound(bool forced,SDL_RWops* rw,std::string str){
    if (!soundMap[str] || forced){
        if (!rw){
            return SoundPtr();
        }
        BufferData *c = Sound::Preload(rw,str);
        if (!c){
            return SoundPtr();
        }
        if (forced && soundMap[str].get()){
            BufferData *oldC = soundMap[str].get();
            alDeleteBuffers(1, &oldC->buffer);
            soundMap[str].reset(c);
        }else{
            soundMap[str] = chain_ptr<BufferData>::make(c);
            if (setOutput)
                Console::GetInstance().AddTextInfoF("Lets alloc rw[%s] in %d",str,id);
        }
        return soundMap[str];
    }
    return soundMap[str];
}

bool AssetMannager::erase(){
    for (auto &it : spriteMap){
        if (it.second.get()){
            SDL_DestroyTexture( it.second.get() );
            it.second.reset(nullptr);
            it.second.destroy();
        }
    }
    spriteMap.clear();
    for (auto &it : soundMap){
        if (it.second.get() && it.second.get()->buffer){
            alDeleteBuffers(1, &it.second.get()->buffer);
            SoundLoader::ShowError("erasing assets");
            it.second.reset(nullptr);
            it.second.destroy();
        }
    }
    soundMap.clear();
    return true;
}
