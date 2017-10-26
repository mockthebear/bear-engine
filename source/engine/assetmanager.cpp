#include "assetmanager.hpp"
#include "../settings/definitions.hpp"
#include "../performance/console.hpp"
#include "../sound/soundloader.hpp"
#include <iostream>


uint32_t AssetMannager::ManagerId = 0;
std::map<uint32_t,AssetMannager*> AssetMannager::AssetMannagerMap;


GlobalAssetManager &GlobalAssetManager::GetInstance(){
    static GlobalAssetManager mngr;
    return mngr ;
}
UIAssetManager &UIAssetManager::GetInstance(){
    static UIAssetManager mngr;
    return mngr;
}

AssetMannager::AssetMannager(){
    ManagerId++;
    id = ManagerId;
    AssetMannagerMap[id] = this;
    setOutput = false;
}



AssetMannager::~AssetMannager(){

}


TexturePtr AssetMannager::makeTexture(bool forced,std::string str,std::string alias,TextureLoadMethod hasAliasing) {
    if (alias == ""){
        alias = str;
    }
    if (!spriteMap[alias] || forced){
        BearTexture* c =Sprite::Preload((char*)str.c_str(),true,hasAliasing);
        if (!c){
            return TexturePtr();
        }
        if (forced && spriteMap[alias].get()){
            #ifdef RENDER_OPENGL
            BearTexture *be = spriteMap[alias].get();
            glDeleteTextures( 1, &be->id );
            delete be;
            #else
            SDL_DestroyTexture( spriteMap[alias].get() );
            #endif // RENDER_OPENGL
            spriteMap[alias].reset(c);
            if (setOutput)
                Console::GetInstance().AddTextInfoF("FORCED: Made texture for [%s] in  manager %d",str,id);
        }else{
            spriteMap[alias] = chain_ptr<BearTexture>::make(c);
            if (setOutput)
                Console::GetInstance().AddTextInfoF("Made texture for [%s] in  manager %d",str,id);
        }
        return spriteMap[alias];
    }
    return spriteMap[alias];
}

TexturePtr AssetMannager::makeTexture(bool forced,std::string str,TextureLoadMethod hasAliasing) {
    return makeTexture(forced,str,1,0,1,hasAliasing);
};

TexturePtr AssetMannager::makeTexture(bool forced,std::string str,int ,float,int,TextureLoadMethod hasAliasing) {
    if (!spriteMap[str] || forced){
        BearTexture* c =Sprite::Preload((char*)str.c_str(),true,hasAliasing);
        if (!c){
            return TexturePtr();
        }
        if (forced && spriteMap[str].get()){
            #ifdef RENDER_OPENGL
            BearTexture *be = spriteMap[str].get();
            glDeleteTextures( 1, &be->id );
            delete be;
            #else
            SDL_DestroyTexture( spriteMap[str].get() );
            #endif // RENDER_OPENGL
            spriteMap[str].reset(c);
            if (setOutput)
                Console::GetInstance().AddTextInfoF("FORCED: Made texture for [%s] in  manager %d",str,id);
        }else{
            spriteMap[str] = chain_ptr<BearTexture>::make(c);
            if (setOutput)
                Console::GetInstance().AddTextInfoF("Made texture for [%s] in  manager %d",str,id);
        }
        return spriteMap[str];
    }
    return spriteMap[str];
}

TexturePtr AssetMannager::makeTexture(bool forced,SDL_RWops* rw,std::string str,TextureLoadMethod hasAliasing)  {
    if (!spriteMap[str] || forced){
        if (!rw){
            return TexturePtr();
        }
        BearTexture* c =Sprite::Preload(rw,str,hasAliasing);
        if (!c){
            Console::GetInstance().AddTextInfoF("Failed to create. %s in %d",str,id);
            return TexturePtr();
        }

        if (forced && spriteMap[str].get()){
            #ifdef RENDER_OPENGL
            BearTexture *be = spriteMap[str].get();
            glDeleteTextures( 1, &be->id );
            delete be;
            #else
            SDL_DestroyTexture( spriteMap[str].get() );
            #endif // RENDER_OPENGL
            spriteMap[str].reset(c);
            if (setOutput)
                Console::GetInstance().AddTextInfoF("FORCED: Made texture RW for [%s] in  manager %d",str,id);
        }else{
            spriteMap[str] = chain_ptr<BearTexture>::make(c);
            if (setOutput)
                Console::GetInstance().AddTextInfoF("Made texture RW for [%s] in  manager %d",str,id);
        }
        return spriteMap[str];
    }

    return spriteMap[str];
}

TexturePtr AssetMannager::makeTexture(bool forced,std::string str,std::string alias,ColorReplacer &r,TextureLoadMethod hasAliasing)  {
    if (alias == ""){
        alias = str;
    }
    if (!spriteMap[alias] || forced){
        BearTexture* c =Sprite::Preload(str,r,hasAliasing);
        if (!c){
            return TexturePtr();
        }

        if (forced && spriteMap[alias].get()){
            #ifdef RENDER_OPENGL
            BearTexture *be = spriteMap[alias].get();
            glDeleteTextures( 1, &be->id );
            delete be;
            #else
            SDL_DestroyTexture( spriteMap[alias].get() );
            #endif // RENDER_OPENGL
            spriteMap[alias].reset(c);
            if (setOutput)
                Console::GetInstance().AddTextInfoF("FORCED: Made replace texture for [%s] in  manager %d",str,id);
        }else{
            spriteMap[alias] = chain_ptr<BearTexture>::make(c);
            if (setOutput)
                Console::GetInstance().AddTextInfoF("Made replace texture for [%s] in  manager %d",str,id);
        }
        return spriteMap[alias];
    }

    return spriteMap[alias];
}

TexturePtr AssetMannager::makeTexture(bool forced,std::string str,ColorReplacer &r,TextureLoadMethod hasAliasing)  {
    if (!spriteMap[str] || forced){
        BearTexture* c =Sprite::Preload(str,r,hasAliasing);
        if (!c){
            return TexturePtr();
        }

        if (forced && spriteMap[str].get()){
            #ifdef RENDER_OPENGL
            BearTexture *be = spriteMap[str].get();
            glDeleteTextures( 1, &be->id );
            delete be;
            #else
            SDL_DestroyTexture( spriteMap[str].get() );
            #endif // RENDER_OPENGL
            spriteMap[str].reset(c);
            if (setOutput)
                Console::GetInstance().AddTextInfoF("FORCED: Made replace texture for [%s] in  manager %d",str,id);
        }else{
            spriteMap[str] = chain_ptr<BearTexture>::make(c);
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
            #ifdef RENDER_OPENGL
            BearTexture *be = it.second.get();
            glDeleteTextures( 1, &be->id );
            delete be;
            #else
            SDL_DestroyTexture( it.second.get() );
            #endif // RENDER_OPENGL
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
