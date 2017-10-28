#ifndef GEOMH
#define GEOMH
#include <math.h>
#include <stdio.h>
#include <cstdint>
#include <algorithm>
#include "../settings/definitions.hpp"
#include SDL_LIB_HEADER

/**
    @brief Basic template Point class (2d vector)
*/
template <typename T=float> class GenericPoint{
    public:
        /**
            Constructor start all with 0
        */
        GenericPoint():x(0),y(0){};

        virtual ~GenericPoint() = default;
        /**
            Start with the two given values
            @param x_ the X position
            @param y_ the Y position
        */
        GenericPoint(T x_,T y_):x(x_),y(y_){};
        /**
            Start with the two given values
            @param n An vector of two elements (const), and those are respectively x and y
        */
        template<typename T2> GenericPoint(GenericPoint<T2> p){
            x = (T)p.x;
            y = (T)p.y;
        };

        GenericPoint(double angle){
            x = cos(angle);
            y = sin(angle);
        };

        GenericPoint(const int n[2]){
            x = n[0];
            y = n[1];
        };
        /**
            Start with the two given values
            @param n An vector of two elements (const), and those are respectively x and y
        */
        GenericPoint(const float n[2]){
            x = n[0];
            y = n[1];
        };
        /**
            Start with the two given values
            @param n An vector of two elements (const), and those are respectively x and y
        */
        GenericPoint(const long n[2]){
            x = n[0];
            y = n[1];
        };
        /**
            Start with and GenericPoint
            @param p an pointer
        */
        GenericPoint(const GenericPoint *p){
            x = p->x;
            y = p->y;
        }
        /**
            Start with and GenericPoint
            @param p an GenericPoint
        */
        GenericPoint(const GenericPoint &p){
            x = p.x;
            y = p.y;
        }

        GenericPoint(const SDL_Point &p):x(p.x),y(p.y){}
        /**
            Get the distance between two points
            @return distance between two points
        */
        T getDistance(GenericPoint *p){
            return sqrt(pow((x - p->x),(T)2) + pow((y-p->y),(T)2));
        };
        T GetDistance(GenericPoint *p){
            return sqrt(pow((x - p->x),(T)2) + pow((y-p->y),(T)2));
        };
        /**
            *Get the magnitude of the current point
            @return Magnitude
        */
        T Mag(){
            return sqrt(pow((x ),(T)2) + pow((y),(T)2));
        };
        /**
            Get the distance between two points
            @return distance between two points
        */
        T getDistance(GenericPoint p){
             return sqrt(pow((x - p.x),(T)2) + pow((y-p.y),(T)2));
        };
        T GetDistance(GenericPoint p){
             return sqrt(pow((x - p.x),(T)2) + pow((y-p.y),(T)2));
        };
        /**
            Get the direction in rads between two points
            @param return in radians the angle
        */
        T getDirection(GenericPoint p){
            return atan2(y-p.y,x-p.x);
        };

        T GetDirection(GenericPoint to){
            return atan2(to.y-y,to.x-x);
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
        /**
            mult two points p1.x * p2.x, p1.y * p2.y
        */
        GenericPoint operator*(T f) const {
           return GenericPoint(x*f,y*f);
        }
        /**
            divide two points p1.x / p2.x, p1.y / p2.y
        */
        GenericPoint operator/(T f) const {
           return GenericPoint(x/f,y/f);
        }
        /**
            Module of two points p1.x % p2.x, p1.y % p2.y
        */
        GenericPoint operator%(T f) const {
           return GenericPoint(x%f,y%f);
        }
        GenericPoint operator*(GenericPoint f) const {
           return GenericPoint(x*f.x,y*f.y);
        }
        GenericPoint operator/(GenericPoint& f) const {
           return GenericPoint(x/f.x,y/f.y);
        }

        void GridSnap(int gridSize){
            x -= (int)x%gridSize;
            y -= (int)y%gridSize;
        }
        /**
            The X codinate;
        */
        T x;
        /**
            The Y cordinate;
        */
        T y;

        uint32_t GetSize(){return 1;};

};
/**
    @brief [WIP] Basic class to 3d points
*/
template <typename T=float> class GenericPoint3{
    public:
        GenericPoint3():x(0),y(0),z(0){};
        GenericPoint3(T x_,T y_,T z_):x(x_),y(y_),z(z_){};

        virtual ~GenericPoint3() = default;

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

        T x,y,z;
        uint32_t GetSize(){return 1;};
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
        GenericRect(T xx,T yy ,T ww,T hh):x(xx),y(yy),w(ww),h(hh){}
        /**
            Empty constructor start all components with 0
        */
        template<typename K>GenericRect(GenericRect<K> reqt){
            x = reqt.x;
            y = reqt.y;
            h = reqt.h;
            w = reqt.w;
        }

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

        GenericRect():x(0),y(0),w(0),h(0){};
        /**
            Update only the x,y
            @param p Get a point then apply Point x and y to the cordinates of the Rect
        */
        GenericRect<T> operator=(const Point& p){
            x = p.x;
            y = p.y;
            return *this;
        };

        GenericRect<T> operator=(const GenericRect<int>& p){
            x = p.x;
            y = p.y;
            w = p.w;
            h = p.h;
            return *this;
        };
        GenericRect<T> operator=(const GenericRect<double>& p){
            x = p.x;
            y = p.y;
            w = p.w;
            h = p.h;
            return *this;
        };
        GenericRect<T> operator=(const GenericRect<float>& p){
            x = p.x;
            y = p.y;
            w = p.w;
            h = p.h;
            return *this;
        };

        GenericRect<T> operator=(SDL_Rect r){
            w = r.w;
            x = r.x;
            h = r.h;
            y = r.y;
            return *this;
        }

        SDL_Rect operator<<(const SDL_Rect& rhs) const {
            rhs.w = w;
            rhs.x = x;
            rhs.h = h;
            rhs.y = y;
            return rhs;
        }


        /**
            Get x,y
            @return an Point with the x,y
        */
        GenericPoint<T> GetPos(){return GenericPoint<T>(x,y);};
        /**
            @return the middle position of the rect. X axis
        */
        T GetXCenter(){return x+w/2.0;};
        /**
            @return the middle position of the rect. Y axis
        */
        T GetYCenter(){return y+h/2.0;};
        /**
            @return if the given two positions are inside the rect
        */
        bool IsInside(T xx,T yy){
            return xx >= x && xx <= x+w && yy >= y && yy <= y+h;
        };
        bool IsInside(Point p){
            return p.x >= x && p.x <= x+w && p.y >= y && p.y <= y+h;
        };
        /**
            @return if the given Point are inside the rect
        */
        bool IsInside(GenericRect<int> r){
            return r.x >= x && r.x <= x+w && r.y >= y && r.y <= y+h;
        };
        bool IsInside(GenericRect<float> r){
            return r.x >= x && r.x <= x+w && r.y >= y && r.y <= y+h;
        };
        bool IsInside(GenericRect<double> r){
            return r.x >= x && r.x <= x+w && r.y >= y && r.y <= y+h;
        };
        /*
            Given an rect, it aligns the THIS rect in the center of the parameter rect
        */
        void CenterRectIn(GenericPoint<T> center){
            auto thisc = GetCenter();
            center.x -= thisc.x;
            center.y -= thisc.y;
            x += center.x;
            y += center.y;
        }

        void CenterRectIn(GenericRect<T> r){
            GenericPoint<T> oterc = r.GetCenter();
            CenterRectIn(oterc);
        }

        void GridSnap(int gridSize,bool onSize=true){
            x -= (int)x%gridSize;
            y -= (int)y%gridSize;
            if (onSize){
                w -= (int)x%gridSize;
                h -= (int)y%gridSize;
            }
        }


        /**
            @return A Point containing in x,y the middle position of the rect
        */
        GenericPoint<T> GetCenter(){return GenericPoint<T>(x+(w/2.0),y+(h/2.0));}
        T x,y,w,h;

        uint32_t GetSize(){return std::max(w,h);};
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
        Circle();
        Circle(float,float,float);
        virtual ~Circle()=default;
        Circle(const float n[3]):Circle(){
            x = n[0];
            y = n[1];
            r = n[2];
        }

        Point GetPoint(){return Point(x,y);};
        float x,y,r;
        float GetSize(){return 2.0f * r;};
};

class Cone : public Circle{
    public:
        Cone():Circle(),start_angle(0.0f),opening(0.0f){};
        /**
            @param x_ the X position
            @param y_ the Y position
            @param radius_ Radius of the circle
            @param start_angle_ The angle of the begin
            @param opening_ the amount of angles in degree of opening
        */
        Cone(float x_,float y_,float radius_,float start_angle_,float opening_):Cone(){
            x = x_;
            y = y_;
            r = radius_;
            opening =opening_;
            start_angle = start_angle_;
        };

        float start_angle,opening;
		float GetSize(){return 2.0f * r;};
};



#endif
