#ifdef RENDER_OPENGL
#include "painters.hpp"
#include "../bear.hpp"
#include "../screenmanager.hpp"
#include "../../framework/debughelper.hpp"

bool Painter::RenderTexture(BearTexture *t_texture, RenderData &t_data){
    if (!t_texture){
        return false;
    }

    GLfloat texLeft = t_data.forwardClip.x;
    GLfloat texRight =  t_data.forwardClip.y;
    GLfloat texTop = t_data.forwardClip.w;
    GLfloat texBottom = t_data.forwardClip.h;



    GLfloat quadWidth = t_data.clip.w ;
    GLfloat quadHeight = t_data.clip.h ;

    glLoadIdentity();

    glEnable(GL_TEXTURE_2D);

    glBindTexture( GL_TEXTURE_2D, t_texture->id );

    glColor4fv(t_data.color);




    glScalef(t_data.scale.x , t_data.scale.y , 1.0f);

    glTranslatef(
        (t_data.position.x * (1.0f/t_data.scale.x)  + quadWidth  / 2.f  ) + (- t_data.center.x* (t_data.scale.x)  + t_data.center.x),
        (t_data.position.y * (1.0f/t_data.scale.y)  + quadHeight / 2.f  ) + (- t_data.center.y* (t_data.scale.y)  + t_data.center.y),
    0.f);

    glRotatef( t_data.angle, 0.f, 0.f, 1.f );



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

    glDisable(GL_TEXTURE_2D);
    glBindTexture( GL_TEXTURE_2D, 0 );

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

    unsigned int pow_w =  size.x;//powerOfTwo(size.x);
    unsigned int pow_h =  size.y;//powerOfTwo(size.y); //No need to this on opengl. only on opengles

    glBindTexture(GL_TEXTURE_2D, texId);
    filter.ApplyFilter();
    glTexImage2D(GL_TEXTURE_2D, 0, mode, pow_w, pow_h, 0, mode, GL_UNSIGNED_BYTE, nullptr);
    if (pixels != nullptr){
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, size.x, size.y, mode, GL_UNSIGNED_BYTE, pixels);
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    BearTexture *ret = new BearTexture(texId,size.x,size.y,pow_w,pow_h,mode);
    ret->textureMode = filter;
    return ret;
}


bool Painter::SetupEnvoriment(ScreenManager *sm){
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetSwapInterval(0);

    glewExperimental = GL_TRUE;
    GLenum glewError = glewInit();
    if( glewError != GLEW_OK )
    {
        bear::out << "Error initializing GLEW!\n";
        return false;
    }


    DebugHelper::DisplayGlError("1");

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	SDL_GL_SwapWindow(sm->m_window);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    PointInt originalScreen = sm->GetGameSize();
	glViewport( 0.f, 0.f, originalScreen.x, originalScreen.y );
	DebugHelper::DisplayGlError("2");
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho( 0.0, originalScreen.x, originalScreen.y, 0.0, 1.0, -1.0 );
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    glPushMatrix();
    glClearColor( 1.f, 1.f, 1.f, 1.f );
    DebugHelper::DisplayGlError("3");

    return true;

}

int Painter::GetMaxTextureSize(){
    int maxSize;
	glGetIntegerv (GL_MAX_TEXTURE_SIZE, &maxSize);
    return maxSize;
}

void Painter::ResetViewPort(PointInt originalSize, PointInt newSize){
    glViewport(0,0,newSize.x, newSize.y);
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho( 0.0, originalSize.x, originalSize.y, 0.0, 1.0, -1.0 );
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
}

#endif // RENDER_OPENGL
