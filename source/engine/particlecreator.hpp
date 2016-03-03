#ifndef PARTICLE_CREATOR
#define PARTICLE_CREATOR

#include "particles.hpp"
#include "smarttexture.hpp"
#include "text.hpp"


class ParticleCreator{
    public:
        static Particle* CreateCustomParticle(int x,int y,float mx,float my,SmartTexture **tex,int amount,float delay,int repeat=0);
        static Particle* CreateAnimatedText(int x,int y,float mx,float my,Text T,float duration);
        static Particle* CreateAnimatedSprite(int x,int y,float mx,float my,std::string spritePath,int amount,float delay,int repeat=0);
};
#endif // PARTICLE_CREATOR_GM
