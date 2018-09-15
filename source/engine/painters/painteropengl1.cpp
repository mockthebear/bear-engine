#ifdef RENDER_OPENGL
#include "painters.hpp"
#include "../bear.hpp"
#include "../screenmanager.hpp"
#include "../../framework/debughelper.hpp"

void RenderData::UpdateVertex(){}

glm::mat4 Painter::Projection;


void Painter::SetViewport(Point screenNow,Point offset){
    glViewport(offset.x, offset.y, screenNow.x, screenNow.y);

}

void BasicRenderData::UpdateModel(){}


void RenderData::SetPosition(Point pos){
    position = pos;
    UpdateVertex();
}

void Painter::SetProjection(Rect rproj){
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(rproj.x, rproj.y, rproj.w, rproj.h, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void Painter::SetProjection(Point size,int flipScreen){
    Point xAxis;
    Point yAxis;
    if (flipScreen&SDL_FLIP_HORIZONTAL){
        xAxis.x = size.x;
        xAxis.y = 0.0f;
    }else{
        xAxis.y = size.x;
        xAxis.x = 0.0f;
    }
    if (flipScreen&SDL_FLIP_VERTICAL){
        yAxis.y = size.x;
        yAxis.x = 0.0f;
    }else{
        yAxis.x = size.y;
        yAxis.y = 0.0f;
    }
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(xAxis.x, xAxis.y, yAxis.x, yAxis.y, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}




bool Painter::CanSupport(PainterSupport sup){
    switch(sup){
        case SUPPORT_SHADERS:
            return true;
        case SUPPORT_VERTEXBUFFER:
            return false;
        case SUPPORT_ORTOHOVERTEX:
            return true;
        default:
            return false;
    }
}

void Painter::DrawVertex(VertexArrayObjectPtr vertexData,BasicRenderDataPtr t_data,int drawMode, bool ignoreModel){
    glLoadIdentity();
    if (!ignoreModel){
        glTranslatef(t_data->position.x, t_data->position.y, 0.0f);
        glRotatef( glm::degrees(t_data->m_angle), 0.f, 0.f, 1.f );
    }
    glBegin( drawMode );
        glColor4fv(t_data->color.Get4fv());
        int i = 0;
        float v1 = 0;
        for (auto &it : vertexData->vertexes.vertexData){
            if (i == 0){
                v1 = it;
                i = 1;
            }else{
                glVertex2f( v1 * t_data->size.x, it * t_data->size.y );
                i = 0;
            }
        }
    glEnd();
}


bool Painter::RenderPointTexture(BearTexture *t_texture, RenderDataPtr t_data){
    return false;

}

RenderData::~RenderData(){};

bool Painter::DrawSprites(int id){
    return false;
}

bool Painter::RenderTexture(BearTexture *t_texture, RenderDataPtr t_data){
    GLfloat texLeft = t_data->m_forwardClip.x;
    GLfloat texRight =  t_data->m_forwardClip.y;
    GLfloat texTop = t_data->m_forwardClip.w;
    GLfloat texBottom = t_data->m_forwardClip.h;

    GLfloat quadWidth = t_data->size.x;
    GLfloat quadHeight = t_data->size.y;

    glLoadIdentity();

    glEnable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture( GL_TEXTURE_2D, t_texture->id );

    glColor4fv(t_data->color.Get4fv());
    glScalef(t_data->m_scale.x , t_data->m_scale.y , 1.0f);

    glTranslatef(
        (t_data->position.x * (1.0f/t_data->m_scale.x)  + quadWidth  / 2.f  ) + (- t_data->center.x* (t_data->m_scale.x)  + t_data->center.x),
        (t_data->position.y * (1.0f/t_data->m_scale.y)  + quadHeight / 2.f  ) + (- t_data->center.y* (t_data->m_scale.y)  + t_data->center.y),
    0.f);

    glRotatef( t_data->m_angle, 0.f, 0.f, 1.f );



    if ((t_data->flip&SDL_FLIP_HORIZONTAL) != 0){
        float holder =  texLeft;
        texLeft = texRight;
        texRight = holder;
    }
    if ((t_data->flip&SDL_FLIP_VERTICAL) != 0){
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

void Painter::SetTexturePixels(uint32_t texture, PointInt size,int mode,unsigned char* pixels ){
    if (!pixels){
        return;
    }
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, size.x, size.y, mode, GL_UNSIGNED_BYTE, pixels);
    glBindTexture(GL_TEXTURE_2D, 0);
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


    DisplayGlError("1");

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	SDL_GL_SwapWindow(sm->m_window);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    PointInt originalScreen = sm->GetGameSize();
	glViewport( 0.f, 0.f, originalScreen.x, originalScreen.y );
	DisplayGlError("2");
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho( 0.0, originalScreen.x, originalScreen.y, 0.0, 1.0, -1.0 );
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    glPushMatrix();
    glClearColor( 1.f, 1.f, 1.f, 1.f );
    DisplayGlError("3");



    return true;

}

int Painter::GetMaxTextureSize(){
    int maxSize;
	glGetIntegerv (GL_MAX_TEXTURE_SIZE, &maxSize);
    return maxSize;
}



#endif // RENDER_OPENGL
