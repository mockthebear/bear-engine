#include <algorithm>
#include <ctgmath>
#include "object.hpp"
#include "../framework/poolmanager.hpp"

#include "../framework/geometry.hpp"

#pragma once
namespace Collision {
        static inline double distanceSquared( int x1, int y1, int x2, int y2 )
        {
            int deltaX = x2 - x1;
            int deltaY = y2 - y1;
            return deltaX*deltaX + deltaY*deltaY;
        }
        static inline float Mag(const Point& p) {
            return sqrt(p.x * p.x + p.y * p.y);
        }

        static inline Point Norm(const Point& p) {
            return p * (1.f / Mag(p));
        }

        static inline float Dot(const Point& a, const Point& b) {
            return a.x * b.x + a.y * b.y;
        }

        static inline Point Rotate(const Point& p, float angle) {
            float cs = cos(angle), sn = sin(angle);
            return Point ( p.x * cs - p.y * sn, p.x * sn + p.y * cs );
        }
        /**
            0 top left
            1 top right
            2 bottom right
            3 bottom left
        */
        static inline int GetCollidingSide( Rect A,Rect B ){
            int direction = -1;


            Point Center1 = A.GetCenter();
            Point Center2 = B.GetCenter();
            if (Center1.y >= Center2.y){
                //Means top
                if (Center1.x <= Center2.x){
                    direction = 1;
                }else{
                    direction = 0;
                }
            }else{
                //means Bottom
                if (Center1.x <= Center2.x){
                    direction = 3;
                }else{
                    direction = 2;
                }
            }
            return direction;
        }

        static inline bool IsColliding( Rect b,Point a ){
            return a.x > b.x && a.x < b.x+b.w && a.y > b.y && a.y < b.y+b.h;
        }


        static inline bool IsColliding( Rect b,Circle a ){
            float cX, cY;
            if( a.x <= b.x )
            {
                cX = b.x;
            }
            else if( a.x >= b.x + b.w )
            {
                cX = b.x + b.w;
            }
            else
            {
                cX = a.x;
            }

            //Find closest y offset
            if( a.y <= b.y )
            {
                cY = b.y;
            }
            else if( a.y >= b.y + b.h )
            {
                cY = b.y + b.h;
            }
            else
            {
                cY = a.y;
            }

            //If the closest point is inside the circle
            if( distanceSquared( a.x, a.y, cX, cY ) <= a.r * a.r )
            {
                //This box and the circle have collided
                return true;
            }

            //If the shapes have not collided
            return false;
        };
        static inline bool IsColliding( Rect rect,Circle circle, double angle ){
            // Rotate circle's center point back
            float centerX = rect.GetXCenter();
            float centerY = rect.GetYCenter();

            double unrotatedCircleX = cos(angle) * (circle.x - centerX) - sin(angle) * (circle.y - centerY) + centerX;
            double unrotatedCircleY = sin(angle) * (circle.x - centerX) + cos(angle) * (circle.y - centerY) + centerY;

            double closestX, closestY;

            if (unrotatedCircleX  < rect.x)
                closestX = rect.x;
            else if (unrotatedCircleX  > rect.x + rect.w)
                closestX = rect.x + rect.w;
            else
                closestX = unrotatedCircleX ;

            // Find the unrotated closest y point from center of unrotated circle
            if (unrotatedCircleY < rect.y)
                closestY = rect.y;
            else if (unrotatedCircleY > rect.y + rect.h)
                closestY = rect.y + rect.h;
            else
                closestY = unrotatedCircleY;

            // Determine collision


            double distance = Point(unrotatedCircleX , unrotatedCircleY).GetDistance(Point(closestX, closestY));
            if (distance < circle.r)
                return true;
            else
                return false;
        };


        static inline bool IsColliding( Rect& b,Cone& a ){
            //Closest point on collision box
            float cX=b.x, cY=b.y;

            //Find closest x offset
            if( a.x <= b.x )
            {
                cX = b.x;
            }
            else if( a.x >= b.x + b.w )
            {
                cX = b.x + b.w;
            }
            else
            {
                cX = a.x;
            }
            if( a.y <= b.y )
            {
                cY = b.y;
            }
            else if( a.y >= b.y + b.h )
            {
                cY = b.y + b.h;
            }
            else
            {
                cY = a.y;
            }
            if( distanceSquared( a.x, a.y, cX, cY ) <= a.r * a.r )
            {

                float angle = atan2(cY-a.y,cX-a.x);
                if (angle < 0){
                    angle = (Geometry::PI()*2+angle);
                }
                if (a.start_angle > a.opening){
                    if (angle <= a.start_angle && angle >= (a.opening) ){
                        return false;
                    }else{
                        return true;
                    }
                }else{
                    if (angle >= a.start_angle && angle <= (a.opening) ){
                        return true;
                    }
                }

                return false;
            }



            return false;
        };


        template<typename T,typename K> static inline bool IsColliding(GenericRect<T> a,  GenericRect<K> b){
            if( a.y + a.h < b.y )
            {
                return false;
            }
            if( a.y > b.y + b.h )
            {
                return false;
            }
            if( (a.x + a.w) < b.x )
            {
                return false;
            }
            if( a.x >  b.x + b.w )
            {
                return false;
            }
            return true;
        };

        static inline int GetCollidingSide2( Rect A,Rect B ){
            if (!IsColliding(A,B))
                return -1;
            Point Center1 = A.GetCenter();
            Point Center2 = B.GetCenter();
            int dir = 0;
            float angle = Center1.getDirection(Center2);
            /*
                0 left
                1 top
                2 right
                3 bottom
            */
            angle = Geometry::toDeg(angle);
            if (angle <= 45 || angle > 315){
                dir = 0;
            }else if(angle > 45 && angle <= 135){
                dir = 1;
            }else if(angle > 135 && angle <= 225){
                dir = 2;
            }else if(angle > 225 && angle <= 315){
                dir = 3;
            }
            return dir;
        }
        static inline int GetCollidingDepth(Rect& a,  Rect& b){
            if (!IsColliding(a,b)){
                return 0;
            }
            int side = GetCollidingSide2(a,b);
            if (side == 0){
                return abs(a.x - (b.x+b.w) );
            }else if (side == 1){
                return abs(a.y - (b.y+b.h) );
            }else if (side == 2){
                return abs(a.x + a.w - (b.x) );
            }else if (side == 3){
                return abs(a.y + a.h - (b.y) );
            }else{
                return -1;
            }
        }


        // Essa é uma implementação do SAT feita pelo Lucas Neves.
        // Recebe dois Rects e suas rotações, e detecta se os retângulos colidem.
        // Mais informações sobre o método podem ser encontradas nos seguintes links:
        // http://www.metanetsoftware.com/technique/tutorialA.html
        // http://www.gamedev.net/page/resources/_/technical/game-programming/2d-rotated-rectangle-collision-r2604
        static inline bool IsColliding(Rect& a,  Rect& b, float angleOfA, float angleOfB) {
            Point A[] = { Point( a.x, a.y + a.h ),
                          Point( a.x + a.w, a.y + a.h ),
                          Point( a.x + a.w, a.y ),
                          Point( a.x, a.y )
                        };
            Point B[] = { Point( b.x, b.y + b.h ),
                          Point( b.x + b.w, b.y + b.h ),
                          Point( b.x + b.w, b.y ),
                          Point( b.x, b.y )
                        };

            for (auto& v : A) {
                v = Rotate(v - a.GetCenter(), angleOfA) + a.GetCenter();
            }

            for (auto& v : B) {
                v = Rotate(v - b.GetCenter(), angleOfB) + b.GetCenter();
            }

            Point axes[] = { Norm(A[0] - A[1]), Norm(A[1] - A[2]), Norm(B[0] - B[1]), Norm(B[1] - B[2]) };

            for (auto& axis : axes) {
                float P[4];

                for (int i = 0; i < 4; ++i) P[i] = Dot(A[i], axis);

                float minA = *std::min_element(P, P + 4);
                float maxA = *std::max_element(P, P + 4);

                for (int i = 0; i < 4; ++i) P[i] = Dot(B[i], axis);

                float minB = *std::min_element(P, P + 4);
                float maxB = *std::max_element(P, P + 4);

                if (maxA <= minB || minA >= maxB)
                    return false;
            }

            return true;
        }

	static inline bool IsColliding( Circle& b,Circle& a ){
        if ( a.GetPoint().GetDistance(b.GetPoint())  <= b.r+a.r){
            return true;
        }
        return false;
    }

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
