#ifndef PARTICLEMANAHGERH
#define PARTICLEMANAHGERH

#include "smarttexture.hpp"
#include <map>

class SmartTextureManager{
    public:
        SmartTextureManager();
        static SmartTextureManager& GetInstance();
        SmartTexture **Alloc(int n,int &position);
        SmartTexture **Alloc(int n);
        SmartTexture **Get(int position);

    private:
        std::map<int,SmartTexture **> Textures;
        std::map<int,int> Sizes;
        int Amount;

};
#endif // PARTICLEMANAHGERH
