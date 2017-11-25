#include "parallelcollisionmanager.hpp"
#include "../framework/threadpool.hpp"
#include "collision.hpp"
#include "gamebase.hpp"

ParallelCollisionManager g_parallelCollision = ParallelCollisionManager();


ParallelCollisionManager::ParallelCollisionManager(){
    StoredAllocCount = 0;
    Reset();
    for (int i=0;i<CB_processPerModule;i++){
    }
}

void ParallelCollisionManager::Reset(){
    for (uint32_t i=0;i<CB_processPerModule;i++){
        if (i < StoredAllocCount){
            content[i] = stored[i];
        }else{
            content[i].id       = i;
            content[i].type     = COLLISION_OBJ_NONE;
            content[i].status   = COLLISION_STATUS_NOT_READY;
            content[i].onlySolid= true;
        }
    }
    allocCount = StoredAllocCount;
}


bool ParallelCollisionManager::MakeJobs(){
    if (allocCount == 0){
        return false;
    }
    #ifndef DISABLE_THREADPOOL
    ThreadPool::GetInstance().AddLambdaJob([=](int,int,void* parId){

        StoredAllocCount = 0;
        for (uint32_t id=0;id<allocCount;id++){
            content[id].status = COLLISION_STATUS_NOT_COLLIDING;
        }
        for (uint32_t id=0;id<allocCount;id++){
            checkCB(id);
        }
    });
    #else
        StoredAllocCount = 0;
        for (uint32_t id=0;id<allocCount;id++){
            content[id].status = COLLISION_STATUS_NOT_COLLIDING;
        }
        for (uint32_t id=0;id<allocCount;id++){
            checkCB(id);
        }
    #endif
    return true;
}


uint32_t ParallelCollisionManager::requestCollision(Rect &box,PoolGroupId group,bool onlyCol,GameObject *thisUnit){
    stored[StoredAllocCount] = CollisionThing(box,onlyCol,group);
    stored[StoredAllocCount].obj = thisUnit;
    StoredAllocCount++;
    return StoredAllocCount-1;
}
CollisionThing ParallelCollisionManager::getInfo(uint32_t id){
    return content[id];
}


CollisionStatus ParallelCollisionManager::isColliding(uint32_t id){
    return content[id].status;
}

void ParallelCollisionManager::checkCB(uint32_t id){
    PoolManager &pool = BearEngine->GetCurrentState().Pool;
    if (content[id].group != -1){
        for (auto &obj : pool.ForGroup(content[id].group)){
            if (obj && obj != content[id].obj && !obj->IsDead()){
                if ( 1 ){
                    if (    (content[id].type == COLLISION_OBJ_RECT && Collision::IsColliding(obj->box,Rect(content[id].objCB)))
                        ||  (content[id].type == COLLISION_OBJ_CIRCLE && Collision::IsColliding(obj->box,Circle(content[id].objCB[0],content[id].objCB[1],content[id].objCB[2])))
                        ||  (content[id].type == COLLISION_OBJ_POINT && Collision::IsColliding(obj->box,Point(content[id].objCB[0],content[id].objCB[1])))
                        ){
                            content[id].status = COLLISION_STATUS_COLLIDING;
                            content[id].obj = obj;
                            return;
                    }
                }
            }
        }
    }
}
