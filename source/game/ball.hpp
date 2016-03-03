#ifndef BALLOBJH
#define BALLOBJH
#include "../engine/object.hpp"
#include "../engine/sprite.hpp"

class Ball: public GameObject{
    public:
        ~Ball();
        Ball();
        Ball(Point pos);
        void Render();
        bool IsDead();
        void Update(float);
        bool IsInScreen(Point& p){return true;};
        bool IsInScreen(){return true;};
        void NotifyCollision(GameObject *p);
        bool Is(int);
        void NotifyDamage(GameObject *bj,int n);
    private:
        bool Created;
        Point speed;
        Sprite sp,sp2;

};

#endif // BALLOBJH


