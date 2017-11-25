#ifndef SHOOTEFFECTH
#define SHOOTEFFECTH

#include "object.hpp"
#include "sprite.hpp"
#include "timer.hpp"
#include "camera.hpp"


class ShootEffect: public GameObject{
    public:
        ShootEffect(GameObject *o,Point p2,Sprite *sp,float timeLimit,bool ends);
        ~ShootEffect();
        void Update(float);
        void Render();
        bool IsDead();
        void Shoot(float x, float y);
        void NotifyCollision(GameObject *p){};
        void NotifyDamage(GameObject *p,int n){};
        bool Is(int);
    private:
        int OH,OW;
        GameObject *Follow;
        Timer endTime;
        float timeLimit,distance;
        bool oneTimeOnly;
        Sprite *sp;
        Point p2,p;
        float leang;

};

#endif // SHOOTEFFECTH



