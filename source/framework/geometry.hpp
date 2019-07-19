#ifndef GEOMH
#define GEOMH
#include <math.h>
#include <stdio.h>
#include <cstdint>
#include <algorithm>
#include <float.h>
#include "../settings/definitions.hpp"
#include SDL_LIB_HEADER


/**
    @brief [WIP] Class to deal with angles and geometry stuff
*/
class Geometry{
    public:
        /**
            Receive an angle in degree and convert to rad
        */
        static double toRad(double v){ return v* PI() / 180.0; };
        /**
            Receive an angle in rad and convert to degree
        */
        static double toDeg(double v){ return (v < 0 ? 2.0*PI() + v : v) * 180.0 / PI(); };
        /**
            PI :V
        */
        static inline double PI(){ return 3.14159265;};
        static double AlignRad(double v){
            double twoPi = 2.0*PI();
            while (v < 0.0){
                v = twoPi + v;
            }
            while (v > twoPi){
                v = v - twoPi;
            }
            return v;
        };
};

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
        template<typename T2> GenericPoint(GenericPoint<T2> p):x(p.x),y(p.y){};

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
        T GetDistance(GenericPoint p){
             return sqrt(pow((x - p.x),(T)2) + pow((y-p.y),(T)2));
        };


        T GetDistanceSquared(GenericPoint p){
             return (pow((x - p.x),(T)2) + pow((y-p.y),(T)2));
        };
        /**
            Get the direction in rads between two points
            @param return in radians the angle
        */
        T GetDirection(GenericPoint&& to){
            return atan2(to.y-y,to.x-x);
        };
        T GetDirection(GenericPoint& to){
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
        bool IsValid(){
            if (x == FLT_MAX || y == FLT_MAX)
                return false;
            return true;
        }

        GenericPoint<T> GetAgainstPosition(GenericPoint<T> other){
            GenericPoint<T> pf;
            if (x > other.x){
                pf.x = 1;
            }else if (x < other.x){
                pf.x = -1;
            }else{
                pf.x = 0;
            }
            if (y > other.y){
                pf.y = 1;
            }else if (y < other.y){
                pf.y = -1;
            }else{
                pf.y = 0;
            }
            return pf;
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
template <typename T=float,typename T2=float> bool operator==(const GenericPoint<T> &a, const GenericPoint<T2> &b){
    return a.x == b.x && a.y == b.y;
}

template <typename T=float,typename T2=float> bool operator!=(const GenericPoint<T> &a, const GenericPoint<T2> &b){
    return !(a.x == b.x && a.y == b.y);
}

template <typename T=float,typename T2=float> GenericPoint<T2> operator/(T a,GenericPoint<T2> b){
    return GenericPoint<T2>(a/b.x,a/b.y);
}

template <typename T=float,typename T2=float> GenericPoint<T2> operator/(GenericPoint<T> a,GenericPoint<T2> b){
    return GenericPoint<T2>(a.x/b.x,a.y/b.y);
}


/**
    @brief [WIP] Basic class to 3d points
*/
template <typename T=float> class GenericPoint3{
    public:
        GenericPoint3():x(0),y(0),z(0){};
        GenericPoint3(T x_,T y_,T z_):x(x_),y(y_),z(z_){};

        virtual ~GenericPoint3() = default;

        template <typename T2> GenericPoint3(GenericPoint<T2> obj1, T a){
            x = obj1.x;
            y = obj1.y;
            z = a;
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

typedef GenericPoint3<int> PointInt3;

template <typename T=float>class GenericPolygon{
    public:
        GenericPolygon(){};

        void Add(GenericPoint<T> p){
            vertices.emplace_back(p);
        }

        bool IsInside(GenericPoint<T> p);

        uint32_t GetVerticesCount(){ return vertices.size(); };

        std::vector<GenericPoint<T>> vertices;


};


template <typename T=float>class GenericLine{
    public:
    GenericLine(T xx,T yy ,T ww,T hh):x1(xx),y1(yy),x2(ww),y2(hh){}
        /**
            Empty constructor start all components with 0
        */

        GenericLine(T val){
            x1 = val;
            y1 = val;
            x2 = val;
            y2 = val;
        };


        template<typename K>GenericLine(GenericLine<K> reqt){
            x1 = reqt.x1;
            y1 = reqt.y1;
            y2 = reqt.y2;
            x2 = reqt.x2;
        };

        template<typename K, typename K2> GenericLine(GenericPoint<K> p1, GenericPoint<K2> p2){
            x1 = p1.x;
            y1 = p1.y;
            x2 = p2.x;
            y2 = p2.y;
        };

        template<typename K>GenericLine(GenericPoint<K> p, T _w, T _h){
            x1 = p.x;
            y1 = p.y;
            x2 = _w;
            y2 = _h;
        };

        GenericLine(const int n[4]){
            x1 = n[0];
            y1 = n[1];
            x1 = n[2];
            y2 = n[3];
        };
        GenericLine(const float n[4]){
            x1 = n[0];
            y1 = n[1];
            x2 = n[2];
            y2 = n[3];
        };
        GenericLine(const long n[4]){
            x1 = n[0];
            y1 = n[1];
            x2 = n[2];
            y2 = n[3];
        };

        GenericLine():x1(0),y1(0),x2(0),y2(0){};

         GenericPoint<T> GetFirst(){
            return GenericPoint<T>(x1, y1);
        }

        GenericPoint<T> GetSecond(){
            return GenericPoint<T>(x2, y2);
        };

        static bool onSegment(GenericPoint<T> p, GenericPoint<T> q, GenericPoint<T> r){
            if (q.x <= std::max(p.x, r.x) && q.x >= std::min(p.x, r.x) &&
                    q.y <= std::max(p.y, r.y) && q.y >= std::min(p.y, r.y))
                return true;
            return false;
        };

        static int orientation(GenericPoint<T> p, GenericPoint<T> q, GenericPoint<T> r){
            int val = (q.y - p.y) * (r.x - q.x) -
                        (q.x - p.x) * (r.y - q.y);
            if (val == 0) return 0;
            return (val > 0)? 1: 2;
        };

        GenericPoint<T> GetIntersection(GenericLine<T> l2){
            T s02_x, s02_y, s10_x, s10_y, s32_x, s32_y, s_numer, t_numer, denom, t;
            s10_x = x2 - x1;
            s10_y = y2 - y1;
            s32_x = l2.x2 - l2.x1;
            s32_y = l2.y2 - l2.y1;

            denom = s10_x * s32_y - s32_x * s10_y;
            if (fabs(denom) <= 0.001)
                return GenericPoint<T>(FLT_MAX, FLT_MAX); // Collinear
            bool denomPositive = denom > 0;

            s02_x = x1 - l2.x1;
            s02_y = y1 - l2.y1;
            s_numer = s10_x * s02_y - s10_y * s02_x;
            if ((s_numer <= 0) == denomPositive)
                return GenericPoint<T>(FLT_MAX, FLT_MAX); // No collision

            t_numer = s32_x * s02_y - s32_y * s02_x;
            if ((t_numer <= 0) == denomPositive)
                return GenericPoint<T>(FLT_MAX, FLT_MAX); // No collision

            if (((s_numer > denom) == denomPositive) || ((t_numer > denom) == denomPositive))
                return GenericPoint<T>(FLT_MAX, FLT_MAX); // No collision
            // Collision detected
            t = t_numer / denom;

            return GenericPoint<T>(x1 + (t * s10_x), y1 + (t * s10_y));
        }

        bool IsIntersect(GenericLine<T> l2){
            int o1 = orientation(GetFirst(), GetSecond(), l2.GetFirst());
            int o2 = orientation(GetFirst(), GetSecond(), l2.GetSecond());
            int o3 = orientation(l2.GetFirst(), l2.GetSecond(), GetFirst());
            int o4 = orientation(l2.GetFirst(), l2.GetSecond(), GetSecond());

            // General case
            if (o1 != o2 && o3 != o4)
                return true;

            // Special Cases
            // p1, q1 and p2 are colinear and p2 lies on segment p1q1
            if (o1 == 0 && onSegment(GetFirst(), l2.GetFirst(), GetSecond())) return true;

            // p1, q1 and p2 are colinear and q2 lies on segment p1q1
            if (o2 == 0 && onSegment(GetFirst(), l2.GetSecond(), GetSecond())) return true;

            // p2, q2 and p1 are colinear and p1 lies on segment p2q2
            if (o3 == 0 && onSegment(l2.GetFirst(), GetFirst(), l2.GetSecond())) return true;

             // p2, q2 and q1 are colinear and q1 lies on segment p2q2
            if (o4 == 0 && onSegment(l2.GetFirst(), GetSecond(), l2.GetSecond())) return true;

            return false; // Doesn't fall in any of the above cases
        };

        T x1, y1, x2, y2;
};

template <typename T=float,typename T2=float> bool operator==(const GenericLine<T> &a, const GenericLine<T2> &b){
    return a.x1 == b.x1 && a.y1 == b.y1  && a.x2 == b.x2 && a.y2 == b.y2;
}

template <typename T=float,typename T2=float> bool operator!=(const GenericLine<T> &a, const GenericLine<T2> &b){
    return !(a.x1 == b.x1 && a.y1 == b.y1 && a.x2 == b.x2 && a.y2 == b.y2);
}

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

        GenericRect(T val){
            x = val;
            y = val;
            h = val;
            w = val;
        };


        template<typename K>GenericRect(GenericRect<K> reqt){
            x = reqt.x;
            y = reqt.y;
            h = reqt.h;
            w = reqt.w;
        };

        template<typename K, typename K2> GenericRect(GenericPoint<K> p1, GenericPoint<K2> p2){
            x = p1.x;
            y = p1.y;
            w = p2.x;
            h = p2.y;
        };

        template<typename K>GenericRect(GenericPoint<K> p, T _w, T _h){
            x = p.x;
            y = p.y;
            w = _w;
            h = _h;
        };

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
            return (*this);
        };
        GenericRect<T> operator/(const float& p){
            x /= p;
            y /= p;
            w /= p;
            h /= p;
            return *this;
        };

        GenericRect<T> operator=(SDL_Rect r){
            w = r.w;
            x = r.x;
            h = r.h;
            y = r.y;
            return *this;
        }

        GenericRect<T> operator+(GenericRect<T> r){
            w += r.w;
            x += r.x;
            h += r.h;
            y += r.y;
            return *this;
        }


        SDL_Rect operator<<(const SDL_Rect& rhs) const {
            rhs.w = w;
            rhs.x = x;
            rhs.h = h;
            rhs.y = y;
            return rhs;
        }

        GenericPolygon<T> Polygon(){
            GenericPolygon<T> p;
            p.Add(GenericPoint<T>(x,y));
            p.Add(GenericPoint<T>(x+w,y));
            p.Add(GenericPoint<T>(x+w,y+h));
            p.Add(GenericPoint<T>(x,y+h));
            return p;
        }

        std::vector<GenericLine<T>> Lines(){
            std::vector<GenericLine<T>> l;
            l.emplace_back( GenericLine<T>(x    ,y  ,x+w    , y     )    );
            l.emplace_back( GenericLine<T>(x+w  ,y  ,x+w    , y+h   )    );
            l.emplace_back( GenericLine<T>(x+w  ,y+h,x      , y+h   )    );
            l.emplace_back( GenericLine<T>(x    ,y+h,x      , y     )    );
            return l;
        };

        std::vector<GenericPoint<T>> Points(){
            std::vector<GenericPoint<T>> l;
            l.emplace_back( GenericPoint<T>(x    ,  y   ));
            l.emplace_back( GenericPoint<T>(x+w  ,  y   ));
            l.emplace_back( GenericPoint<T>(x+w  ,  y+h ));
            l.emplace_back( GenericPoint<T>(x    ,  y+h ));

            return l;
        };


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

        void PositionSized(){
            w += x;
            h += y;
        }


        /**
            @return A Point containing in x,y the middle position of the rect
        */
        GenericPoint<T> GetCenter(){return GenericPoint<T>(x+(w/2.0),y+(h/2.0));};
        GenericPoint<T> GetSize(){return GenericPoint<T>(w,h);};
        T x,y,w,h;

};

typedef GenericRect<float> Rect;
typedef GenericRect<int> RectInt;


template <typename T=float,typename T2=float> bool operator==(const GenericRect<T> &a, const GenericRect<T2> &b){
    return a.x == b.x && a.y == b.y && a.w == b.w && a.h == b.h;
}


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


typedef GenericPolygon<float> BearPolygon;



typedef GenericLine<float> Line;
#endif
