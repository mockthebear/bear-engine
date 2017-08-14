#ifndef SMTXH
#define SMTXH
#include "../settings/definitions.hpp"
#include "../framework/geometry.hpp"

#include SDL_LIB_HEADER
#include <stdint.h>
#include <functional>
class SmartTexture{
    public:
        SmartTexture(int x,int y,int w,int h,bool deleteTexture=false,bool hasAliasing=false);
        SmartTexture(SDL_Texture *tex,Uint32 *p,int xx,int yy,int hh,int ww):t(tex),pixels(p),h(hh),w(ww),x(xx),y(yy){deleteTexture=false;};
        void CallLambda(std::function<void (Uint32 *, int, int)> F){ F(pixels,h,w);};
        ~SmartTexture();
        void Render(PointInt pos,float rotation=0);
        void UpdateTexture();
        Uint32 *GetPixels(){return pixels;};
        SDL_Texture * GetTexture(){return t;};
        int getW(){return w;};
        int getH(){return h;};
        inline int FormatXY(int x_,int y_){return y_*w + x_; };
        Uint32 &At(int x_,int y_){return pixels[y_*w + x_]; };
        //SmartTexture& operator=(SmartTexture T) = 0;
    private:
        bool deleteTexture;
        SDL_Texture *t;
        Uint32 * pixels;
        int h,w;
        int x,y;

};


class SmartSurface
{
	public:
		SmartSurface();
		SmartSurface(std::string path);
		~SmartSurface();
		bool loadFromFile( std::string path );
		void free();
		void setColor( Uint8 red, Uint8 green, Uint8 blue );
		void setBlendMode( SDL_BlendMode blending );
		void setAlpha( Uint8 alpha );
		void Render( int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE );
		int getWidth(){return w;};
		int getHeight(){return h;};
		bool lockTexture();
		bool unlockTexture();
		void* getPixels(){return mPixels;};
		int getPitch(){return pitch;};
		Uint32 getPixel32( unsigned int x, unsigned int y );
	private:
		SDL_Texture* texture;
		void* mPixels;
		int pitch;
		int w,h;
};



#endif //
