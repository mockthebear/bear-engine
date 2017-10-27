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
    t = std::shared_ptr<BearTexture>(RenderHelp::CreateTexture(ww,hh),[](BearTexture* bher)
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
    #ifndef RENDER_OPENGL

   SDL_Rect rr;
    double scaleRatioW = ScreenManager::GetInstance().GetScaleRatioW(); //floor(ScreenManager::GetInstance().GetScaleRatioH()*32.1)/32.1
    double scaleRatioH = ScreenManager::GetInstance().GetScaleRatioH(); //floor(ScreenManager::GetInstance().GetScaleRatioH()*32.1)/32.1
    rr.x = pos.x*scaleRatioW + ScreenManager::GetInstance().GetOffsetW();
    rr.y = pos.y*scaleRatioH + ScreenManager::GetInstance().GetOffsetH();
    rr.h = h*scaleRatioH;
    rr.w = w*scaleRatioW;
    SDL_RenderCopyEx(BearEngine->GetRenderer(),t,NULL,&rr,angle,nullptr,SDL_FLIP_NONE);
    #else
    glLoadIdentity();
    glEnable(GL_TEXTURE_2D);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);


    GLfloat texLeft = 0.0f;
    GLfloat texRight = 1.0f;
    GLfloat texTop = 0.0f;
    GLfloat texBottom = 1.0f;

    GLfloat quadWidth =  t->size_w ;
    GLfloat quadHeight = t->size_h ;


    texRight = (  t->size_w ) / (float)t->w;
    texBottom = ( t->size_h ) / (float)t->h;


    glTranslatef(
                    (pos.x   + quadWidth / 2.f  ),
                    (pos.y  + quadHeight/ 2.f  ),
                    0.f );

    glScalef(scale.x,scale.y, 1.0f);
    glRotatef( angle, 0.f, 0.f, 1.f );

    glBindTexture( GL_TEXTURE_2D, t->id );

    glBegin( GL_QUADS );
        glTexCoord2f(  texLeft,    texTop ); glVertex2f( -quadWidth / 2.f, -quadHeight / 2.f );
        glTexCoord2f( texRight ,    texTop ); glVertex2f(  quadWidth / 2.f, -quadHeight / 2.f );
        glTexCoord2f( texRight , texBottom ); glVertex2f(  quadWidth / 2.f,  quadHeight / 2.f );
        glTexCoord2f(  texLeft , texBottom ); glVertex2f( -quadWidth / 2.f,  quadHeight / 2.f );
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
    #endif // RENDER_OPENGL
}
