#include "smarttexturemanager.hpp"

SmartTextureManager::SmartTextureManager(){
    Amount = 0;
}

SmartTextureManager& SmartTextureManager::GetInstance(){
    static SmartTextureManager Singleton;
    return Singleton;
}
SmartTexture **SmartTextureManager::Alloc(int n,int &position){
    SmartTexture ** ret = Alloc(n);
    position = Amount-1;
    return ret;
}
SmartTexture **SmartTextureManager::Alloc(int n){
    Textures[Amount]    = new SmartTexture *[n];
    Sizes[Amount]       = n;
    Amount++;
    return Textures[Amount-1];
}

SmartTexture **SmartTextureManager::Get(int position){
    return Textures[position];
}
