#include "geometry.hpp"
#include <math.h>
Rect::Rect(float xx,float yy,float ww,float hh){
    x = xx;
    y = yy;
    h = hh;
    w = ww;
}
Rect::Rect(){
    x = y =h = w =0;
}
Point Rect::GetCenter(){
    return Point(x+(w/2.0),y+(h/2.0));
}
bool Rect::IsInside(float xp,float yp){
    return xp >= x and xp <= x+w and yp >= y and yp <= y+h;
}


bool Rect::IsInside(Rect *r){
    //printf("%f - %f\n",x,w);
    return r->x >= x and r->x <= x+w and r->y >= y and r->y <= y+h;
}


Circle::Circle(float xx,float yy,float rr){
    x = xx;
    y = yy;
    r = rr;
}
Circle::Circle(){
    x = y = r = 0;
}

void Rect::Sum(float n){
    x = x+n;
    y = y+n;
    h = h+n;
    w = w+n;
}

