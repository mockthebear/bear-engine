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
#include "../glm/gtc/matrix_transform.hpp"

float ScreenManager::ClearColor[4] = {0.0f, 0.0f, 0.0f, 1.0f};

ScreenManager::~ScreenManager(){
    TerminateScreen();
}
ScreenManager::ScreenManager(){
    m_scaleRatio = Point(1,1);
    lastValidScale = Point(1,1);
    postProcess = false;
    m_ScreenRationMultiplier = 4.0f;
    ShakingDuration = 0;
    shaking = 0;
    shaking = false;
    m_fps = 0;
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
        m_targetScreen.FreeTexture();
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

    if (!Painter::SetupEnvoriment(this)){
        return false;
    }
    ResetProjection();

    if (postProcess){
        StartPostProcessing();
    }

    SDL_GL_SetSwapInterval(0);

    int maxSize = Painter::GetMaxTextureSize();
    m_maxTextureSize.x = maxSize;
    m_maxTextureSize.y = maxSize;
    Painter::SetEmptyTexture();
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
    /*m_renderer = SDL_CreateRenderer( m_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
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
    */
    return nullptr;
}

void ScreenManager::RenderPresent(){

    if (postProcess){
        if (storedShader.IsLoaded()){
            storedShader.Bind();
        }
        m_targetScreen.UnBind();
        m_targetScreen.Render(Point(0.0f,0.0f));
        if (storedShader.IsLoaded()){
            storedShader.Unbind();
        }
        DisplayGlError("RenderPresent");
    }
    glFlush();
    SDL_GL_SwapWindow(m_window);

}


bool ScreenManager::StartPostProcessing(){
    postProcess = true;


    m_targetScreen.ClearTexture();
    m_targetScreen.Generate(m_originalScreen.x, m_originalScreen.y);

    DisplayGlError("StartPostProcessing");
    return true;
}

void ScreenManager::PreRender(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(ClearColor[0],ClearColor[1],ClearColor[2],ClearColor[3]);
    if (postProcess){
        m_targetScreen.Bind();
        glClearColor(ClearColor[0],ClearColor[1],ClearColor[2],ClearColor[3]);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

}

void ScreenManager::Render(){

}

void ScreenManager::ForceProjection(Point screenSize,int flipScreen){
    //Painter::SetViewport(screenSize, flipScreen,screenSize);
    //glm::ortho(0.0f, 800.0f, 600.0f, 0.0f, -1.0f, 1.0f);



    screenProjection = false;
}

void ScreenManager::ResetProjection(int flipScreen){
    //Painter::Projection = glm::ortho(0.0f, (float)m_originalScreen.x, 0.0f, (float)m_originalScreen.y, -1.0f, 1.0f);
    //bear::out << "rst projeej\n";
    Painter::SetProjection(m_originalScreen+ m_extraSidedResolution, flipScreen);
    Painter::SetViewport(m_screen+ m_extraSidedResolution,m_offsetScreen);
    screenProjection = true;
}

void ScreenManager::NotifyResized(){
    int newW,newH;
    SDL_GetWindowSize(m_window, &newW, &newH);




    if (GameBehavior::GetInstance().OnResize(newW,newH)){
        if (postProcess){
            m_targetScreen.FreeTexture();
            StartPostProcessing();
        }
        ResetProjection();
    }

}

void ScreenManager::SetWindowSize(int w,int h, bool callCore){
    if (callCore){
        Resize(w,h);
    }else{
        m_originalScreen.x=w;
        m_originalScreen.y=h;
        m_screen.x = w;
        m_screen.y = h;
        if (postProcess){
            m_targetScreen.FreeTexture();
            StartPostProcessing();
        }
    }

    ConfigManager::GetInstance().SetScreenSize(w,h, callCore);

    m_originalScreen.x=w;
    m_originalScreen.y=h;
    m_scaleRatio = Point(1,1);
    lastValidScale = Point(1,1);

    ResetProjection();
}

void ScreenManager::ToggleFullScreen(){
    static bool full = false;
    if (!full){
        SDL_SetWindowFullscreen(m_window, SDL_WINDOW_FULLSCREEN_DESKTOP);
    }else{
        SDL_SetWindowFullscreen(m_window, SDL_FALSE);
        ScreenManager::GetInstance().Resize(m_originalScreen.x,m_originalScreen.y);
    }
    full = !full;
}

void ScreenManager::Resize(int w,int h){

    m_originalScreen.x=w;
    m_originalScreen.y=h;
    m_screen.x = w;
    m_screen.y = h;

    SDL_SetWindowSize(m_window,w,h);

    ResetProjection();

    if (postProcess){
        m_targetScreen.FreeTexture();
        StartPostProcessing();
    }

    //SDL_SetWindowSize(m_window,w,h);
}
void ScreenManager::ResizeToScale(int w,int h,ResizeAction behave){


    if (w < ConfigManager::GetInstance().GetScreenW()*MinimumScale.x || h < ConfigManager::GetInstance().GetScreenH()*MinimumScale.y){
        Resize(ConfigManager::GetInstance().GetScreenW()*MinimumScale.x,ConfigManager::GetInstance().GetScreenH()*MinimumScale.y);
        return;
    }

    m_scaleRatio.x =  ( (double)w / (double)m_originalScreen.x);
    m_scaleRatio.y =  ( (double)h / (double)m_originalScreen.y);

    m_trueScaleRatio = m_scaleRatio;




    if (behave == RESIZE_KEEP_ASPECT || behave == RESIZE_KEEP_ASPECT_INCREASING){

        if (m_allowedResolutions.size() > 0){
            float nearestx = 100.0f;
            float nearesty = 100.0f;
            for (auto &it : m_allowedResolutions){
                if (m_scaleRatio.x > it){
                    nearestx = it;
                }else if (m_scaleRatio.x == it){
                    nearestx = it;
                }
                if (m_scaleRatio.y > it){
                    nearesty = it;
                }else if (m_scaleRatio.y == it){
                    nearesty = it;
                }
            }
            m_scaleRatio.y = nearesty;
            m_scaleRatio.x = nearestx;
        }else{
            m_scaleRatio.x = floor(m_scaleRatio.x*m_ScreenRationMultiplier)/m_ScreenRationMultiplier;
            m_scaleRatio.y = floor(m_scaleRatio.y*m_ScreenRationMultiplier)/m_ScreenRationMultiplier;
        }

        float lowestScale = std::max(m_scaleRatio.x, m_scaleRatio.y);

        m_scaleRatio.x = lowestScale;
        m_scaleRatio.y = lowestScale;

        m_screen.x = m_originalScreen.x * lowestScale;
        m_screen.y = m_originalScreen.y * lowestScale;

        if (behave != RESIZE_KEEP_ASPECT_INCREASING){

            m_offsetScreen.x = m_originalScreen.x == w ? 0 : w/2 - m_screen.x/2;
            m_offsetScreen.y = m_originalScreen.y == h ? 0 : h/2 - m_screen.y/2;
        }else{

            //m_offsetScreen.x = m_originalScreen.x == w ? 0 : w/2 - m_screen.x/2;
            //m_offsetScreen.y = m_originalScreen.y == h ? 0 : h/2 - m_screen.y/2;

            //m_offsetScreen.x = m_originalScreen.x == w ? 0 : w/2 - m_screen.x/2;
            //m_offsetScreen.y = m_originalScreen.y == h ? 0 : h/2 - m_screen.y/2;

            m_extraSidedResolution = PointInt(w - m_screen.x , h - m_screen.y );
            //
            //ScreenManager::GetInstance().SetWindowSize(newW,newH, false);
           // m_offsetScreen.x = m_originalScreen.x == w ? 0 : w/2 - m_screen.x/2;
            //m_offsetScreen.y = m_originalScreen.y == h ? 0 : h/2 - m_screen.y/2;

            Camera::Resize(Point(m_originalScreen.x + m_extraSidedResolution.x,m_originalScreen.y + m_extraSidedResolution.y));

            bear::out << "Left to the sides: " << m_extraSidedResolution << " : "<<m_screen<<" : "<<w<<","<<h<<" OFF: "<<m_offsetScreen<<"\n";

        }



    }else{



        /*if (behave != RESIZE_FREE_SCALE){
            m_scaleRatio.x = floor(m_scaleRatio.x*m_ScreenRationMultiplier)/m_ScreenRationMultiplier;
            m_scaleRatio.y = floor(m_scaleRatio.y*m_ScreenRationMultiplier)/m_ScreenRationMultiplier;
            m_scaleRatio.x = std::max(1.0f,m_scaleRatio.x);
            m_scaleRatio.y = std::max(1.0f,m_scaleRatio.y);
        }
        m_trueScaleRatio = m_scaleRatio;*/


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

    bear::out << "New: "<< m_screen << " and scale is " << m_scaleRatio << "\n";

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
        //bear::out << m_fps << "\n";
        m_frames = 0;

    }
}

