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
    textures = NULL;
    Alpha = 255;
    follow = NULL;

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
    textures = NULL;
    Alpha = 255;
    AlphaDegen = 0;

    Depth = PARTICLE_DEPTH;
    follow = NULL;
    PatternMove = MOVE_SET_NONE;
    Speed = Point(0,0);
    createdPosition = Point(x,y);


}

Particle::Particle(int x,int y,SmartTexture **vect,int amount,float fameDelay,int rep){
    OBJ_REGISTER(Particle);
    Rotating = rotation = 0;

    box.x = x;
    box.y = y;

    repeat = rep;
    currentFrame=0;
    currentDelay=fameDelay;
    Frame = amount;
    Delay = fameDelay;
    HasSprite = false;
    textures = vect;

    Alpha = 255;
    AlphaDegen = 0;
    follow = NULL;

    Depth = PARTICLE_DEPTH;

    PatternMove = MOVE_SET_NONE;
    Speed = Point(0,0);
    createdPosition = Point(x,y);
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
    textures = NULL;
    follow = NULL;
    txt = txt_;
    Alpha = 255;
    Depth = PARTICLE_DEPTH;
    PatternMove = MOVE_SET_NONE;
    Speed = Point(0,0);
    createdPosition = Point(x,y);
    AlphaDegen = 0;
}

Particle::Particle(int x,int y,Sprite sp_,int frames,float delay,int rep){
    OBJ_REGISTER(Particle);
    Rotating = rotation = 0;
    box.x = x;
    box.y = y;
    repeat = rep;
    currentFrame=0;
    currentDelay=delay;
    Frame = frames;
    AlphaDegen = 0;
    Delay = delay;
    HasSprite = true;
    sp = sp_;
    follow = NULL;
    Alpha = 255;
    Depth = PARTICLE_DEPTH;
    textures = NULL;
    PatternMove = MOVE_SET_NONE;
    Speed = Point(0,0);
    createdPosition = Point(x,y);
}

Particle::Particle(GameObject *follow_,Point target,Sprite sp_,float duration){
    OBJ_REGISTER(Particle);
    Rotating = rotation = 0;
    AlphaDegen = 0;
    p2 = target;
    follow= follow_;
    repeat = 0;
    currentFrame=0;
    currentDelay=duration;
    Frame = 1;
    Delay = duration;

    HasSprite = true;
    sp = sp_;
    Alpha = 255;
    Depth = PARTICLE_DEPTH;
    textures = NULL;
    PatternMove = MOVE_SET_NONE;
    Speed = Point(0,0);

}

Particle::~Particle(){

}

void Particle::SetSprite(Sprite sp_){
    sp = sp_;
    HasSprite = true;
    currentFrame=0;
    Frame=sp.SetFrameCount();
    currentDelay=sp.SetFrameTime();
    Delay=delay;
    textures = NULL;
}


void Particle::SetSprite(std::string name,int frames,float delay){
    sp = Sprite(name.c_str(),frames,delay);
    HasSprite = true;
    currentFrame=0;
    Frame=frames;
    currentDelay=delay;
    Delay=delay;
    textures = NULL;
}
void Particle::SetPatternMoveLine(Point p,Point accel){
    Speed = p;
    Acceleration = accel;
    PatternMove = MOVE_SET_LINE;
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
    if (!follow){
        rotation += Rotating*dt;
        if (PatternMove == MOVE_SET_CUSTOM){
            customF(this,dt,internalFloat);
        }else if(PatternMove == MOVE_SET_LINE){
            MovementSet::Line(box,dt, Speed, Acceleration );
        }else if(PatternMove == MOVE_SET_CIRCLE){
            MovementSet::Circle( box,dt, Acceleration.x,Acceleration.y,internalFloat,createdPosition );
        }
    }else{
        Point p = Point(follow->box.getXCenter()+8,follow->box.getYCenter()+8);
        box.x = (p.x+p2.x)/2.0;
        box.y = (p.y+p2.y)/2.0;
        float angle = atan2(p.y-p2.y,p.x-p2.x);
        Distance = p.getDistance(&p2);
        angle = (angle > 0 ? angle : (2.0*PI + angle)) * 360.0 / (2.0*PI);
        rotation = angle;
    }

    currentDelay -= dt;
    if (currentDelay <= 0){
        currentDelay = Delay;
        currentFrame++;
        if (currentFrame >= Frame and repeat > 0){
            repeat--;
            currentFrame = 0;
        }
    }
    if (IsDead() and txt.IsWorking()){
        txt.Close();
    }
}

void Particle::Render(){

    if(HasSprite){
        sp.SetAlpha(Alpha);
        if (follow != NULL){
            sp.SetClip(0,0,std::min(std::max(Distance,(float)sp.GetWidth() ),Distance ),sp.GetHeight() );
            //sp->SetClip(0,0,sp->GetHeight(),std::min(std::max(distance,(float)sp->GetWidth() ),distance ) );
            sp.Render(box.x-Camera::pos.x- Distance/2,box.y-Camera::pos.y- sp.GetHeight()/2,rotation);
        }else{
            sp.Render(Camera::AdjustPosition(box),rotation);
        }
    }else if(textures != NULL){
        if (currentFrame >= Frame)
            return;
        textures[currentFrame]->Render(Camera::AdjustPosition(box),rotation);
    }else if(txt.IsWorking()){
        txt.SetRotation(rotation);
        txt.SetAlpha(Alpha);
        txt.Render(Camera::AdjustPosition(box));
    }

}
bool Particle::IsDead(){
    return currentFrame >= Frame or (Alpha <= 0 or Alpha > 255);
}

bool Particle::Is(int t){
    return t == OBJ_PARTICLE;
}
