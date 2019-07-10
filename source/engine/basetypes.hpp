#pragma once
#include "libheader.hpp"
#include "../framework/geometry.hpp"
#include "../framework/debughelper.hpp"
#include "../framework/chainptr.hpp"

typedef class BearColor{
    public:
        BearColor():r(1.0f),g(1.0f),b(1.0f),a(1.0f){};
        BearColor(float f1):r(f1),g(f1),b(f1),a(f1){};
        BearColor(float f1,float f2,float f3,float f4):r(f1),g(f2),b(f3),a(f4){};
        BearColor(double f1,double f2,double f3,double f4):r(f1),g(f2),b(f3),a(f4){};
        BearColor(uint8_t f1,uint8_t f2,uint8_t f3,uint8_t f4):r(f1/255.0f),g(f2/255.0f),b(f3/255.0f),a(f4/255.0f){};
        BearColor(int f1,int f2,int f3,int f4):r(f1/255.0f),g(f2/255.0f),b(f3/255.0f),a(f4/255.0f){};
        float r,g,b,a;
        float& operator[](int x){
            switch (x){
                case 0:
                    return r;
                case 1:
                    return g;
                case 2:
                    return b;
                case 3:
                    return a;
                default:
                    return a;
            }
        }
        float *Get4fv(){
            return &r;
        }
} BearColor;

extern const BearColor BCWhite4[4];
extern const BearColor BCWhite2[2];


class Vertex{
    public:
        Vertex():x(0.0f),y(0.0f),clx(-1.0f),cly(-1.0f),r(1.0f),g(1.0f),b(1.0f),a(1.0f){};
        Vertex(Point pos):x(pos.x),y(pos.y),clx(-1.0f),cly(-1.0f),r(1.0f),g(1.0f),b(1.0f),a(1.0f){};
        Vertex(Point pos, BearColor bc):x(pos.x),y(pos.y),clx(-1.0f),cly(-1.0f),r(bc.r),g(bc.g),b(bc.b),a(bc.a){};
        Vertex(Point pos, Point clip):x(pos.x),y(pos.y),clx(clip.x),cly(clip.y),r(1.0f),g(1.0f),b(1.0f),a(1.0f){};
        Vertex(Point pos, Point clip, BearColor bc):x(pos.x),y(pos.y),clx(clip.x),cly(clip.y),r(bc.r),g(bc.g),b(bc.b),a(bc.a){};


        static void SetAttributes();

        float *begin(){
            return &x;
        }

        float *end(){
            return (&x) + GetSize();
        }
        static inline int GetSize(){
            return sizeof(Vertex);
        }

        float x,y,clx,cly,r,g,b,a;
};


class RectColor : public Rect{
    public:
        RectColor():Rect(),colors(BCWhite4),angle(0.0f){};
        RectColor(Rect r,const BearColor cols[4] = BCWhite4, float ang = 0.0f):Rect(r),colors(BCWhite4),angle(ang){
            for (int i=0;i<4;i++)
                colors[i] = cols[i];
        };
        RectColor(Rect r,const BearColor col, float ang = 0.0f):Rect(r),colors(BCWhite4),angle(ang){
            SetColor(col);
        };

        RectColor(float _x, float _y, float _w, float _h, int r, int g, int b, int a = 255,float ang = 0.0f):Rect(_x,_y,_w,_h),colors(BCWhite4),angle(ang){
            colors[0] = colors[1] = colors[2] = colors[3] = BearColor(r,g,b,a);
        };

        RectColor(float _x, float _y, float _w, float _h, BearColor co ,float ang = 0.0f):Rect(_x,_y,_w,_h),colors(BCWhite4),angle(ang){
            colors[0] = colors[1] = colors[2] = colors[3] = co;
        };

        void SetColor(const BearColor &col){
            for (int i=0;i<4;i++)
                colors[i] = col;
        }
    BearColor colors[4];
    float angle;
};

class PointColor : public Point{
    public:
        PointColor():Point(),color(){};
        PointColor(float _x, float _y):Point(_x,_y),color(){};
        PointColor(Point p):Point(p),color(){};
        PointColor(Point p, BearColor c):Point(p),color(c){};
        PointColor(float _x, float _y, BearColor c):Point(_x,_y),color(c){};
        PointColor(float _x, float _y, uint8_t r, uint8_t g, uint8_t b, uint8_t a=255):Point(_x,_y),color(r,g,b,a){};
        PointColor(Point p, uint8_t r, uint8_t g, uint8_t b, uint8_t a=255):Point(p),color(r,g,b,a){};

        BearColor color;
};

class LineColor : public Line{
    public:
        LineColor():Line(),color1(),color2(){};
        LineColor(Point p1, Point p2):Line(p1,p2),color1(),color2(){};
        LineColor(Point p1, Point p2, BearColor c):Line(p1,p2),color1(c),color2(c){};
        LineColor(Point p1, Point p2, BearColor c1, BearColor c2):Line(p1,p2),color1(c1),color2(c2){};
        LineColor(float _x1, float _y1, float _x2, float _y2):Line(_x1, _y1, _x2, _y2),color1(),color2(){};
        LineColor(float _x1, float _y1, float _x2, float _y2, BearColor c):Line(_x1, _y1,_x2, _y2),color1(c),color2(c){};
        LineColor(float _x1, float _y1, float _x2, float _y2, BearColor c1, BearColor c2):Line(_x1, _y1, _x2, _y2),color1(c1),color2(c2){};
        LineColor(float _x1, float _y1, float _x2, float _y2, uint8_t r,uint8_t g, uint8_t b, uint8_t a):Line(_x1, _y1, _x2, _y2),color1(r,g,b,a),color2(r,g,b,a){};
        BearColor color1;
        BearColor color2;
};

class CircleColor : public Circle{
    public:
        CircleColor():Circle(),color1(),color2(){};
        CircleColor(Circle c):Circle(c),color1(),color2(){};
        CircleColor(Point p, float radius):Circle(p.x, p.y, radius),color1(),color2(){};
        CircleColor(Circle c,BearColor c1):Circle(c),color1(c1),color2(c1){};
        CircleColor(Point p, float radius,BearColor c1):Circle(p.x, p.y, radius),color1(c1),color2(c1){};
        CircleColor(Circle c,BearColor c1,BearColor c2):Circle(c),color1(c1),color2(c2){};
        CircleColor(Point p, float radius,BearColor c1,BearColor c2):Circle(p.x, p.y, radius),color1(c1),color2(c2){};


        CircleColor(float _x, float _y, float rad, uint8_t red,uint8_t g, uint8_t b, uint8_t a=255):Circle(_x,_y,rad),color1(red,g,b,a),color2(red,g,b,a){};
        CircleColor(int _x, int _y, int rad, uint8_t red,uint8_t g, uint8_t b, uint8_t a=255):Circle(_x,_y,rad),color1(red,g,b,a),color2(red,g,b,a){};


        BearColor color1;
        BearColor color2;
};





enum TextureLoadMethodEnum{
    TEXTURE_DEFAULT,
    TEXTURE_NEAREST,
    TEXTURE_LINEAR,
    TEXTURE_TRILINEAR,
};


class TextureLoadMethod{
    public:
    static TextureLoadMethod DefaultLoadingMethod;
    TextureLoadMethod():mode(TEXTURE_NEAREST){};
    TextureLoadMethod(TextureLoadMethodEnum md):mode(md){};
    void ApplyFilter(){
        switch (mode){
            case TEXTURE_NEAREST:
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
                DisplayGlError("TEXTURE_NEAREST: 1");
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
                DisplayGlError("TEXTURE_NEAREST: 2");
                break;
            case TEXTURE_LINEAR:
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
                DisplayGlError("TEXTURE_LINEAR: 1");
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
                DisplayGlError("TEXTURE_LINEAR: 2");
                break;
            case TEXTURE_TRILINEAR:
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                DisplayGlError("TEXTURE_TRILINEAR: 1");
                //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                DisplayGlError("TEXTURE_TRILINEAR: 2");
                break;
            case TEXTURE_DEFAULT:
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
                DisplayGlError("TEXTURE_DEFAULT: 1");
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
                DisplayGlError("TEXTURE_DEFAULT: 2");
                break;
            default:
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
                DisplayGlError("?: 1");
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
                DisplayGlError("?: 2");
                break;
        }
    }
    TextureLoadMethodEnum mode;
};




class BearTexture{
    public:
        BearTexture():id(0),texture_w(0),texture_h(0),size_w(0),size_h(0),textureMode(TEXTURE_NEAREST),mode(GL_RGBA),GLimageId(0){};

        uint32_t DropTexture();
        void ClearTexture();
        void ApplyPixels(uint8_t *pixels);

        BearTexture(uint32_t textureId,uint32_t imagewidth,uint32_t imageheight,uint32_t texturewidth,uint32_t textureheight,GLenum imgMode)
        :id(textureId),texture_w(texturewidth),texture_h(textureheight),size_w(imagewidth),size_h(imageheight),textureMode(TEXTURE_NEAREST),mode(imgMode){};
        uint32_t id;
        uint32_t texture_w;
        uint32_t texture_h;
        uint32_t size_w;
        uint32_t size_h;
        TextureLoadMethod textureMode;
        GLenum mode;
        uint32_t GLimageId;

};

class BearException{
    public:
        BearException():type(0),severity(0),Error(""){};
        BearException(std::string what):type(1),severity(0),Error(what){};

        void Show();
        uint32_t type;
        uint32_t severity;
        std::string Error;
};


enum PainterSupport{
    SUPPORT_SHADERS = 0,
    SUPPORT_VERTEXBUFFER,
    SUPPORT_ORTOHOVERTEX,
    SUPPORT_POINTSPRITE,
};

typedef chain_ptr<BearTexture> TexturePtr;
