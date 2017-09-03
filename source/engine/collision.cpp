#include "collision.hpp"



int Collision::AdjustCollision(float &sx,float &sy,float dt,GameObject* dis,PoolManager &pool,PoolGroupId gid){
    Rect tempXY(dis->box.x+sx*dt,dis->box.y+sy*dt,dis->box.w,dis->box.h);
    if (sx != 0.0f || sy != 0.0f)
        for (auto &obj : pool.ForGroup(gid))
        {
            if (obj != dis && !obj->IsDead()){
                if (obj->solid ){
                    if (Collision::IsColliding(obj->box,tempXY) ){
                        sy = 0;
                        sx = 0;
                        /*tempXY = Rect(dis->box.x+sx*dt,dis->box.y+sy*dt,dis->box.w,dis->box.h);
                        if (Collision::IsColliding(obj->box,tempXY,0,0) ){
                            SoftWarpAway(dis,obj);
                        }*/
                        dis->NotifyCollision((GameObject*)obj);
                        return 4;
                    }
                    if (sy == 0.0f && sx == 0.0f)
                        break;
                }
            }
        }
        return -1;

}
std::vector<Rect> Collision::CheckCollision(std::vector<Rect> &rectArr,GameObject* dis,PoolManager &pool,PoolGroupId gid,bool onlySolid){
    std::vector<Rect> col;
    uint32_t i;
    for (auto &obj : pool.ForGroup(gid))
    {
        if (obj && obj != dis && !obj->IsDead()){
            if (!onlySolid ||obj->solid ){
                for (i=0;i<rectArr.size();i++){
                    if (Collision::IsColliding(obj->box,rectArr[i]) ){
                        col.emplace_back(rectArr[i]);
                    }
                }
            }
        }
    }
    return col;
}

bool Collision::CheckCollision(Rect tempXY,GameObject* dis,PoolManager &pool,PoolGroupId gid,bool onlySolid){
        for (auto &obj : pool.ForGroup(gid))
        {
        if (obj && obj != dis && !obj->IsDead()){
            if (!onlySolid ||obj->solid ){
                if (Collision::IsColliding(obj->box,tempXY) ){
                    return true;
                }
            }
        }
    }
    return false;
}

std::vector<GameObject*> Collision::GetCollidingObjects(GameObject* thisObject,PoolManager &pool,PoolGroupId gid,bool onlySolid){
    std::vector<GameObject*> data;
    for (auto &obj : pool.ForGroup(gid))
        {
        if (obj != thisObject && !obj->IsDead()){
            if (!onlySolid || obj->solid ){
                if (Collision::IsColliding(obj->box,thisObject->box) ){
                    data.emplace_back(obj);
                }
            }
        }
    }
    return data;
}

GameObject* Collision::GetCollidingObject(GameObject* thisObject,PoolManager &pool,PoolGroupId gid,bool onlySolid){
    for (auto &obj : pool.ForGroup(gid))
        {
        if (obj != thisObject && !obj->IsDead()){
            if (!onlySolid || obj->solid ){
                if (Collision::IsColliding(obj->box,thisObject->box) ){
                    return obj;
                }
            }
        }
    }
    return nullptr;
}



int Collision::AdjustCollisionIndependent(float &sx,float &sy,float dt,GameObject* dis,PoolManager &pool,PoolGroupId gid,float msize){
    Rect tempX(dis->box.x+sx*dt,dis->box.y,dis->box.w,dis->box.h);
    Rect tempY(dis->box.x,dis->box.y+sy*dt,dis->box.w,dis->box.h);
    Rect tempXY(dis->box.x+sx*dt,dis->box.y+sy*dt,dis->box.w,dis->box.h);
    int ret = -1;
    float colSize = msize;
    float colSize2 = msize;
    if (sx != 0 || sy !=0)
        for (auto &obj : pool.ForGroup(gid)){
            if (obj != dis && !obj->IsDead()){
                if ( obj->solid ){
                    if (Collision::IsColliding(obj->box,tempY)){
                        float ax=0;
                        bool canWarpInverted = false;
                        if (sy > 0){
                            if (canWarpInverted && Collision::IsColliding(obj->box,dis->box)){
                                for (ax=-sy;ax<0;ax -= colSize){
                                    Rect tempY2(dis->box.x,dis->box.y+ax*dt,dis->box.w,dis->box.h);
                                    if (!Collision::IsColliding(obj->box,tempY2)){
                                        ax-=colSize2;
                                        break;
                                    }
                                }
                            }else{
                                for (ax=0;ax<sy;ax += colSize){
                                    Rect tempY2(dis->box.x,dis->box.y+ax*dt,dis->box.w,dis->box.h);
                                    if (Collision::IsColliding(obj->box,tempY2)){

                                            ax-=colSize2;
                                        break;
                                    }
                                }
                            }
                            sy = ax;
                            Rect tempY2(dis->box.x,dis->box.y+sy*dt,dis->box.w,dis->box.h);
                            if (Collision::IsColliding(obj->box,tempY2)){
                                sy = 0;
                            }
                            dis->NotifyCollision(obj);

                            ret = 0;

                        }else{
                            for (ax=0;ax<(sy)*-1;ax+=colSize){
                                Rect tempY2(dis->box.x,dis->box.y-ax*dt,dis->box.w,dis->box.h);
                                if (Collision::IsColliding(obj->box,tempY2)){

                                        ax-=colSize2;
                                    break;
                                }
                            }
                            sy = -ax;
                            Rect tempY2(dis->box.x,dis->box.y-sy*dt,dis->box.w,dis->box.h);
                            if (Collision::IsColliding(obj->box,tempY2)){
                                sy = 0;
                            }
                            dis->NotifyCollision(obj);

                            ret = 1;
                        }

                    }
                    if (Collision::IsColliding(obj->box,tempX)){
                        float ax=0;
                        if (sx > 0){
                            int iter = 0;
                            for (ax=0;ax<sx;ax+=colSize){
                                Rect tempY2(dis->box.x+ax*dt,dis->box.y,dis->box.w,dis->box.h);
                                iter++;
                                if (Collision::IsColliding(obj->box,tempY2)){
                                    ax-=colSize2;
                                    break;
                                }
                            }
                            sx = ax;
                            Rect tempY2(dis->box.x+ax*dt,dis->box.y,dis->box.w,dis->box.h);
                            if (Collision::IsColliding(obj->box,tempY2)){
                                sx = 0;
                            }
                            dis->NotifyCollision(obj);
                            ret = 2;
                        }else{
                            for (ax=0;ax<(sx)*-1;ax+=colSize){
                                Rect tempY2(dis->box.x-ax*dt,dis->box.y,dis->box.w,dis->box.h);
                                if (Collision::IsColliding(obj->box,tempY2)){

                                        ax-=colSize2;
                                    break;
                                }
                            }
                            sx = -ax;
                            Rect tempY2(dis->box.x+ax*dt,dis->box.y,dis->box.w,dis->box.h);
                            if (Collision::IsColliding(obj->box,tempY2)){
                                sx = 0;
                            }
                            dis->NotifyCollision(obj);
                            ret = 3;
                        }
                    }
                    if (sy == 0 && sx == 0)
                        break;
                }
            }
        }
    return ret;
}

bool Collision::SoftWarpAway(GameObject* thisObject,GameObject* otherObject,Point speed){
    Rect aux = thisObject->box;
    float dist = 0;
    while (Collision::IsColliding(aux,otherObject->box)){
        dist += 0.5;
        aux = thisObject->box;
        aux.x += speed.x*dist;
        aux.y += speed.y*dist;
        if  (!Collision::IsColliding(aux,otherObject->box)){
            thisObject->box = aux;
            return true;
        }
    }
    return false;
}

bool Collision::SoftWarpAway(GameObject* thisObject,GameObject* otherObject){
    Point Center1 = thisObject->box.GetCenter();
    Point Center2 = otherObject->box.GetCenter();
    float angle = Center2.getDirection(Center1);
    Rect aux = thisObject->box;
    float dist = 0;
    while (Collision::IsColliding(aux,otherObject->box)){
        dist += 0.2f;
        aux = thisObject->box;
        aux.x += sin(angle)*dist;
        aux.y += cos(angle)*dist;
        if  (!Collision::IsColliding(aux,otherObject->box)){
            thisObject->box = aux;
            return true;
        }
    }
    /*for (auto &obj : pool.ForGroup(gid)){
        if (obj != dis && !obj->IsDead()){
            if ( obj->solid ){

            }
        }
    }*/
    return false;
}

bool Collision::WarpAway(Rect &obj1,Rect obj2){
    int direction = GetCollidingSide(obj1,obj2);
    switch (direction){
        case 0:{
            obj1.x = obj2.x - obj1.w;
            obj1.y = obj2.y - obj1.h;
            break;
        };
        case 1:{
            obj1.x = obj2.x - obj1.w;
            obj1.y = obj2.y - obj1.h;
            break;
        };
        case 2:{
            obj1.x = obj2.x + obj2.w;
            obj1.y = obj2.y + obj2.h;
            break;
        };
        case 3:{
            obj1.x = obj2.x - obj1.w;
            obj1.y = obj2.y + obj2.h;
            break;
        };
    }
    return true;
}
