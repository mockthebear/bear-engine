#ifndef GEOMH
#define GEOMH
#include <math.h>
#include <stdio.h>
/**
    @brief Basic template Point class (2d vector)
*/
template <typename T=float> class GenericPoint{
    public:
        /**
            Constructor star all with 0
        */
        GenericPoint(){
            x = y = 0;
        };
        GenericPoint(T x_,T y_){
            x = x_;
            y = y_;
        };


        GenericPoint(const int n[2]){
            x = n[0];
            y = n[1];
        };
        GenericPoint(const float n[2]){
            x = n[0];
            y = n[1];
        };
        GenericPoint(const long n[2]){
            x = n[0];
            y = n[1];
        };
        GenericPoint(const GenericPoint *p){
            x = p->x;
            y = p->y;
        }
        GenericPoint(const GenericPoint &p){
            x = p.x;
            y = p.y;
        }



        /**
            Get the distance between two points
        */
        T getDistance(GenericPoint *p){
            return sqrt(pow((x - p->x),(T)2) + pow((y-p->y),(T)2));
        };
        /**
            Get the distance between two points
        */
        T getDistance(GenericPoint p){
             return sqrt(pow((x - p.x),(T)2) + pow((y-p.y),(T)2));
        };
        /**
            Get the direction in rads between two points
        */
        T getDirection(GenericPoint p){
            return atan2(y-p.y,x-p.x);
        };
        /**
            Sum two points p1.x + p2.x, p1.y + p2.y
        */
        GenericPoint operator+(const GenericPoint& rhs) const {
           return GenericPoint(x + rhs.x, y + rhs.y);
        };
        /**
            Sub two points p1.x - p2.x, p1.y - p2.y
        */


        GenericPoint operator-(const GenericPoint& rhs) const {
            return GenericPoint(x - rhs.x, y - rhs.y);
        }
        GenericPoint operator*(T f) const {
           return GenericPoint(x*f,y*f);
        }
        /**
            The X;
        */
        T x;
        /**
            The Y;
        */
        T y;

};
/**
    @brief [WIP] Basic class to 3d points
*/
template <typename T=float> class GenericPoint3{
    public:
        GenericPoint3(){
            x = y = z = 0;
        };
        GenericPoint3(const int n[3]){
            x = n[0];
            y = n[1];
            z = n[2];
        };
        GenericPoint3(const float n[3]){
            x = n[0];
            y = n[1];
            z = n[2];
        };
        GenericPoint3(const long n[3]){
            x = n[0];
            y = n[1];
            z = n[2];
        };

        GenericPoint3(T x_,T y_,T z_){
            x = x_;
            y = y_;
            z = z_;
        };
        T x,y,z;
};
/**
    @brief Point its an GenericPoint made from float
*/
typedef GenericPoint<float> Point;
/**
    @brief Point its an GenericPoint (3d) made from float
*/
typedef GenericPoint3<float> Point3;
/**
    @brief Point its an GenericPoint made from int
*/
typedef GenericPoint<int> PointInt;


/**
    @brief Basic rect class
*/
template <typename T=float>class GenericRect{
    public:
        /**
            Constructor
        */
        GenericRect(T xx,T yy ,T ww,T hh){
            x = xx;
            y = yy;
            h = hh;
            w = ww;
        }
        /**
            Empty constructor start all components with 0
        */

        GenericRect(const int n[4]){
            x = n[0];
            y = n[1];
            w = n[2];
            h = n[3];
        };
        GenericRect(const float n[4]){
            x = n[0];
            y = n[1];
            w = n[2];
            h = n[3];
        };
        GenericRect(const long n[4]){
            x = n[0];
            y = n[1];
            w = n[2];
            h = n[3];
        };

        GenericRect(){
            x = y = h = w = 0;
        };
        /**
            Update only the x,y
            @param p Get a point then apply Point x and y to the cordinates of the Rect
        */
        void operator=(const Point& p){
            x = p.x;
            y = p.y;
        };

        void operator=(const GenericRect<int>& p){
            x = p.x;
            y = p.y;
            w = p.w;
            h = p.h;
        };
        void operator=(const GenericRect<double>& p){
            x = p.x;
            y = p.y;
            w = p.w;
            h = p.h;
        };
        void operator=(const GenericRect<float>& p){
            x = p.x;
            y = p.y;
            w = p.w;
            h = p.h;
        };
        /**
            Get x,y
            @return an Point with the x,y
        */
        GenericPoint<T> GetPos(){return GenericPoint<T>(x,y);};
        /**
            @return the middle position of the rect. X axis
        */
        T getXCenter(){return x+w/2.0;};
        /**
            @return the middle position of the rect. Y axis
        */
        T getYCenter(){return y+h/2.0;};
        /**
            @return if the given two positions are inside the rect
        */
        bool IsInside(T xx,T yy){
            return xx >= x and xx <= x+w and yy >= y and yy <= y+h;
        };
        bool IsInside(Point p){
            return p.x >= x and p.x <= x+w and p.y >= y and p.y <= y+h;
        };
        /**
            @return if the given Point are inside the rect
        */
        bool IsInside(GenericRect<int> r){
            return r.x >= x and r.x <= x+w and r.y >= y and r.y <= y+h;
        };
        bool IsInside(GenericRect<float> r){
            return r.x >= x and r.x <= x+w and r.y >= y and r.y <= y+h;
        };
        bool IsInside(GenericRect<double> r){
            return r.x >= x and r.x <= x+w and r.y >= y and r.y <= y+h;
        };


        /**
            @return A Point containing in x,y the middle position of the rect
        */
        GenericPoint<T> GetCenter(){return GenericPoint<T>(x+(w/2.0),y+(h/2.0));}
        T x,y,h,w;
};

typedef GenericRect<float> Rect;
typedef GenericRect<int> RectInt;
/**
    @brief [WIP] Class to deal with angles and geometry stuff
*/
class Geometry{
    public:
        /**
            Receive an angle in degree and convert to rad
        */
        static double toRad(double v){ return (v < 0 ? 360-v : v)* PI() / 180.0; };
        /**
            Receive an angle in rad and convert to degree
        */
        static double toDeg(double v){ return (v < 0 ? 2.0*PI() + v : v) * 180.0 / PI(); };
        /**
            PI :V
        */
        static inline double PI(){ return 3.14159265;};
};
/**
    @brief [WIP] Class to describe an semi circle.
*/

class Circle{
    public:
        Circle(float,float,float);
        Circle(const float n[3]){
            x = n[0];
            y = n[1];
            r = n[2];
        }
        Circle();
        float x,y,r;
};

class Cone : public Circle{
    public:
        /**
            @param x_ the X position
            @param y_ the Y position
            @param radius_ Radius of the circle
            @param start_angle_ The angle of the begin
            @param opening_ the amount of angles in degree of opening
        */
        Cone(float x_,float y_,float radius_,float start_angle_,float opening_){
            x = x_;
            y = y_;
            r = radius_;
            start_angle= start_angle_;
            opening = opening_;
        };
        Cone(){
            x = 0;
            y = 0;
            r = 1;
            start_angle= 0;
            opening = 0;
        };
        float start_angle,opening;
};



#endif
