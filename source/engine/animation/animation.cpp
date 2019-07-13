#include "animation.hpp"
#include "../sprite.hpp"


void Animation::Update(float dt){
    if (pause){
        return;
    }
    if (LockedFinished)
        finishedAnimation = false;
    finishedSingleFrame = false;
    SprDelay -= dt;
    if (SprDelay <= 0){
        isFormated = false;
        finishedSingleFrame = true;
        LastFrame = sprX;
        sprX++;
        SprDelay = SprMaxDelay;
        if (sprX >= MaxFrames){
            if (CanRepeat){
                Loops++;
                sprX = 0;
                RepeatTimes--;
            }else{
                sprX--;
                LockedFinished = false;
                finishedSingleFrame = false;
            }
            if (RepeatTimes <= 0)
                finishedAnimation = true;
        }
    }
}

void Animation::SetFrame(int x,int y){
    if (y != -1){
        sprY = y;
    }
    sprX = x;
    if (sprX >= MaxFrames){
        sprX = 0;
    }
    isFormated = false;
}

PointInt Animation::GetFrame(){
    return PointInt(sprX, sprY);
}


void Animation::SetFrameCount(int fc){
    MaxFrames     =   std::max(1, fc);
    finishedAnimation =   false;
};

int Animation::GetFrameCount(){
    return MaxFrames;
};

void Animation::SetFrameTime(float ft){
    SprMaxDelay = ft;
    SprDelay = 0;
};

float Animation::GetFrameTime(){
    return SprMaxDelay;
};

void Animation::SetRepeatTimes(int t){
    LastRepeatCount = RepeatTimes = t;
}

int Animation::GetRepeatTimes(){
    return RepeatTimes;
}



void Animation::SetAnimation(int y,int maxFrames,float timer){
    if (timer >= 0){
        SetAnimationTime(timer);
    }
    isFormated = false;
    sprY = y;
    MaxFrames = maxFrames;
    ResetAnimation();
}

void Animation::ResetAnimation(){
    LockedFinished = true;
    sprX = 0;
    SprDelay = SprMaxDelay;
    finishedAnimation = false;
    finishedSingleFrame = false;
    isFormated = false;
    Loops = 0;
    RepeatTimes = LastRepeatCount;
}

void Animation::SetAnimationTime(float time){
    SprDelay = SprMaxDelay = time;
}

void Animation::FormatSprite(Sprite& sp){
    sp.SetClip(sprX * sprW, sprY * sprH,sprW,sprH);
    isFormated = true;
}

void Animation::RenderL(float x,float y,Sprite sp){
    if (!isFormated){
        FormatSprite(sp);
    }
    sp.Render(PointInt(x,y));
    isFormated = false;
}

void Animation::Render(float x,float y,Sprite& sp){
    if (!isFormated){
        FormatSprite(sp);
    }
    sp.Render(PointInt(x,y));
    isFormated = false;
}
