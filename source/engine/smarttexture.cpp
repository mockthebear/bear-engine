#include "smarttexture.hpp"
#include "gamebase.hpp"
#include "renderhelp.hpp"

SmartTexture::SmartTexture(int xx,int yy,int ww,int hh,bool del,bool hasAliasing){
   h = hh;
   w = ww;
   deleteTexture = del;
   pixels = nullptr;



    #ifndef RENDER_OPENGL
    if (hasAliasing)
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY,"1");
    t = SDL_CreateTexture( BearEngine->GetRenderer(),SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STATIC, ww, hh);
    if (hasAliasing)
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY,"0");
    SDL_SetTextureBlendMode( t, SDL_BLENDMODE_BLEND );
    #else
    t = std::shared_ptr<BearTexture>(RenderHelp::CreateTexture(ww,hh,TEXTURE_TRILINEAR),[](BearTexture* bher)
            {
                bher->ClearTexture();
                delete bher;
            });
    #endif // RENDER_OPENGL

   pixels = new Uint32[w * h];

   x = xx;
   y = yy;
}



void SmartTexture::UpdateTexture(){
    #ifndef RENDER_OPENGL
    SDL_UpdateTexture(t, NULL, pixels, w * sizeof(Uint32));
    #else
    glBindTexture(GL_TEXTURE_2D, t->id);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, t->size_w, pow_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, t->size_w, t->size_h, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    glBindTexture(GL_TEXTURE_2D, 0);
    #endif // RENDER_OPENGL

}
SmartTexture::~SmartTexture(){
    delete []pixels;
    if (deleteTexture){
        //SDL_DestroyTexture( t );
    }
}
void SmartTexture::Render(PointInt pos,float angle,Point scale){
    RenderHelp::RenderTexture(t.get(),pos,Rect(0,0,-1,-1),angle,scale);
}
