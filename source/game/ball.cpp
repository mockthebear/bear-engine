#include "ball.hpp"
#include "../input/inputmanager.hpp"
#include "../engine/camera.hpp"
#include "../engine/light.hpp"

Ball::Ball(){
    OBJ_REGISTER(Ball);
    Created = false;
}
Ball::Ball(Point pos){
    OBJ_REGISTER(Ball);
    box = pos;
    box.w = 32;
    box.h = 32;
    Created = true;
    speed.y = -0.1;
    sp = Sprite("assets:ibg.png");
}
Ball::~Ball(){

}
void Ball::Render(){
    sp.Render(Camera::AdjustPosition(box));
}
bool Ball::IsDead(){
    return !Created;
}
void Ball::Update(float dt){
    Point3 accel = InputManager::GetInstance().GetAccelerometerForces();
    speed.x += accel.x * 1.50 * dt;
    speed.y += accel.y * 1.50 * dt;

    Rect aux = box;
    //Out of bounds
    if (aux.x < 0 or aux.x > 800 - box.w){
        speed.x *= -0.8;
    }
    if (aux.y < 0 or aux.y > 600 - box.h){
        speed.y *= -0.8;
    }

    box.x += speed.x;
    box.y += speed.y;
    speed.x -= speed.x*0.1*dt;
    speed.y -= speed.y*0.1*dt;


    if (box.y < -2){
        box.y = 0;

    }
    if (box.x < -2){
        box.x = 0;


    }
    if (box.x > 802 - box.w){
        box.x = 800 - box.w;
    }
    if (box.y > 602 - box.h){
        box.y = 600 - box.h;
    }
    Light::GetInstance()->AddLightM(box.x+16,box.y+16,255);

}
void Ball::NotifyCollision(GameObject *p){
    speed.x *= -1;
    speed.y *= -1;
}
bool Ball::Is(int hasharg){
    return hasharg == GetHash();
}
void Ball::NotifyDamage(GameObject *bj,int n){

}
