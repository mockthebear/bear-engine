#include "particles.hpp"
#include "camera.hpp"
#include "renderhelp.hpp"
#include <iostream>
#define PARTICLE_DEPTH 10000
#define PI 3.1415926

Particle::Particle(){
    OBJ_REGISTER(Particle);
    Rotating = rotation = 0;
    box.x = 0;
    box.y = 0;
    box.w = 1;
    box.h = 1;

    Depth = PARTICLE_DEPTH;

    repeat = 0;

    PatternMove = MOVE_SET_NONE;
    Speed = Point(0,0);
    AlphaDegen = 0;


    currentFrame=Frame=0;
    currentDelay=Delay=0;
    HasSprite = false;
    Alpha = 255;
    Scale = 1;
    Scaling = 0;

}
Particle::Particle(int x,int y){
    OBJ_REGISTER(Particle);
    Rotating = rotation = 0;

    box.x = x;
    box.y = y;
    box.w = 1;
    box.h = 1;

    repeat = 0;

    currentFrame=Frame=0;
    currentDelay=Delay=0;
    HasSprite = false;
    Alpha = 255;
    AlphaDegen = 0;

    Depth = PARTICLE_DEPTH;
    PatternMove = MOVE_SET_NONE;
    Speed = Point(0,0);
    createdPosition = Point(x,y);
    Scale = 1;
    Scaling = 0;


}

Particle::Particle(int x,int y,Text txt_,float duration){
    OBJ_REGISTER(Particle);
    Rotating = rotation = 0;
    box.x = x;
    box.y = y;
    repeat = 0;
    currentFrame=0;
    currentDelay=duration;
    Frame = 1;
    Delay = duration;
    HasSprite = false;
    txt = txt_;
    Alpha = 255;
    Depth = PARTICLE_DEPTH;
    PatternMove = MOVE_SET_NONE;
    Speed = Point(0,0);
    createdPosition = Point(x,y);
    AlphaDegen = 0;
    Scale = 1;
    Scaling = 0;
}

Particle::Particle(int x,int y,Sprite sp_,float duration,int rep){
    OBJ_REGISTER(Particle);
    Rotating = rotation = 0;
    box.x = x;
    box.y = y;
    repeat = rep;
    currentFrame=0;
    AlphaDegen = 0;

    SetSprite(sp_,duration,rep);

    Alpha = 255;
    Depth = PARTICLE_DEPTH;
    PatternMove = MOVE_SET_NONE;
    Speed = Point(0,0);
    createdPosition = Point(x,y);
    Scale = 1;
    Scaling = 0;
}


Particle::~Particle(){

}



void Particle::SetSprite(Sprite sp_,float duration,int repeats){
    sp              =   sp_;
    HasSprite       =   true;
    repeat          =   repeats;
    currentFrame    =   0;
    Frame           =   sp.GetFrameCount();
    currentDelay    =   std::max(sp_.GetFrameTime(),duration);
    Delay           =   std::max(sp_.GetFrameTime(),duration);
}

void Particle::SetPatternMoveLine(Point p,Point accel){
    Speed = p;
    Acceleration = accel;
    PatternMove = MOVE_SET_LINE;
}

void Particle::SetPatternMoveLineFriction(Point p,Point accel){
    Speed = p;
    Acceleration = accel;
    PatternMove = MOVE_SET_FRICTION;
}

void Particle::SetPatternMoveCircle(float speed,float angle,float radius){
    Acceleration.x = speed;
    Acceleration.y = angle;
    internalFloat = radius;
    PatternMove = MOVE_SET_CIRCLE;
}

void Particle::Update(float dt){

    if (HasSprite)
        sp.Update(dt);
    if (AlphaDegen != 0){
        Alpha -= AlphaDegen*dt;
        Alpha = std::max(0.0f,Alpha);
    }
    Scale += Scaling*dt;
    rotation += Rotating*dt;
    if (PatternMove == MOVE_SET_CUSTOM){
        customF(this,dt,internalFloat);
    }else if(PatternMove == MOVE_SET_LINE){
        MovementSet::Line(box,dt, Speed, Acceleration );
    }else if(PatternMove == MOVE_SET_FRICTION){
        MovementSet::Friction(box,dt, Speed, Acceleration );
    }else if(PatternMove == MOVE_SET_CIRCLE){
        MovementSet::Circle( box,dt, Acceleration.x,Acceleration.y,internalFloat,createdPosition );
    }


    currentDelay -= dt;
    if (currentDelay <= 0){
        currentDelay = Delay;
        currentFrame++;
    }
    if (sp.IsAnimationOver() || currentFrame >= Frame){
        if (repeat > 0){
            sp.ResetAnimation();
            repeat--;
            currentFrame = 0;
        }else{
            currentFrame = Frame;
        }
    }
    if (IsDead() && txt.IsWorking()){
        txt.Close();
    }
}

void Particle::Render(){
    if (IsDead()){
        return;
    }
    if(HasSprite){
        sp.SetAlpha(Alpha);
        sp.SetScale(Point(Scale,Scale));
        //sp.SetCenter(Point(sp.GetWidth()/Frame,sp.GetHeight()/Frame));
        sp.Render(box.x-Camera::pos.x,box.y-Camera::pos.y,rotation);
        sp.SetAlpha(255);
    }else if(txt.IsWorking()){
        txt.SetRotation(rotation);
        //txt.SetScale(Point(Scale,Scale));
        txt.SetAlpha(Alpha);

        txt.Render(Camera::AdjustPosition(box));
        txt.SetAlpha(255);
    }

}
bool Particle::IsDead(){
    return currentFrame >= Frame || (Alpha <= 0 || Alpha > 255);
}

bool Particle::Is(int t){
    return t == OBJ_PARTICLE;
}
