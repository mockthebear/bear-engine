#ifndef CONTROLABEOBJECTH
#define CONTROLABEOBJECTH
#include "../engine/object.hpp"
#include "../engine/sprite.hpp"

class ControlableObject: public GameObject{
    public:
        ~ControlableObject();
        ControlableObject();
        ControlableObject(int x,int y);
        void Render();
        bool IsDead();
        void Update(float);
        bool IsInScreen(Point& p){return true;};
        bool IsInScreen(){return true;};
        void NotifyCollision(GameObject *p);
        bool Is(int);
        void NotifyDamage(GameObject *bj,int n);
        bool isOnTop(){return false;};
        int hasPerspective(){return 0;};
    private:
        float wobble;
        Sprite sp;
        bool isCreated;
        Point speed;
};

#endif // CONTROLABEOBJECTH




