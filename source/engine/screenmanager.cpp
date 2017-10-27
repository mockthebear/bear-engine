#include "screenmanager.hpp"
#include "../settings/configmanager.hpp"
#include "../performance/console.hpp"
#include "../framework/utils.hpp"
#include "shadermanager.hpp"
#include "renderhelp.hpp"
#include __BEHAVIOR_FOLDER__
#include "camera.hpp"
#include <iostream>
#include <stdlib.h>
ScreenManager::~ScreenManager(){
    TerminateScreen();
}
ScreenManager::ScreenManager(){
    m_scaleRatio = Point(1,1);
    lastValidScale = Point(1,1);
    postProcess = true;
    ShakingDuration = 0;
    shaking = 0;
    shaking = false;
    m_fps = 0;
    m_defaultScreen = nullptr;
    m_frames = 0;
    m_frameDelay = 11.0f;
    m_window = NULL;
    m_window = NULL;
    MinimumScale = Point(1,1);
}

ScreenManager& ScreenManager::GetInstance(){
    static ScreenManager teste;
    return teste;
}
void ScreenManager::TerminateScreen(){

    if (m_renderer != NULL){
        bear::out << "[Terminating renderer]...";
        SDL_DestroyRenderer( m_renderer );
        m_renderer = NULL;
        bear::out << "[Done]\n";
    }

    if (m_window != NULL){
        bear::out << "[Terminating window]...";
        SDL_DestroyWindow( m_window );
        m_window = NULL;
        bear::out << "[Done]\n";
    }

}
void ScreenManager::NotyifyScreenClosed(){
    bear::out << "[Forcing window terminate]\n";
    m_window = NULL;
}

bool ScreenManager::SetupOpenGL(){
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetSwapInterval(1);

    glewExperimental = GL_TRUE;
    GLenum glewError = glewInit();
    if( glewError != GLEW_OK )
    {
        printf( "Error initializing GLEW! %s\n", glewGetErrorString( glewError ) );
    }


	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	SDL_GL_SwapWindow(m_window);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glViewport( 0.f, 0.f, m_originalScreen.x, m_originalScreen.y );


    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho( 0.0, m_originalScreen.x, m_originalScreen.y, 0.0, 1.0, -1.0 );
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    glPushMatrix();
    glClearColor( 0.f, 0.f, 0.f, 1.f );

    if (postProcess){

        glActiveTexture(GL_TEXTURE0);
        glGenTextures(1, &fbo_texture);
        glBindTexture(GL_TEXTURE_2D, fbo_texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_originalScreen.x, m_originalScreen.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glBindTexture(GL_TEXTURE_2D, 0);

        /* Depth buffer */
        glGenRenderbuffers(1, &rbo_depth);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo_depth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, m_originalScreen.x, m_originalScreen.y);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        /* Framebuffer to link everything together */
        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, fbo_texture, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo_depth);



        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        GLfloat fbo_vertices[] = {
            -1,  1,
            1,  1,
            -1,  -1,
            1,  -1,
        };
        glGenBuffers(1, &vbo_fbo_vertices);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_fbo_vertices);
        glBufferData(GL_ARRAY_BUFFER, sizeof(fbo_vertices), fbo_vertices, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);


        g_shader.Compile("engine/vertex.glvs","engine/color.glfs");
    }
    return true;
}

SDL_Window* ScreenManager::StartScreen(std::string name){
    if( SDL_GetCurrentDisplayMode( 0, &m_displayMode ) == 0 )
    {
        m_display = PointInt(m_displayMode.w,m_displayMode.h);
    }
    m_screen = PointInt(ConfigManager::GetInstance().GetScreenW(),ConfigManager::GetInstance().GetScreenH());
    if (m_display.x != 0 && m_display.y != 0 && (m_display.x < m_screen.x || m_display.y < m_screen.y) ){
        bear::out << "[ScreenManager::StartScreen] Display size suported is "<<m_display.x<<"x"<<m_display.y<<".\n";
        bear::out << "[ScreenManager::StartScreen] Impossible to create"<<m_screen.x<<"x"<<m_screen.y<<" dummy display.\n";
        //return NULL;
    }
    m_originalScreen = m_screen;
    Uint32 flags = 0;
    //Todo: resizer
    if (ConfigManager::GetInstance().GetResizeAction() != RESIZE_BEHAVIOR_NORESIZE){
        flags |= SDL_WINDOW_RESIZABLE;
    }
    flags |= SDL_WINDOW_OPENGL;
    m_window = SDL_CreateWindow( name.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, m_screen.x, m_screen.y,flags); //SDL_WINDOW_RESIZABLE
    if (!m_window){
        bear::out << "[ScreenManager::StartScreen] Display size suported is "<<m_display.x<<"x"<<m_display.y<<".\n";
        bear::out << "[ScreenManager::StartScreen] Impossible to create"<<m_screen.x<<"x"<<m_screen.y<<" dummy display.\n";
        return NULL;
    }
    m_glContext = SDL_GL_CreateContext(m_window);

    return m_window;
}

bool ScreenManager::MakeDefaultScreenAsTexture(){
    if (m_defaultScreen){
        return false;
    }
    m_defaultScreen = SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET,m_originalScreen.x,m_originalScreen.y);
    SetRenderTarget(nullptr,true);
    return m_defaultScreen != nullptr;
}

bool ScreenManager::ClearScreenTexture(){
    if (m_defaultScreen){
        SDL_DestroyTexture(m_defaultScreen);
        m_defaultScreen = nullptr;
        SetRenderTarget(nullptr);
        return true;
    }else{
        return false;
    }
}

SDL_Renderer* ScreenManager::StartRenderer(){
    m_renderer = SDL_CreateRenderer( m_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
    SDL_RendererInfo info;
    SDL_GetRendererInfo(m_renderer,&info);
    Console::GetInstance().AddText(utils::format("Driver: %s",info.name));
    Console::GetInstance().AddText(utils::format("Max view: %d x %d",info.max_texture_width,info.max_texture_height));

    m_maxTextureSize.x = info.max_texture_width;
    m_maxTextureSize.y = info.max_texture_height;

    if (m_renderer){
        SDL_SetRenderDrawBlendMode(m_renderer,SDL_BLENDMODE_BLEND);
    }
    //m_defaultScreen = SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET,m_originalScreen.x,m_originalScreen.y);


    return m_renderer;
}
#include "../input/inputmanager.hpp"
void ScreenManager::RenderPresent(){
    #ifndef RENDER_OPENGL
    if (m_defaultScreen){
        SetRenderTarget(m_defaultScreen);
    }
    int w = ConfigManager::GetInstance().GetScreenW();
    int h = ConfigManager::GetInstance().GetScreenH();
    RenderHelp::DrawSquareColorA(w,0,w/2.0,h + w/2.0,0,0,0,255);
    RenderHelp::DrawSquareColorA(-w/2.0,0,w/2.0,h + w/2.0,0,0,0,255);
    RenderHelp::DrawSquareColorA(-w/2.0,h,w*2,h/2.0,0,0,0,255);
    SDL_RenderPresent(m_renderer);
    #else

    if (postProcess){
        glViewport(m_offsetScreen.x, m_offsetScreen.y,m_screen.x, m_screen.y);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glOrtho( 0.0, m_originalScreen.x, m_originalScreen.y, 0.0, 1.0, -1.0 );
        glLoadIdentity();
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);


        g_shader.Bind();

        Point p = g_input.GetMouse();
                p.y = p.y/(float)SCREEN_SIZE_H;
                p.x = p.x/(float)SCREEN_SIZE_W;
                glUniform2f(g_shader.GetUniformLocation("Cent2d"),p.x,1.0f-p.y);

        glBindTexture(GL_TEXTURE_2D, fbo_texture);
        glEnable(GL_TEXTURE_2D);

        float w = m_screen.x;
        float h = m_screen.y;

        GLfloat  texLeft = 0.0f;
        GLfloat  texRight =  1.0f;
        GLfloat  texTop =  1.0f;
        GLfloat  texBottom = 0.0f;
        float quadWidth = m_screen.x;
        float quadHeight = m_screen.y;

        glTranslatef(
                        ( quadWidth / 2.f  ),
                        ( quadHeight/ 2.f  ),
                        0.f );

        glBegin( GL_QUADS );
                glTexCoord2f(  texLeft,    texTop ); glVertex2f( -quadWidth / 2.f, -quadHeight / 2.f );
                glTexCoord2f( texRight ,    texTop ); glVertex2f(  quadWidth / 2.f, -quadHeight / 2.f );
                glTexCoord2f( texRight , texBottom ); glVertex2f(  quadWidth / 2.f,  quadHeight / 2.f );
                glTexCoord2f(  texLeft , texBottom ); glVertex2f( -quadWidth / 2.f,  quadHeight / 2.f );
            glEnd();


        glPopMatrix();
    }
    SDL_GL_SwapWindow(m_window);
    if (postProcess)
        g_shader.Unbind();
    #endif // RENDER_OPENGL
}
void ScreenManager::PreRender(){
    #ifndef RENDER_OPENGL
    if (m_defaultScreen){
        SetRenderTarget(m_defaultScreen);
    }
    SDL_SetRenderDrawColor(m_renderer, 0,0,0, 0);
    SDL_RenderClear( m_renderer );
    #else
    if (postProcess){
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        glViewport(0,0,m_screen.x, m_screen.y);
    }else{
        glClearColor(0.0f, 0.0f, 0.0f, 1.0);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    }

    #endif // RENDER_OPENGL
}

void ScreenManager::Render(){
    #ifndef RENDER_OPENGL
    if (m_defaultScreen)
        SetRenderTarget(nullptr);
    if (m_defaultScreen){
        //SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY,"1");
        SDL_SetRenderDrawColor(m_renderer, 0,0,0, 0);
        SDL_RenderClear( m_renderer );
        SDL_Rect dimensions2;
        dimensions2.x = m_offsetScreen.x;
        dimensions2.y = m_offsetScreen.y;
        dimensions2.h = m_originalScreen.y*m_scaleRatio.y;
        dimensions2.w = m_originalScreen.x*m_scaleRatio.x;
        SDL_RenderCopy(m_renderer,m_defaultScreen,nullptr,&dimensions2);
        //SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY,"0");
    }
    #endif // RENDER_OPENGL

}

void ScreenManager::NotifyResized(){
    int newW,newH;
    SDL_GetWindowSize(m_window, &newW, &newH);




    if (GameBehavior::GetInstance().OnResize(newW,newH)){
        if (postProcess){
            glBindTexture(GL_TEXTURE_2D, fbo_texture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_screen.x, m_screen.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
            glBindTexture(GL_TEXTURE_2D, 0);

            glBindRenderbuffer(GL_RENDERBUFFER, rbo_depth);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, m_screen.x, m_screen.y);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);
        }else{
            glViewport(m_offsetScreen.x, m_offsetScreen.y,m_screen.x, m_screen.y);
        }
    }

}

void ScreenManager::SetWindowSize(int w,int h){

    Resize(w,h);
    ConfigManager::GetInstance().SetScreenSize(w,h);

    m_originalScreen.x=w;
    m_originalScreen.y=h;
    m_scaleRatio = Point(1,1);
    lastValidScale = Point(1,1);
}

void ScreenManager::Resize(int w,int h){

    SDL_SetWindowSize(m_window,w,h);
}
void ScreenManager::ResizeToScale(int w,int h,ResizeAction behave){
    if (w < ConfigManager::GetInstance().GetScreenW()*MinimumScale.x || h < ConfigManager::GetInstance().GetScreenH()*MinimumScale.y){
        Resize(ConfigManager::GetInstance().GetScreenW()*MinimumScale.x,ConfigManager::GetInstance().GetScreenH()*MinimumScale.y);
        return;
    }
    m_scaleRatio.x =  ( (double)w / (double)m_originalScreen.x);
    m_scaleRatio.y =  ( (double)h / (double)m_originalScreen.y);

    m_scaleRatio.x = round(m_scaleRatio.x*8.0)/8.0;
    m_scaleRatio.y = round(m_scaleRatio.y*8.0)/8.0;

    m_scaleRatio.x = std::max(1.0f,m_scaleRatio.x);
    m_scaleRatio.y = std::max(1.0f,m_scaleRatio.y);

    m_trueScaleRatio = m_scaleRatio;


    if (behave == RESIZE_SCALE){
        /*
            Strech screen to maintain an scale ratio
        */
        m_scaleRatio.x = std::min(m_scaleRatio.x,m_scaleRatio.y);
        m_scaleRatio.y = std::min(m_scaleRatio.x,m_scaleRatio.y);

        Point aux = m_originalScreen;
        aux.x *= m_scaleRatio.x;
        aux.y *= m_scaleRatio.y;
        m_screen.x = aux.x;
        m_screen.y = aux.y;

    }else{
        m_screen.x = w;
        m_screen.y = h;
    }


    //todo: Screen size less than original
    if (w < m_originalScreen.x*m_scaleRatio.x || h < m_originalScreen.y*m_scaleRatio.y){
        Point ValidScale = m_scaleRatio;
        Point LocalScale = m_scaleRatio;
        while(m_originalScreen.x*LocalScale.x > w || m_originalScreen.y*LocalScale.y > h){
            ValidScale.x -= 1.0/8.0;
            ValidScale.y -= 1.0/8.0;
            LocalScale.x = round(ValidScale.x*8.0)/8.0;
            LocalScale.y = round(ValidScale.y*8.0)/8.0;
        }
        m_scaleRatio = LocalScale;
        Point aux = m_originalScreen;
        aux.x *= m_scaleRatio.x;
        aux.y *= m_scaleRatio.y;
        m_screen.x = aux.x;
        m_screen.y = aux.y;
    }

    m_offsetScreen.x = m_originalScreen.x == w ? 0 : w/2 - m_screen.x/2;
    m_offsetScreen.y = m_originalScreen.y == h ? 0 : h/2 - m_screen.y/2;
}

void ScreenManager::SetScreenName(std::string name){
    SDL_SetWindowTitle(m_window, name.c_str());
}
void ScreenManager::Update(float dt){
    if (shaking){
        ShakingDuration -= dt;
        if (ShakingDuration <= 0){
            shaking = false;
            shake = Point(0,0);
            savedShake = Point(0,0);
        }
        ShakingTick -= dt;
        if (ShakingTick <= 0){
            ShakingTick = ShakingTick_original;
            shake.x = (int)(GetScaleRatioW()*savedShake.x) - rand()%(int)(std::max(savedShake.x*2*GetScaleRatioW(),2.0));
            shake.y = (int)(GetScaleRatioH()*savedShake.y) - rand()%(int)(std::max(savedShake.y*2*GetScaleRatioH(),2.0));
        }

    }
    m_frames++;
    m_frameDelay -= dt;

    if (m_frameDelay <= 0){
        m_frameDelay = 10.0f;
        m_fps = m_frames;
        m_frames = 0;

    }
}
int ScreenManager::SetRenderTarget(SDL_Texture *t,bool trueNull){
    //todo: texture targeting
    /*
    if (t == nullptr && m_defaultScreen){
        //return SDL_SetRenderTarget(m_renderer,m_defaultScreen);
    }
    if (trueNull){
        return SDL_SetRenderTarget(m_renderer,m_defaultScreen);
    }
    */
    return SDL_SetRenderTarget(m_renderer,t);
}
