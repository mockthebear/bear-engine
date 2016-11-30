#pragma once
#include <memory>
#include <unordered_map>
#include "sprite.hpp"
#include "../sound/sound.hpp"
#include "music.hpp"
#include "../performance/console.hpp"
#include <iostream>


class AssetMannager{
    public:
        AssetMannager();
        ~AssetMannager();

        bool erase();




        TexturePtr makeTexture(bool forced,std::string str,int fcount=1,float ftime=0,int rep=1,bool hasAliasing=false);
        TexturePtr makeTexture(bool forced,SDL_RWops* rw,std::string str,bool hasAntiAliasign=false);
        TexturePtr makeTexture(bool forced,std::string fileName,ColorReplacer &r,bool HasAliasing=false);


        SoundPtr makeSound(bool forced,std::string fileName);
        SoundPtr makeSound(bool forced,SDL_RWops* rw,std::string str);

        std::vector<std::string> getAssetNamesInternal(Sprite t){
            std::vector<std::string> stuff;
            for (auto &it : spriteMap){
                stuff.emplace_back(it.first);
            }
            return stuff;
        }


        template<typename T> std::vector<std::string> getAssetNames(){
            T empt;
            std::vector<std::string> assetList = getAssetNamesInternal(empt);
            return assetList;
        }





        template<typename ...Args> TexturePtr gen(bool forced,Sprite s,std::string str,Args ...args){
            return makeTexture(forced,str,args...);
        }

       template<typename ...Args> SoundPtr gen(bool forced,Sound s,std::string str,Args ...args){
            return makeSound(forced,str,args...);
        }

        template<class T,typename ...Args> T reload(Args ...args){
            auto t = gen(true,T(),args...);
            if (t.get()){
                T sp(t,args...);
                sp.SetAssetManager(id);
                return sp;
            }
            return T();
        }

        template<class T,typename ...Args> T make(Args ...args){
            auto t = gen(false,T(),args...);
            if (t.get()){
                T sp(t,args...);
                sp.SetAssetManager(id);
                return sp;
            }
            return T();
        }

    private:
        bool setOutput;
        static uint32_t ManagerId;
        uint32_t id;

        std::unordered_map<std::string, TexturePtr> spriteMap;
        std::unordered_map<std::string, SoundPtr> soundMap;
        //std::unordered_map<std::string, MusicPtr> musicMap;


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

