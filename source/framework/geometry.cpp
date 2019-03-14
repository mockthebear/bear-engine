#include "geometry.hpp"
#include <math.h>

Circle::Circle(float xx,float yy,float rr){
    x = xx;
    y = yy;
    r = rr;
}
Circle::Circle(){
    x = y = 0;
    r = 1;
}

template <typename T> bool onSegment(GenericPoint<T> p, GenericPoint<T> q, GenericPoint<T> r)
{
    if (q.x <= std::max(p.x, r.x) && q.x >= std::min(p.x, r.x) &&
            q.y <= std::max(p.y, r.y) && q.y >= std::min(p.y, r.y))
        return true;
    return false;
}

template <typename T> int orientation(GenericPoint<T> p, GenericPoint<T> q, GenericPoint<T> r)
{
    int val = (q.y - p.y) * (r.x - q.x) -
              (q.x - p.x) * (r.y - q.y);
    if (val == 0) return 0;
    return (val > 0)? 1: 2;
}

template <typename T> bool doIntersect(GenericPoint<T> p1, GenericPoint<T> q1, GenericPoint<T> p2, GenericPoint<T> q2)
{
    int o1 = orientation(p1, q1, p2);
    int o2 = orientation(p1, q1, q2);
    int o3 = orientation(p2, q2, p1);
    int o4 = orientation(p2, q2, q1);

    // General case
    if (o1 != o2 && o3 != o4)
        return true;

    // Special Cases
    // p1, q1 and p2 are colinear and p2 lies on segment p1q1
    if (o1 == 0 && onSegment(p1, p2, q1)) return true;

    // p1, q1 and p2 are colinear and q2 lies on segment p1q1
    if (o2 == 0 && onSegment(p1, q2, q1)) return true;

    // p2, q2 and p1 are colinear and p1 lies on segment p2q2
    if (o3 == 0 && onSegment(p2, p1, q2)) return true;

     // p2, q2 and q1 are colinear and q1 lies on segment p2q2
    if (o4 == 0 && onSegment(p2, q1, q2)) return true;

    return false; // Doesn't fall in any of the above cases
}


template <typename T> bool GenericPolygon<T>::IsInside(GenericPoint<T> p){
    if (GetVerticesCount() < 3){
        return false;
    }
    GenericPoint<T> maxm(0xffff, p.y);

    int count = 0, i = 0;
    do{
        int next = (i+1)%GetVerticesCount();
        if (doIntersect(vertices[i], vertices[next], p, maxm))
        {
            if (orientation(vertices[i], p, vertices[next]) == 0)
               return onSegment(vertices[i], p, vertices[next]);
            count++;
        }
        i = next;
    }while (i != 0);


    return count&1;
}


template class GenericPolygon<float>;
