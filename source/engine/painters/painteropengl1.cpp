#ifdef RENDER_OPENGL
#include "painters.hpp"
#include "../bear.hpp"

bool Painter::RenderTexture(TexturePtr &t_texture, RenderData &t_data){


    GLfloat texLeft;
    GLfloat texRight;
    GLfloat texTop;
    GLfloat texBottom;

    glLoadIdentity();
    glEnable(GL_TEXTURE_2D);

    glColor4fv(t_data.color);

    float w = t_data.size.x;
    float h = t_data.size.y;

    texLeft = t_data.clip.x / (float)w;
    texRight =  ( t_data.clip.x + t_data.clip.w ) / (float)w;
    texTop = t_data.clip.y / (float)h;
    texBottom = ( t_data.clip.y + t_data.clip.h ) / (float)h;

    GLfloat quadWidth = t_data.clip.w ;
    GLfloat quadHeight = t_data.clip.h ;

    glScalef(t_data.scale.x , t_data.scale.y , 1.0f);

    glTranslatef(
        (t_data.position.x * (1.0f/t_data.scale.x)  + quadWidth  / 2.f  ) + (- t_data.center.x* (t_data.scale.x)  + t_data.center.x),
        (t_data.position.y * (1.0f/t_data.scale.y)  + quadHeight / 2.f  ) + (- t_data.center.y* (t_data.scale.y)  + t_data.center.y),
    0.f);

    glRotatef( t_data.angle, 0.f, 0.f, 1.f );

    glBindTexture( GL_TEXTURE_2D, t_texture.get()->id );

    /*if ((t_data.flip&SDL_FLIP_HORIZONTAL) != 0){
        float holder =  texLeft;
        texLeft = texRight;
        texRight = holder;
    }
    if ((t_data.flip&SDL_FLIP_VERTICAL) != 0){
        float holder =  texTop;
        texTop = texBottom;
        texBottom = holder;
    }*/

    glBegin( GL_TRIANGLE_FAN );
        glTexCoord2f(  texLeft,    texTop ); glVertex2f( -quadWidth / 2.f, -quadHeight / 2.f );
        glTexCoord2f( texRight ,    texTop ); glVertex2f(  quadWidth / 2.f, -quadHeight / 2.f );
        glTexCoord2f( texRight , texBottom ); glVertex2f(  quadWidth / 2.f,  quadHeight / 2.f );
        glTexCoord2f(  texLeft , texBottom ); glVertex2f( -quadWidth / 2.f,  quadHeight / 2.f );
    glEnd();

    return true;
}

#endif // RENDER_OPENGL
