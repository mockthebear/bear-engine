#include "screenmanager.hpp"
#include "../settings/configmanager.hpp"
#include "../performance/console.hpp"
#include "../framework/utils.hpp"
#include "../framework/debughelper.hpp"
#include "shadermanager.hpp"
#include "renderhelp.hpp"
#include __BEHAVIOR_FOLDER__
#include "camera.hpp"
#include <iostream>
#include <stdlib.h>
#include "../input/inputmanager.hpp"
float ScreenManager::ClearColor[4] = {0.0f, 0.0f, 0.0f, 0.0f};

ScreenManager::~ScreenManager(){
    TerminateScreen();
}
ScreenManager::ScreenManager(){
    m_scaleRatio = Point(1,1);
    lastValidScale = Point(1,1);
    postProcess = false;
    m_ScreenRationMultiplier = 2.0f;
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
    if (postProcess){
        glDeleteTextures(1, &fbo_texture);
        glDeleteRenderbuffers(1, &rbo_depth);
        glDeleteFramebuffers(1, &fbo);
        postProcess = false;
    }
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

    #ifdef RENDER_OPENGL

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetSwapInterval(0);




    glewExperimental = GL_TRUE;
    GLenum glewError = glewInit();
    if( glewError != GLEW_OK )
    {
        bear::out << "Error initializing GLEW!\n";
    }


    DebugHelper::DisplayGlError("1");
    //glDebugMessageCallback(openglErrF, nullptr);


	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	SDL_GL_SwapWindow(m_window);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    ResetViewPort();

    RenderHelp::SetupShaders();


	#ifdef GL2
        DebugHelper::DisplayGlError("2");
        glMatrixMode( GL_PROJECTION );
        glLoadIdentity();
        glOrtho( 0.0, m_originalScreen.x, m_originalScreen.y, 0.0, 1.0, -1.0 );
        glMatrixMode( GL_MODELVIEW );
        glLoadIdentity();


        glPushMatrix();
        glClearColor( 1.f, 1.f, 1.f, 1.f );
        DebugHelper::DisplayGlError("3");
    #endif // GL2

    if (postProcess){
        StartPostProcessing();
    }

    SDL_GL_SetSwapInterval(0);

    GLint maxSize;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxSize);
    m_maxTextureSize.x = maxSize;
    m_maxTextureSize.y = maxSize;

    #endif // RENDER_OPENGL





    return true;
}

bool ScreenManager::StartPostProcessing(){
    postProcess = true;

    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &fbo_texture);
    glBindTexture(GL_TEXTURE_2D, fbo_texture);


    ConfigManager::GetInstance().screenMode.ApplyFilter();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

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

    DebugHelper::DisplayGlError("StartPostProcessing");
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
    }
    m_originalScreen = m_screen;
    Uint32 flags = 0;
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
    //Disable any fullscreen on startup. Necessary on linux to dont break the display
    SDL_SetWindowFullscreen(m_window, SDL_FALSE);
    return m_window;
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
    return m_renderer;
}

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
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glLoadIdentity();
        glClearColor(ClearColor[0],ClearColor[1],ClearColor[2],ClearColor[3]);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        glViewport(m_offsetScreen.x, m_offsetScreen.y,m_screen.x, m_screen.y);
        if (storedShader.IsLoaded()){
            storedShader.Bind();
        }
        glBindTexture(GL_TEXTURE_2D, fbo_texture);
        glEnable(GL_TEXTURE_2D);
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

        float w = m_originalScreen.x;
        float h = m_originalScreen.y;

        glTranslatef(
                        ( w / 2.f  ),
                        ( h / 2.f  ),
                        0.f );

        glBegin( GL_TRIANGLE_FAN );
                glTexCoord2f(  0.0f,    1.0f ); glVertex2f( -w / 2.f, -h / 2.f );
                glTexCoord2f( 1.0f ,    1.0f ); glVertex2f(  w / 2.f, -h / 2.f );
                glTexCoord2f( 1.0f , 0.0f ); glVertex2f(  w / 2.f,  h / 2.f );
                glTexCoord2f(  0.0f , 0.0f ); glVertex2f( -w / 2.f,  h / 2.f );
        glEnd();

        if (storedShader.IsLoaded()){
            storedShader.Unbind();
        }
        DebugHelper::DisplayGlError("RenderPresent");
    }
    glFinish();
    SDL_GL_SwapWindow(m_window);
    #endif // RENDER_OPENGL
}

void ScreenManager::ResetViewPort(){
    #ifdef RENDER_OPENGL
    /*glViewport(0,0,m_screen.x, m_screen.y);
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho( 0.0, m_originalScreen.x, m_originalScreen.y, 0.0, 1.0, -1.0 );
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();*/
    glViewport( 0.f, 0.f, m_originalScreen.x, m_originalScreen.y );
    #endif // RENDER_OPENGL
}
void ScreenManager::PreRender(){
    #ifdef RENDER_OPENGL

    if (postProcess){
        glViewport(shake.x,shake.y,m_screen.x, m_screen.y);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glBindTexture( GL_TEXTURE_2D, 0 );
        glClearColor(ClearColor[0],ClearColor[1],ClearColor[2],ClearColor[3]);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }else{
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        glClearColor(ClearColor[0],ClearColor[1],ClearColor[2],ClearColor[3]);
    }
    #else
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        glClearColor(ClearColor[0],ClearColor[1],ClearColor[2],ClearColor[3]);
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
            glMatrixMode( GL_PROJECTION );
            glLoadIdentity();
            glOrtho( 0.0, m_originalScreen.x, m_originalScreen.y, 0.0, 1.0, -1.0 );
            glMatrixMode( GL_MODELVIEW );
            glLoadIdentity();
            glPushMatrix();
        }
    }

}

void ScreenManager::SetWindowSize(int w,int h){

    Resize(w,h);

    m_scaleRatio = Point(1,1);
    lastValidScale = Point(1,1);



    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho( 0.0, w,h, 0.0, 1.0, -1.0 );
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();



}

void ScreenManager::Resize(int w,int h){

    m_originalScreen.x=w;
    m_originalScreen.y=h;
    m_screen.x = w;
    m_screen.y = h;

    SDL_SetWindowSize(m_window,w,h);

    if (postProcess){
        glDeleteTextures(1, &fbo_texture);
        glDeleteRenderbuffers(1, &rbo_depth);
        glDeleteFramebuffers(1, &fbo);
        glDeleteBuffers(1, &vbo_fbo_vertices);
        StartPostProcessing();
    }
}
void ScreenManager::ResizeToScale(int w,int h,ResizeAction behave){
    if (behave != RESIZE_FREE_SCALE){
        if (w < ConfigManager::GetInstance().GetScreenW()*MinimumScale.x || h < ConfigManager::GetInstance().GetScreenH()*MinimumScale.y){
            Resize(ConfigManager::GetInstance().GetScreenW()*MinimumScale.x,ConfigManager::GetInstance().GetScreenH()*MinimumScale.y);
            return;
        }
    }
    m_scaleRatio.x =  ( (double)w / (double)m_originalScreen.x);
    m_scaleRatio.y =  ( (double)h / (double)m_originalScreen.y);
    if (behave != RESIZE_FREE_SCALE){

        m_scaleRatio.x = floor(m_scaleRatio.x*m_ScreenRationMultiplier)/m_ScreenRationMultiplier;
        m_scaleRatio.y = floor(m_scaleRatio.y*m_ScreenRationMultiplier)/m_ScreenRationMultiplier;


        m_scaleRatio.x = std::max(1.0f,m_scaleRatio.x);
        m_scaleRatio.y = std::max(1.0f,m_scaleRatio.y);
    }

    m_trueScaleRatio = m_scaleRatio;


    if (behave == RESIZE_SCALE ){
        /*
            Strech screen to maintain an scale ratio
        */

        Point aux = m_originalScreen;
        aux.x *= m_scaleRatio.x;
        aux.y *= m_scaleRatio.y;
        m_screen.x = aux.x;
        m_screen.y = aux.y;

    }else{
        m_screen.x = w;
        m_screen.y = h;
    }

    if (behave != RESIZE_FREE_SCALE){
        if (w < m_originalScreen.x*m_scaleRatio.x || h < m_originalScreen.y*m_scaleRatio.y){
            Point ValidScale = m_scaleRatio;
            Point LocalScale = m_scaleRatio;
            while(m_originalScreen.x*LocalScale.x > w || m_originalScreen.y*LocalScale.y > h){
                ValidScale.x -= 1.0/m_ScreenRationMultiplier;
                ValidScale.y -= 1.0/m_ScreenRationMultiplier;
                LocalScale.x = floor(ValidScale.x*m_ScreenRationMultiplier)/m_ScreenRationMultiplier;
                LocalScale.y = floor(ValidScale.y*m_ScreenRationMultiplier)/m_ScreenRationMultiplier;
            }
            m_scaleRatio = LocalScale;
            Point aux = m_originalScreen;
            aux.x *= m_scaleRatio.x;
            aux.y *= m_scaleRatio.y;
            m_screen.x = aux.x;
            m_screen.y = aux.y;
        }
    }
    if (behave != RESIZE_FREE_SCALE){
        m_offsetScreen.x = m_originalScreen.x == w ? 0 : w/2 - m_screen.x/2;
        m_offsetScreen.y = m_originalScreen.y == h ? 0 : h/2 - m_screen.y/2;
    }else{
        m_offsetScreen.x = 0;
        m_offsetScreen.y = 0;
    }

}

void ScreenManager::SetScreenName(std::string name){
    SDL_SetWindowTitle(m_window, name.c_str());
}
void ScreenManager::Update(float dt){
    if (shaking){
        ShakingDuration -= dt;
        ShakingTick -= dt;
        if (ShakingTick <= 0){
            ShakingTick = ShakingTick_original;
            shake.x = (int)(GetScaleRatioW()*savedShake.x) - rand()%(int)(std::max(savedShake.x*2*GetScaleRatioW(),2.0));
            shake.y = (int)(GetScaleRatioH()*savedShake.y) - rand()%(int)(std::max(savedShake.y*2*GetScaleRatioH(),2.0));
        }
        if (ShakingDuration <= 0){
            shaking = false;
            shake = Point(0,0);
            savedShake = Point(0,0);
        }

    }else{
        shake = Point(0,0);
    }
    m_frames++;
    m_frameDelay -= dt;

    if (m_frameDelay <= 0){
        m_frameDelay = 10.0f;
        m_fps = m_frames;
        m_frames = 0;

    }
}

GLuint ScreenManager::GetDefaultFrameBuffer(){
    if (!postProcess){
        return 0;
    }
    return frameBuffer;
}


