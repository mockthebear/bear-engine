#include "../engine/object.hpp"
#include "luainterface.hpp"

#ifndef LUAOBJH
#define LUAOBJH


class LuaObject: public GameObject{
    public:
        void Update(float dt);
        ~LuaObject();
        LuaObject();
        void Render();
        void Start();
        void Stop();
        bool IsDead();
        bool IsInScreen(Point& p);
        bool IsInScreen();
        void NotifyCollision(GameObject *p);
        bool Is(int i);
        void NotifyDamage(GameObject *bj,int n);
    private:
        bool Active;
};

#endif //LuaObject


