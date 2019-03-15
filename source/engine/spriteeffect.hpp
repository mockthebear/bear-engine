#ifndef SpriteEffectS_BE
#define SpriteEffectS_BE
#include "object.hpp"
#include "sprite.hpp"
#include "text.hpp"

namespace MovementSet{
    static inline void Line( Rect &box,float dt, Point &speed, Point &Acceleration ){
        speed.x += Acceleration.x;
        speed.y += Acceleration.y;
        box.x += speed.x*dt;
        box.y += speed.y*dt;
    };
    static inline void Circle( Rect &box,float dt, float speed,float &angle, float radius,Point createdPosition ){
        angle += speed * dt;
        box.x = createdPosition.x + radius*cos(angle);
        box.y = createdPosition.y + radius*sin(angle);
    };
    static inline void Friction( Rect &box,float dt, Point &speed, Point &Acceleration ){
        Point aux = speed;
        speed.x += Acceleration.x;
        speed.y += Acceleration.y;
        /*
            todo:

            wtf is this?
        */
        if ((aux.x/aux.x) != (speed.x/speed.x)){
            Acceleration.x = 0;
            speed.x = 0;
        }
        if ((aux.y/aux.y) != (speed.y/speed.y)){
            Acceleration.y = 0;
            speed.y = 0;
        }
        box.x += speed.x*dt;
        box.y += speed.y*dt;
    };
};

enum MovementPattern{
    MOVE_SET_NONE,
    MOVE_SET_LINE,
    MOVE_SET_CIRCLE,
    MOVE_SET_CUSTOM,
    MOVE_SET_FRICTION,
};
/**
    @brief SpriteEffects, Non-Object animations, Effect Text, and others
    *
    * Unless you're trying to make something complex or trying to understand
    * how these class works, you may check SpriteEffectCreator.
*/
class SpriteEffect: public GameObject{
    public:
        /**
            Create the most simple SpriteEffect. Just empty things
        */
        SpriteEffect(int x,int y);
        /**
            *Create an animated text effect
            @param x The x position
            @param y The y position
            @param txt an Text
            @param the duration
        */
        SpriteEffect(int x,int y,Text txt,float duration);
        /**
            *Create an SpriteEffect with an SmartTexture vector. Each component must be an frame
            @param x The x position
            @param y The y position
            @param sp The sprite.
            @param frames The amount of frames.  To know how to make animated sprites, check the class Sprite
            @param delay of course is an animation, so you need the delay between the frames
            @param repeat the amount of times the animation will repeat
        */
        SpriteEffect(int x,int y,Sprite sp,float duration = 0,int repeat=0);


        /**
            *Empty constructor create an dead SpriteEffect
        */
        SpriteEffect();
        /**
            *BYEEE
        */
        ~SpriteEffect();

        /**
            You can force an sprite to be set. Should be used on
            SpriteEffect::SpriteEffect(int x,int y)
        */
        void SetSprite(Sprite sp,float duration=0,int repeat=0);
        /**
            Set the SpriteEffect to move in a line.
            @param speed in x and y
            @param acceleration in x and y
        */
        void SetPatternMoveLine(Point velocity,Point acceleration);
        /**
            Set the SpriteEffect to on an circle
            @param speed radial speed
            @param angle Angle that it begins
            @param the radius of the circle
        */
        void SetPatternMoveCircle(float speed,float angle,float radius);
        /**
            Custom movement set
            @param customP is an lambda function or function pointer
            void(SpriteEffect *dis,float dt,float &data)
                @param dis is the pointer to the current SpriteEffect
                @param dt is the delta time
                @param data is an reference to an internal float. you can use it to save one value
        */
        void SetPatternMoveCustom(std::function<void(SpriteEffect *dis,float dt,float &data)> customP){
            PatternMove = MOVE_SET_CUSTOM;
            customF = customP;
            internalFloat = 0;
        };
        void SetPatternMoveLineFriction(Point velocity,Point acceleration);
        /**
            Set rotation speed
        */
        void SetRotation(float r){Rotating = r;};
        void SetRotationModule(float r){rotation = r;};
        void SetDepth(int r){Depth = r;};
        void SetRepeat(int r){repeat = r;};
        /**
            Set the SpriteEffect to decrease alpha in the same rate at the end of duration
        */
        void SetAlphaAsDuration(){
            AlphaDegen = Alpha/(float)( (Delay)*(repeat+1)*Frame);
        }
        /**
            Set the SpriteEffect alpha to decrease at your own degen
            @param degen. The amount of alpha it will be lost per second
            @param currentAlpha The current alpha
        */
        void SetAlphaDegen(float degen,float currentAlpha=255){
            AlphaDegen = degen;
            Alpha = currentAlpha;
        }

        /*
            Obg general
        */
        /**
            Updater
        */
        void Update(float);
        /**
            The render function
        */
        void Render();

        void SetScaling(float scale){Scaling = scale;};
        void SetScale(float scale){Scale = scale;};


        /**
            The function used to know if is dead
        */
        bool IsDead();
        void Kill(){Frame=0;Alpha=0;};
        /**
         SpriteEffects dont take damage
        */
        void NotifyDamage(GameObject *,int){};
        /**
            Empty. There is no collision
        */
        void NotifyCollision(GameObject *){};
        /**
            Comparator
        */
        bool Is(int);
        /**
            Yep! always update
            @return always true
        */
        bool canForceUpdate(){return true;};
        /**
            You can edit the depth with SpriteEffect::Depth
            @return always true
        */
        int hasPerspective(){return Depth;};
        /**
            Saved position from where it was created
        */
        Point createdPosition;
        Sprite &GetSprite(){ return sp;};
        int Depth;
    private:
        float rotation;

        /*
            Rotation stuff
        */
        float Rotating;
        /*
            Life/duration
        */
        float Scaling;
        float Scale;
        int repeat;
        float Alpha,AlphaDegen;
        /*
            Movement
        */
        MovementPattern PatternMove;
        Point Speed,Acceleration;
        std::function<void(SpriteEffect *dis,float dt,float &data)> customF;
        /*
            Animation
        */
        int currentFrame,Frame;
        float currentDelay,Delay;
        /*
            Shoot effect
        */
        Point p2;
        float Distance;
        /*
            Resources
        */
        bool HasSprite;
        Sprite sp;
        Text txt;

        float internalFloat;



};

#endif // SpriteEffectS_BE
