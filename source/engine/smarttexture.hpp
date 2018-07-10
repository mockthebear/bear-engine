#ifndef SMTXH
#define SMTXH
#include "../settings/definitions.hpp"
#include "../framework/geometry.hpp"
#include "sprite.hpp"

#include SDL_LIB_HEADER
#include <stdint.h>
#include <functional>
class SmartTexture{
    public:
        SmartTexture(int x,int y,int w,int h,bool deleteTexture=false,bool hasAliasing=false);

        SmartTexture(BearTexture *tex,Uint32 *p,int xx,int yy,int hh,int ww):t(tex),pixels(p),h(hh),w(ww),x(xx),y(yy){deleteTexture=false;};

        void CallLambda(std::function<void (Uint32 *, int, int)> F){ F(pixels,h,w);};
        ~SmartTexture();
        void Render(PointInt pos,float rotation=0,Point scale=Point(1.0f,1.0f));
        void UpdateTexture();
        Uint32 *GetPixels(){return pixels;};

        BearTexture * GetTexture(){return t.get();};

        int getW(){return w;};
        int getH(){return h;};
        inline int FormatXY(int x_,int y_){return y_*w + x_; };
        Uint32 &At(int x_,int y_){return pixels[y_*w + x_]; };
        //SmartTexture& operator=(SmartTexture T) = 0;
    private:
        bool deleteTexture;
        std::shared_ptr<BearTexture> t;
        Uint32 * pixels;
        int h,w;
        int x,y;

};


#endif //
