#pragma once
#include "../../framework/geometry.hpp"


class Sprite;

class Animation{
    public:

        Animation():Loops(0),RepeatTimes(0),sprX(0),sprY(0),sprW(0),sprH(0),MaxFrames(1),SprDelay(1.0f),SprMaxDelay(1.0f),CanRepeat(true),LastFrame(0),LastRepeatCount(0),
        pause(false),finishedAnimation(false),finishedSingleFrame(false),isFormated(false),LockedFinished(true){};
        Animation(float w,float h):Animation(){
           SetGridSize(w,h);
           ResetAnimation();
        };
        void SetGridSize(float w,float h){
           sprW = w;
           sprH = h;
        };
        void Update(float dt);
        void Pause(bool p){
            pause = p;
        }
        bool IsAnimationEnd(){
            return finishedAnimation;
        }
        bool IsFrameEnd(){
            return finishedSingleFrame;
        }
        void SetAnimation(int y=0,int maxFrames=0,float timer=-1);
        void ResetAnimation();
        void SetAnimationTime(float time);
        void FormatSprite(Sprite& sp);
        void RenderL(float x,float y,Sprite sp);
        void Render(float x,float y,Sprite& sp);
        void SetFrame(int x,int y=-1);
        PointInt GetFrame();
        void SetFrameCount(int fc);
        int GetFrameCount();
        void SetFrameTime(float ft);
        float GetFrameTime();
        void SetRepeatTimes(int t);
        int GetRepeatTimes();

        uint32_t Loops;
        int32_t RepeatTimes;
        uint32_t sprX;
        uint32_t sprY;
        uint32_t sprW;
        uint32_t sprH;
        uint32_t MaxFrames;
        float SprDelay;
        float SprMaxDelay;
        bool CanRepeat;
        uint32_t LastFrame;
        uint32_t LastRepeatCount;
    private:
        friend class Sprite;
        friend class AnimatedSprite;
        bool pause;
        bool finishedAnimation;
        bool finishedSingleFrame;
        bool isFormated;
        bool LockedFinished;
};

