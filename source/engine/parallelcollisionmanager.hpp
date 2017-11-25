#pragma once
#include "../framework/poolmanager.hpp"
#define CB_processPerModule 100

enum CollisionObjType{
    COLLISION_OBJ_NONE,
    COLLISION_OBJ_POINT,
    COLLISION_OBJ_CIRCLE,
    COLLISION_OBJ_RECT,
};

enum CollisionStatus{
    COLLISION_STATUS_NOT_READY = -1,
    COLLISION_STATUS_NOT_COLLIDING = 0,
    COLLISION_STATUS_COLLIDING = 1,
};
class CollisionThing{
    public:
        CollisionThing(Point &bx,bool os,PoolGroupId gid):CollisionThing(){
            onlySolid = os;
            objCB[0] = bx.x;
            objCB[1] = bx.y;
            type = COLLISION_OBJ_POINT;
            group = gid;
        };
        CollisionThing(Circle &bx,bool os,PoolGroupId gid):CollisionThing(){
            onlySolid = os;
            objCB[0] = bx.x;
            objCB[1] = bx.y;
            objCB[2] = bx.r;
            type = COLLISION_OBJ_CIRCLE;
            group = gid;
        };
        CollisionThing(Rect &bx,bool os,PoolGroupId gid):CollisionThing(){
            onlySolid = os;
            objCB[0] = bx.x;
            objCB[1] = bx.y;
            objCB[2] = bx.w;
            objCB[3] = bx.h;
            type = COLLISION_OBJ_RECT;
            group = gid;
        };
        CollisionThing(){
            type = COLLISION_OBJ_NONE;
            status = COLLISION_STATUS_NOT_READY;
            id = 0;
            onlySolid = true;
            obj = nullptr;
            group = -1;
        };
        GameObject *obj;
        PoolGroupId group;
        CollisionStatus status;
        CollisionObjType type;
        uint64_t id;
        float objCB[4];
        bool onlySolid;
};

class ParallelCollisionManager{
    public:
        ParallelCollisionManager();


        void Reset();
        bool MakeJobs();

        uint32_t requestCollision(Rect &box,PoolGroupId group,bool onlySolid = false,GameObject *thisUnit=nullptr);
        CollisionStatus isColliding(uint32_t id);
        CollisionThing getInfo(uint32_t id);

    private:
        uint32_t allocCount;
        uint32_t StoredAllocCount;
        void checkCB(uint32_t id);
        CollisionThing content[CB_processPerModule];
        CollisionThing stored[CB_processPerModule];
};

extern ParallelCollisionManager g_parallelCollision;
