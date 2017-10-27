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
        #ifndef RENDER_OPENGL
        SmartTexture(SDL_Texture *tex,Uint32 *p,int xx,int yy,int hh,int ww):t(tex),pixels(p),h(hh),w(ww),x(xx),y(yy){deleteTexture=false;};
        #else
        SmartTexture(BearTexture *tex,Uint32 *p,int xx,int yy,int hh,int ww):t(tex),pixels(p),h(hh),w(ww),x(xx),y(yy){deleteTexture=false;};
        #endif // RENDER_OPENGL
        void CallLambda(std::function<void (Uint32 *, int, int)> F){ F(pixels,h,w);};
        ~SmartTexture();
        void Render(PointInt pos,float rotation=0,Point scale=Point(1.0f,1.0f));
        void UpdateTexture();
        Uint32 *GetPixels(){return pixels;};
        #ifndef RENDER_OPENGL
        SDL_Texture * GetTexture(){return t;};
        #else
        BearTexture * GetTexture(){return t.get();};
        #endif // RENDER_OPENGL
        int getW(){return w;};
        int getH(){return h;};
        inline int FormatXY(int x_,int y_){return y_*w + x_; };
        Uint32 &At(int x_,int y_){return pixels[y_*w + x_]; };
        //SmartTexture& operator=(SmartTexture T) = 0;
    private:
        bool deleteTexture;
        #ifndef RENDER_OPENGL
        SDL_Texture* t;
        bool aliasing;
        #else
        std::shared_ptr<BearTexture> t;
        #endif // RENDER_OPENGL
        Uint32 * pixels;
        int h,w;
        int x,y;

};


#endif //
