#include "collision.h"



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
