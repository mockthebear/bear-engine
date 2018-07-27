#include "smarttexture.hpp"
#include "gamebase.hpp"
#include "renderhelp.hpp"

SmartTexture::SmartTexture(int xx,int yy,int ww,int hh,bool del,bool hasAliasing){
   h = hh;
   w = ww;
   deleteTexture = del;
   pixels = nullptr;


    TextureLoadMethod alias(TEXTURE_TRILINEAR);
    t = std::shared_ptr<BearTexture>(Painter::MakeTexture(PointInt(ww,hh),GL_RGBA,nullptr,alias),[](BearTexture* bher)
            {
                bher->ClearTexture();
                delete bher;
            });

   pixels = new Uint32[w * h];

   x = xx;
   y = yy;
}



void SmartTexture::UpdateTexture(){
    glBindTexture(GL_TEXTURE_2D, t->id);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, t->size_w, pow_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, t->size_w, t->size_h, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    glBindTexture(GL_TEXTURE_2D, 0);
}
SmartTexture::~SmartTexture(){
    delete []pixels;
    if (deleteTexture){
        //SDL_DestroyTexture( t );
    }
}
void SmartTexture::Render(PointInt pos,float angle,Point scale){


    /*glLoadIdentity();
    glEnable(GL_TEXTURE_2D);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);


    GLfloat texLeft = 0.0f;
    GLfloat texRight = 1.0f;
    GLfloat texTop = 0.0f;
    GLfloat texBottom = 1.0f;

    GLfloat quadWidth =  t->size_w ;
    GLfloat quadHeight = t->size_h ;


    texRight = (  t->size_w ) / (float)t->texture_w;
    texBottom = ( t->size_h ) / (float)t->texture_h;

    glScalef(scale.x,scale.y, 1.0f);
    glTranslatef(
                    (pos.x   + quadWidth / 2.f  ),
                    (pos.y  + quadHeight/ 2.f  ),
                    0.f );


    glRotatef( angle, 0.f, 0.f, 1.f );

    glBindTexture( GL_TEXTURE_2D, t->id );

    glBegin( GL_TRIANGLE_FAN );
        glTexCoord2f(  texLeft,    texTop ); glVertex2f( -quadWidth / 2.f, -quadHeight / 2.f );
        glTexCoord2f( texRight ,    texTop ); glVertex2f(  quadWidth / 2.f, -quadHeight / 2.f );
        glTexCoord2f( texRight , texBottom ); glVertex2f(  quadWidth / 2.f,  quadHeight / 2.f );
        glTexCoord2f(  texLeft , texBottom ); glVertex2f( -quadWidth / 2.f,  quadHeight / 2.f );
    glEnd();

    glDisable(GL_TEXTURE_2D);*/

}
