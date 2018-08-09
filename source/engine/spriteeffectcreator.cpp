#include "spriteeffectcreator.hpp"
#include "gamebase.hpp"
#include "text.hpp"

SpriteEffect* SpriteEffectCreator::CreateAnimatedText(int x,int y,float mx,float my,Text T,float duration){

    SpriteEffect *p = Game::GetCurrentState().SpriteEffectPool->AddInstance(SpriteEffect(x,y,T,duration));
    if (p != NULL){
       p->SetPatternMoveLine(Point(mx,my),Point(0,0));
    }
    return p;
}



SpriteEffect* SpriteEffectCreator::CreateAnimatedSprite(int x,int y,float mx,float my,std::string spritePath,int amount,float delay,int repeat){
    Sprite sp = Game::GetCurrentState().Assets.make<Sprite>(spritePath,amount,delay);
    SpriteEffect *p = Game::GetCurrentState().SpriteEffectPool->AddInstance(SpriteEffect(x,y,sp,delay*amount,repeat));
    if (p != NULL){
       p->SetPatternMoveLine(Point(mx,my),Point(0,0));
       p->Depth = 99999;
    }
    return p;
}
