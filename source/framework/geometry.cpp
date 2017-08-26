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
