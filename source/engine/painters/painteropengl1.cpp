#ifdef RENDER_OPENGL
#include "painters.hpp"
#include "../bear.hpp"

bool Painter::RenderTexture(BearTexture *t_texture, RenderData &t_data){
    if (!t_texture){
        return false;
    }

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

    glBindTexture( GL_TEXTURE_2D, t_texture->id );

    if ((t_data.flip&SDL_FLIP_HORIZONTAL) != 0){
        float holder =  texLeft;
        texLeft = texRight;
        texRight = holder;
    }
    if ((t_data.flip&SDL_FLIP_VERTICAL) != 0){
        float holder =  texTop;
        texTop = texBottom;
        texBottom = holder;
    }

    glBegin( GL_TRIANGLE_FAN );
        glTexCoord2f(  texLeft,    texTop ); glVertex2f( -quadWidth / 2.f, -quadHeight / 2.f );
        glTexCoord2f( texRight ,    texTop ); glVertex2f(  quadWidth / 2.f, -quadHeight / 2.f );
        glTexCoord2f( texRight , texBottom ); glVertex2f(  quadWidth / 2.f,  quadHeight / 2.f );
        glTexCoord2f(  texLeft , texBottom ); glVertex2f( -quadWidth / 2.f,  quadHeight / 2.f );
    glEnd();

    return true;
}

BearTexture* Painter::MakeTexture(PointInt size,int mode,unsigned char* pixels,TextureLoadMethod &filter){
    if (size.x == 0 || size.y == 0){
        return nullptr;
    }
    GLuint texId = 0;
    glGenTextures(1, &texId);
    if (texId == 0){
        return nullptr;
    }

    unsigned int pow_w =  powerOfTwo(size.x);
    unsigned int pow_h =  powerOfTwo(size.y);

    glBindTexture(GL_TEXTURE_2D, texId);
    filter.ApplyFilter();
    glTexImage2D(GL_TEXTURE_2D, 0, mode, pow_w, pow_h, 0, mode, GL_UNSIGNED_BYTE, nullptr);
    if (pixels != nullptr){
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, size.x, size.y, mode, GL_UNSIGNED_BYTE, pixels);
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    BearTexture *ret = new BearTexture(texId,pow_w,pow_h,mode);
    ret->textureMode = filter;
    ret->size_w = size.x;
    ret->size_h = size.y;
    return ret;
}




#endif // RENDER_OPENGL
