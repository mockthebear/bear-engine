#include "particlecreator.hpp"
#include "gamebase.hpp"
#include "text.hpp"


Particle* ParticleCreator::CreateCustomParticle(int x,int y,float mx,float my,SmartTexture **tex,int amount,float delay,int repeat){
    Particle *p = Game::GetCurrentState().ParticlePool->AddInstance(Particle(x,y,tex,amount,delay,repeat));
    if (p != NULL){
       p->SetAlphaDegen(0.5,255);
       p->SetPatternMoveLine(Point(mx,my),Point(0,0));
    }
    return p;
}


Particle* ParticleCreator::CreateAnimatedText(int x,int y,float mx,float my,Text T,float duration){

    Particle *p = Game::GetCurrentState().ParticlePool->AddInstance(Particle(x,y,T,duration));
    if (p != NULL){
       p->SetPatternMoveLine(Point(mx,my),Point(0,0));
    }
    return p;
}



Particle* ParticleCreator::CreateAnimatedSprite(int x,int y,float mx,float my,std::string spritePath,int amount,float delay,int repeat){
    Sprite sp = Game::GetCurrentState().Assets.make<Sprite>(spritePath,amount,delay);
    Particle *p = Game::GetCurrentState().ParticlePool->AddInstance(Particle(x,y,sp,repeat));
    if (p != NULL){
       p->SetPatternMoveLine(Point(mx,my),Point(0,0));
       p->Depth = 99999;
    }
    return p;
}
