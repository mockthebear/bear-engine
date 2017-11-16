#include "../engine/object.hpp"
#include "luainterface.hpp"

#ifndef LUAOBJH
#define LUAOBJH
#ifndef __EMSCRIPTEN__


class LuaObject: public GameObject{
    public:

        ~LuaObject();
        LuaObject();
        LuaObject(int x,int y);
        bool canForceUpdate(){ return forceUpdate;};
        bool canForceRender(){ return forceRender;};
        int hasPerspective(){return perspective;};
        void Destroy();
        void Kill(){Active=false;};
        void Update(float dt);
        void Render();
        bool IsDead();
        void NotifyCollision(GameObject *p);
        bool Is(int i);
        void NotifyDamage(GameObject *bj,int n);
        void NotifyInPool(void*);

        REGISTER_GETSETTER(X,float,box.x);
        REGISTER_GETSETTER(Y,float,box.y);
        REGISTER_GETSETTER(Width,float,box.w);
        REGISTER_GETSETTER(Height,float,box.h);
        REGISTER_GETSETTER(Box,Rect,box);

        float MoveX(float var){ return (box.x += var); };
        float MoveY(float var){ return (box.x += var); };

        int GetPoolIndex(){return poolIndex;};
        uint64_t GetMyRef(){return (uint64_t)this;};
        GameObject *GetMyObj(){return this;};

    private:
        friend class LuaInterface;
        int perspective;
        bool Active,forceUpdate,forceRender;

};

#endif // __EMSCRIPTEN__

#endif //LuaObject


