#ifdef RENDER_OPENGL
#include "painters.hpp"
#include "../bear.hpp"
#include "../screenmanager.hpp"
#include "../../framework/debughelper.hpp"

void RenderData::UpdateVertex(){}

glm::mat4 Painter::Projection;


void Painter::SetViewport(Point size,int flipScreen,Point screenNow,Point offset){
    glViewport(offset.x, offset.y, screenNow.x, screenNow.y);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
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

void Painter::DrawVertex(VertexArrayObjectPtr vertexData,BasicRenderDataPtr t_data,int drawMode){
    glLoadIdentity();
    glTranslatef(t_data->position.x, t_data->position.y, 0.0f);
    glRotatef( glm::degrees(t_data->m_angle), 0.f, 0.f, 1.f );
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
    if (!t_texture || t_texture->id == 0){
        return false;
    }
    float points[] ={
        t_data->position.x, t_data->position.y,
        t_data->m_forwardClip.x, t_data->m_forwardClip.y, t_data->m_forwardClip.w, t_data->m_forwardClip.h,
        t_data->m_clip.w, t_data->m_clip.h,
        t_data->m_scale.x, t_data->m_scale.y,
        t_data->m_angle,

    };
    m_vao.vertexes.AddVertexes( sizeof(points) / sizeof(float) ,points);

    return true;
}


bool Painter::DrawSprites(int id){
    return false;
}

bool Painter::RenderTexture(BearTexture *t_texture, RenderDataPtr t_data){
    return false;
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



#endif // RENDER_OPENGL
