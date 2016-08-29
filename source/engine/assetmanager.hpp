#pragma once
#include <memory>
#include <unordered_map>
#include "sprite.hpp"
#include "sound.hpp"
#include "../performance/console.hpp"
#include <iostream>


class AssetMannager{
    public:
        AssetMannager();
        ~AssetMannager();

        bool erase();

        TexturePtr makeTexture(std::string str,bool hasAntiAliasign=false);
        TexturePtr makeTexture(SDL_RWops* rw,std::string str,bool hasAntiAliasign=false);
        TexturePtr makeTexture(std::string fileName,ColorReplacer &r,bool HasAliasing=false);


        SoundPtr makeSound(std::string fileName);
        SoundPtr makeSound(SDL_RWops* rw,std::string str);



        template<typename ...Args> TexturePtr gen(Sprite s,std::string str,Args ...args){
            return makeTexture(str,args...);
        }

        template<typename ...Args> SoundPtr gen(Sound s,std::string str,Args ...args){
            return makeSound(str,args...);
        }

        template<class T,typename ...Args> T make(Args ...args){
            auto t = gen(T(),args...);
            if (t and t.get() and *t.get()){
                T sp(t,args...);
                sp.SetAssetManager(id);
                return sp;
            }
            return T();
        }

    private:
        static uint32_t ManagerId;
        uint32_t id;

        std::unordered_map<std::string, TexturePtr> spriteMap;
        std::unordered_map<std::string, SoundPtr> soundMap;


        static std::map<uint32_t,AssetMannager*> AssetMannagerMap;
};





class GlobalAssetManager: public AssetMannager{
    public:
        static GlobalAssetManager &GetInstance();
};

class UIAssetManager: public AssetMannager{
    public:
        static UIAssetManager &GetInstance();
};

