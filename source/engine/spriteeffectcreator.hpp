#ifndef SpriteEffect_CREATOR
#define SpriteEffect_CREATOR

#include "spriteeffect.hpp"
#include "text.hpp"


class SpriteEffectCreator{
    public:
        static SpriteEffect* CreateAnimatedText(int x,int y,float mx,float my,Text T,float duration);
        static SpriteEffect* CreateAnimatedSprite(int x,int y,float mx,float my,std::string spritePath,int amount,float delay,int repeat=0);
};
#endif // SpriteEffect_CREATOR_GM
