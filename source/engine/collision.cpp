#include "collision.hpp"



int Collision::AdjustCollision(float &sx,float &sy,float dt,GameObject* dis,PoolManager &pool,PoolGroupId gid){
    Rect tempXY(dis->box.x+sx*dt,dis->box.y+sy*dt,dis->box.w,dis->box.h);
    if (sx != 0 or sy !=0)
        for (auto &obj : pool.ForGroup(gid))
        {
            if (obj != dis && !obj->IsDead()){
                if (obj->solid ){
                    if (Collision::IsColliding(obj->box,tempXY,0,0) ){
                        sy = 0;
                        sx = 0;
                        dis->NotifyCollision((GameObject*)obj);
                        return 4;
                    }
                    if (sy == 0 and sx == 0)
                        break;
                }
            }
        }
        return -1;

}

bool Collision::CheckCollision(Rect tempXY,GameObject* dis,PoolManager &pool,PoolGroupId gid,bool onlySolid){
        for (auto &obj : pool.ForGroup(gid))
        {
        if (obj != dis && !obj->IsDead()){
            if (!onlySolid ||obj->solid ){
                if (Collision::IsColliding(obj->box,tempXY,0,0) ){
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
                if (Collision::IsColliding(obj->box,thisObject->box,0,0) ){
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
                if (Collision::IsColliding(obj->box,thisObject->box,0,0) ){
                    return obj;
                }
            }
        }
    }
    return nullptr;
}



int Collision::AdjustCollisionIndependent(float &sx,float &sy,float dt,GameObject* dis,PoolManager &pool,PoolGroupId gid){
    Rect tempX(dis->box.x+sx*dt,dis->box.y,dis->box.w,dis->box.h);
    Rect tempY(dis->box.x,dis->box.y+sy*dt,dis->box.w,dis->box.h);
    Rect tempXY(dis->box.x+sx*dt,dis->box.y+sy*dt,dis->box.w,dis->box.h);
    int ret = -1;
    if (sx != 0 or sy !=0)
        for (auto &obj : pool.ForGroup(gid)){
            if (obj != dis && !obj->IsDead()){
                if ( obj->solid ){
                    if (Collision::IsColliding(obj->box,tempY,0,0)){
                        int ax=0;
                        if (sy > 0){
                            for (ax=0;ax<sy;ax++){
                                Rect tempY2(dis->box.x,dis->box.y+ax*dt,dis->box.w,dis->box.h);
                                if (Collision::IsColliding(obj->box,tempY2,0,0)){
                                    ax-=1;
                                    break;
                                }
                            }
                            dis->NotifyCollision(obj);
                            sy = ax;
                            ret = 0;
                        }else{
                            for (ax=0;ax<(sy)*-1;ax++){
                                Rect tempY2(dis->box.x,dis->box.y-ax*dt,dis->box.w,dis->box.h);
                                if (Collision::IsColliding(obj->box,tempY2,0,0)){
                                    ax-=1;
                                    break;
                                }
                            }
                            dis->NotifyCollision(obj);
                            sy = -ax;
                            ret = 1;
                        }

                    }
                    if (Collision::IsColliding(obj->box,tempX,0,0)){
                        int ax=0;
                        if (sx > 0){
                            for (ax=0;ax<sx;ax++){
                                Rect tempY2(dis->box.x+ax*dt,dis->box.y,dis->box.w,dis->box.h);
                                if (Collision::IsColliding(obj->box,tempY2,0,0)){
                                    ax -= 1;
                                    break;
                                }
                            }
                            dis->NotifyCollision(obj);
                            sx = ax;
                            ret = 2;
                        }else{
                            for (ax=0;ax<(sx)*-1;ax++){
                                Rect tempY2(dis->box.x-ax*dt,dis->box.y,dis->box.w,dis->box.h);
                                if (Collision::IsColliding(obj->box,tempY2,0,0)){
                                    ax-=1;
                                    break;
                                }
                            }
                            dis->NotifyCollision(obj);
                            sx = -ax;
                            ret = 3;
                        }
                    }
                    if (sy == 0 and sx == 0)
                        break;
                }
            }
        }
    return ret;
}

bool Collision::SoftWarpAway(GameObject* thisObject,GameObject* otherObject){
    int direction = -1;
    Point Center1 = thisObject->box.GetCenter();
    Point Center2 = otherObject->box.GetCenter();
    float angle = Center1.getDirection(Center2);
    for (float dist=(otherObject->box.w+otherObject->box.h)/2;dist < otherObject->box.w*otherObject->box.w;dist++){
        Rect aux = thisObject->box;
        aux.x += sin(angle)*dist;
        aux.y += cos(angle)*dist;
        if  (not Collision::IsColliding(aux,otherObject->box)){
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
    int direction = -1;
    /*
        0 top left
        1 top right
        2 bottom right
        3 bottom left
    */
    Point Center1 = obj1.GetCenter();
    Point Center2 = obj2.GetCenter();
    if (Center1.y > Center2.y){
        //Means top
        if (Center1.x < Center2.x){
            direction = 0;
        }else{
            direction = 1;
        }
    }else{
        //means Bottom
        if (Center1.x < Center2.x){
            direction = 3;
        }else{
            direction = 2;
        }
    }
    std::cout << direction << "\n";
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
}
