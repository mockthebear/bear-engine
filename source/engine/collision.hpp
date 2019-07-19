#pragma once
#include <algorithm>
#include <ctgmath>
#include "object.hpp"
#include "../framework/poolmanager.hpp"


#include "primitivecollision.hpp"



namespace Collision {

    std::vector<GameObject*> GetNearObjects(GameObject* dis,PoolManager &pool,PoolGroupId gid,float scale=4,bool onlySolid=true);


    bool WarpAway(Rect &obj1,Rect obj2);
    bool SoftWarpAway(GameObject* thisObject,GameObject* otherObject);
    bool SoftWarpAway(GameObject* thisObject,GameObject* otherObject,Point speed);


    int AdjustCollision(float &sx,float &sy,float dt,GameObject* dis,PoolManager &pool,PoolGroupId gid=-1,bool onlySolid = true);
    int AdjustCollisionIndependent(float &sx,float &sy,float dt,GameObject* dis,PoolManager &pool,PoolGroupId gid=-1,float msize=0.5,bool onlySolid = true);
    bool CheckCollision(Rect tempXY,GameObject* dis,PoolManager &pool,PoolGroupId gid=-1,bool onlySolid = true);

    int AdjustCollision(float &sx,float &sy,float dt,GameObject* dis,std::vector<GameObject*> vec,bool onlySolid = true);
    int AdjustCollisionIndependent(float &sx,float &sy,float dt,GameObject* dis,std::vector<GameObject*> vec,float msize=0.5,bool onlySolid = true);
    bool CheckCollision(Rect tempXY,GameObject* dis,std::vector<GameObject*> vec,bool onlySolid = true);




    Rect GetIntersectRect(Rect& a,  Rect& b);
    std::vector<Rect> CheckCollision(std::vector<Rect> &rectArr,GameObject* dis,PoolManager &pool,PoolGroupId gid,bool onlySolid=true);

    GameObject* GetCollidingObject(GameObject* thisObject,PoolManager &pool,PoolGroupId gid=-1,bool onlySolid = true);
    std::vector<GameObject*> GetCollidingObjects(GameObject* thisObject,PoolManager &pool,PoolGroupId gid=-1,bool onlySolid = true);


};
