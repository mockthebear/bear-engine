#include "collision.hpp"
#include "gamebase.hpp"



int Collision::AdjustCollision(float &sx,float &sy,float dt,GameObject* dis,PoolManager &pool,PoolGroupId gid,bool onlySolid){
    Rect tempXY(dis->box.x+sx*dt,dis->box.y+sy*dt,dis->box.w,dis->box.h);
    if (sx != 0.0f || sy != 0.0f)
        for (auto &obj : pool.ForGroup(gid))
        {
            if (obj != dis && !obj->IsDead()){
                if (!onlySolid || obj->IsSolid(dis) ){
                    if (Collision::IsColliding(obj->box,tempXY) ){
                        sy = 0;
                        sx = 0;
                        /*tempXY = Rect(dis->box.x+sx*dt,dis->box.y+sy*dt,dis->box.w,dis->box.h);
                        if (Collision::IsColliding(obj->box,tempXY,0,0) ){
                            SoftWarpAway(dis,obj);
                        }*/
                        dis->NotifyCollision((GameObject*)obj);
                        obj->NotifyCollision(dis);
                        return 4;
                    }
                    if (sy == 0.0f && sx == 0.0f)
                        break;
                }
            }
        }
        return -1;

}

Rect Collision::GetIntersectRect(Rect& a,  Rect& b){
    if (!Collision::IsColliding(a,b)){
        return Rect(0,0,0,0);
    }
    float c1 = std::max(a.x, b.x);
    float c2 = std::max(a.y, b.y);
    float c3 = std::min(a.x+a.w,b.x + b.w);
    float c4 = std::min(a.y + a.h, b.y + b.h);
    return Rect(c1,c2, c3-c1,c4-c2 );
}

int Collision::AdjustCollision(float &sx,float &sy,float dt,GameObject* dis,std::vector<GameObject*> vec,bool onlySolid){
    Rect tempXY(dis->box.x+sx*dt,dis->box.y+sy*dt,dis->box.w,dis->box.h);
    if (sx != 0.0f || sy != 0.0f)
        for (auto &obj : vec)
        {
            if (obj != dis && !obj->IsDead()){
                if (!onlySolid || obj->IsSolid(dis) ){
                    if (Collision::IsColliding(obj->box,tempXY) ){
                        sy = 0;
                        sx = 0;
                        /*tempXY = Rect(dis->box.x+sx*dt,dis->box.y+sy*dt,dis->box.w,dis->box.h);
                        if (Collision::IsColliding(obj->box,tempXY,0,0) ){
                            SoftWarpAway(dis,obj);
                        }*/
                        dis->NotifyCollision(obj);
                        obj->NotifyCollision(dis);
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
            if (!onlySolid ||obj->IsSolid(dis) ){
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
            if (!onlySolid ||obj->IsSolid(dis) ){
                if (Collision::IsColliding(obj->box,tempXY) ){
                    return true;
                }
            }
        }
    }
    return false;
}

bool Collision::CheckCollision(Rect tempXY,GameObject* dis,std::vector<GameObject*> vec,bool onlySolid){
        for (auto &obj : vec)
        {
        if (obj && obj != dis && !obj->IsDead()){
            if (!onlySolid ||obj->IsSolid(dis) ){
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
    for (auto &obj : pool.ForGroup(gid)){
        if (obj && obj != thisObject && !obj->IsDead()){
            if (!onlySolid || obj->IsSolid(thisObject) ){
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
            if (!onlySolid || obj->IsSolid(thisObject) ){
                if (Collision::IsColliding(obj->box,thisObject->box) ){
                    return obj;
                }
            }
        }
    }
    return nullptr;
}

std::vector<GameObject*> Collision::GetNearObjects(GameObject* dis,PoolManager &pool,PoolGroupId gid,float scale,bool onlySolid){
    std::vector<GameObject*> nearObjs;
    if (!dis){
        return nearObjs;
    }
    Rect newR = dis->box;
    newR.w *= scale;
    newR.h *= scale;
    newR.CenterRectIn(dis->box);
    for (auto &obj : pool.ForGroup(gid)){
        if (obj != dis && !obj->IsDead()){
            if (!onlySolid || obj->IsSolid(dis) ){
                if (Collision::IsColliding(obj->box,newR)){
                    nearObjs.emplace_back(obj);
                }
            }
        }
    }

    return nearObjs;
}


int Collision::AdjustCollisionIndependent(float &sx,float &sy,float dt,GameObject* dis,PoolManager &pool,PoolGroupId gid,float msize,bool onlySolid){
    Rect tempX(dis->box.x+sx*dt,dis->box.y,dis->box.w,dis->box.h);
    Rect tempY(dis->box.x,dis->box.y+sy*dt,dis->box.w,dis->box.h);
    Rect tempXY(dis->box.x+sx*dt,dis->box.y+sy*dt,dis->box.w,dis->box.h);
    int ret = -1;
    float colSize = msize;
    float colSize2 = msize;
    if (sx != 0 || sy !=0)
        for (auto &obj : pool.ForGroup(gid)){
            if (obj != dis && !obj->IsDead()){
                if (!onlySolid || obj->IsSolid(dis) ){
                    if (Collision::IsColliding(obj->box,tempY)){
                        float ax=0;
                        if (sy > 0){
                            /*if (canWarpInverted && Collision::IsColliding(obj->box,dis->box)){
                                for (ax=-sy;ax<0;ax -= colSize){
                                    Rect tempY2(dis->box.x,dis->box.y+ax*dt,dis->box.w,dis->box.h);
                                    if (!Collision::IsColliding(obj->box,tempY2)){
                                        ax-=colSize2;
                                        break;
                                    }
                                }
                            }else{*/
                            for (ax=-colSize*2.0f;ax<sy;ax += colSize){
                                Rect tempY2(dis->box.x,dis->box.y+ax*dt,dis->box.w,dis->box.h);
                                if (Collision::IsColliding(obj->box,tempY2)){
                                        if (ax >= 0){
                                            ax -=colSize2;
                                        }
                                    break;
                                }
                            }

                            sy = ax;
                            Rect tempY2(dis->box.x,dis->box.y+sy*dt,dis->box.w,dis->box.h);
                            if (Collision::IsColliding(obj->box,tempY2)){
                                sy = 0;
                            }
                            dis->NotifyCollision(obj);
                            obj->NotifyCollision(dis);

                            ret = 0;

                        }else{
                            //for (ax=-colSize*2.0f;ax<sy;ax += colSize){

                            for (ax=colSize*2.0f;ax>sy;ax-=colSize){
                                Rect tempY2(dis->box.x,dis->box.y-ax*dt,dis->box.w,dis->box.h);
                                if (Collision::IsColliding(obj->box,tempY2)){
                                    if (ax <= 0){
                                        ax +=colSize2;
                                    }
                                        //ax+=colSize2;
                                    break;
                                }
                            }
                            sy = ax;
                            Rect tempY2(dis->box.x,dis->box.y-sy*dt,dis->box.w,dis->box.h);
                            if (Collision::IsColliding(obj->box,tempY2)){
                                sy = 0;
                            }
                            dis->NotifyCollision(obj);
                            obj->NotifyCollision(dis);

                            ret = 1;
                        }

                    }
                    if (Collision::IsColliding(obj->box,tempX)){
                        float ax=0;
                        if (sx > 0){
                            int iter = 0;
                            for (ax=-colSize;ax<sx;ax+=colSize){
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
                            obj->NotifyCollision(dis);
                            ret = 2;
                        }else{
                            for (ax=colSize;ax>sx;ax-=colSize){
                                Rect tempY2(dis->box.x-ax*dt,dis->box.y,dis->box.w,dis->box.h);
                                if (Collision::IsColliding(obj->box,tempY2)){

                                        ax+=colSize2;
                                    break;
                                }
                            }
                            sx = ax;
                            Rect tempY2(dis->box.x+ax*dt,dis->box.y,dis->box.w,dis->box.h);
                            if (Collision::IsColliding(obj->box,tempY2)){
                                sx = 0;
                            }
                            dis->NotifyCollision(obj);
                            obj->NotifyCollision(dis);
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

int Collision::AdjustCollisionIndependent(float &sx,float &sy,float dt,GameObject* dis,std::vector<GameObject*> vec,float msize,bool onlySolid){
    Rect tempX(dis->box.x+sx*dt,dis->box.y,dis->box.w,dis->box.h);
    Rect tempY(dis->box.x,dis->box.y+sy*dt,dis->box.w,dis->box.h);

    int ret = -1;
    float colSize = msize;
    float colSize2 = msize;
    if (sx != 0 || sy !=0)
        for (auto &obj : vec){
            if (obj != dis && !obj->IsDead()){
                if (!onlySolid || obj->IsSolid(dis) ){
                    if (Collision::IsColliding(obj->box,tempY)){
                        float ax=0;
                        if (sy > 0){

                            float dti = Game::GetInstance()->GetDeltaTimeI();
                            float checkSize = (sy/dti);
                            for (ax=0;ax<sy;ax += checkSize){
                                Rect tempY2(dis->box.x,dis->box.y+ax*dt,dis->box.w,dis->box.h);
                                if (Collision::IsColliding(obj->box,tempY2)){
                                    if (ax == 0){
                                        for (ax = -sy*2;ax < 0;ax += checkSize){
                                            Rect tempY3(dis->box.x,dis->box.y+ax*dt,dis->box.w,dis->box.h);
                                            //bear::out << ax << " -> "<<Collision::IsColliding(obj->box,tempY3)<<":"<<(tempY3.y-dis->box.y) <<"\n";
                                            if (Collision::IsColliding(obj->box,tempY3)){
                                                //bear::out << "p:"<<ax<<"\n";
                                                //ax = 0;
                                                //dis->box.y = obj->box.y-dis->box.h-checkSize*dt;
                                                ax -= checkSize;

                                                break;
                                            }
                                        }
                                        break;
                                    }else{
                                        ax -= checkSize;
                                    }
                                    break;
                                }
                            }
                            sy = ax;
                            tempY = Rect(dis->box.x,dis->box.y+sy*dt,dis->box.w,dis->box.h);
                            dis->NotifyCollision(obj);
                            obj->NotifyCollision(dis);
                            if (Collision::IsColliding(obj->box,tempY)){
                                sy = 0;
                            }


                            ret = 0;
                        }else{
                            for (ax=colSize;ax>sy;ax-=colSize){
                                Rect tempY2(dis->box.x,dis->box.y-ax*dt,dis->box.w,dis->box.h);
                                if (Collision::IsColliding(obj->box,tempY2)){

                                        ax+=colSize2;
                                    break;
                                }
                            }
                            sy = ax;
                            tempY = Rect(dis->box.x,dis->box.y+sy*dt,dis->box.w,dis->box.h);
                            if (Collision::IsColliding(obj->box,tempY)){
                                sy = 0;

                            }
                            dis->NotifyCollision(obj);
                            obj->NotifyCollision(dis);


                            ret = 1;
                        }

                    }
                    if (Collision::IsColliding(obj->box,tempX)){
                        float ax=0;
                        if (sx > 0){
                            for (ax=-colSize;ax<sx;ax+=colSize){
                                Rect tempY2(dis->box.x+ax*dt,dis->box.y,dis->box.w,dis->box.h);

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
                            tempX = Rect(dis->box.x+sx*dt,dis->box.y,dis->box.w,dis->box.h);
                            dis->NotifyCollision(obj);
                            obj->NotifyCollision(dis);
                            ret = 2;
                        }else{
                            for (ax=colSize;ax>sx;ax-=colSize){
                                Rect tempY2(dis->box.x-ax*dt,dis->box.y,dis->box.w,dis->box.h);
                                if (Collision::IsColliding(obj->box,tempY2)){

                                        ax+=colSize2;
                                    break;
                                }
                            }
                            sx = ax;
                            Rect tempY2(dis->box.x+ax*dt,dis->box.y,dis->box.w,dis->box.h);
                            if (Collision::IsColliding(obj->box,tempY2)){
                                sx = 0;
                            }
                            tempX = Rect(dis->box.x+sx*dt,dis->box.y,dis->box.w,dis->box.h);
                            dis->NotifyCollision(obj);
                            obj->NotifyCollision(dis);
                            ret = 3;
                        }
                    }
                    if (sy == 0 && sx == 0){
                        break;
                    }
                    //Rect tempXY(dis->box.x+sx*dt,dis->box.y+sy*dt,dis->box.w,dis->box.h);
                    //if (Collision::IsColliding(obj->box,tempXY)){

                    //}
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
    float angle = Center2.GetDirection(Center1);
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
    int direction = Collision::GetCollidingSide(obj1,obj2);
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
        default: break;

    }
    return true;
}
