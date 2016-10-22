#include "screenmanager.hpp"
#include "../settings/configmanager.hpp"
#include "../performance/console.hpp"
#include "../framework/utils.hpp"
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
    ShakingDuration = 0;
    shaking = 0;
    shaking = false;
    m_fps = 0;
    m_defaultScreen = nullptr;
    m_frames = 0;
    m_frameDelay = 11.0f;
    m_window = NULL;
    m_window = NULL;
}

ScreenManager& ScreenManager::GetInstance(){
    static ScreenManager teste;
    return teste;
}
void ScreenManager::TerminateScreen(){

    if (m_renderer != NULL){
        std::cout << "[Terminating renderer]\n";
        SDL_DestroyRenderer( m_renderer );
        m_renderer = NULL;
        std::cout << "[Done] "<<SDL_GetError()<<"\n";
    }

    if (m_window != NULL){
        std::cout << "[Terminating window]"<<SDL_GetError()<<"\n";
        SDL_DestroyWindow( m_window );
        m_window = NULL;
        std::cout << "[Done] "<<SDL_GetError()<<"\n";
    }

}
void ScreenManager::NotyifyScreenClosed(){
    std::cout << "[Forcing window terminate]\n";
    m_window = NULL;
}
SDL_Window* ScreenManager::StartScreen(std::string name){
    if( SDL_GetCurrentDisplayMode( 0, &m_displayMode ) == 0 )
    {
        m_display = PointInt(m_displayMode.w,m_displayMode.h);
    }
    m_screen = PointInt(ConfigManager::GetInstance().GetScreenW(),ConfigManager::GetInstance().GetScreenH());
    if (m_display.x != 0 && m_display.y != 0 && (m_display.x < m_screen.x or m_display.y < m_screen.y) ){
        std::cout << "[ScreenManager::StartScreen] Display size suported is "<<m_display.x<<"x"<<m_display.y<<".\nImpossible to create"<<m_screen.x<<"x"<<m_screen.y<<"\n";
        return NULL;
    }
    m_display = m_screen;
    m_originalScreen = m_screen;



    m_window = SDL_CreateWindow( name.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, m_screen.x, m_screen.y, SDL_WINDOW_RESIZABLE); //SDL_WINDOW_RESIZABLE
    return m_window;
}
SDL_Renderer* ScreenManager::StartRenderer(){
    m_renderer = SDL_CreateRenderer( m_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
    SDL_RendererInfo info;
    SDL_GetRendererInfo(m_renderer,&info);
    Console::GetInstance().AddText(utils::format("Driver: %s",info.name));
    Console::GetInstance().AddText(utils::format("Max view: %d x %d",info.max_texture_width,info.max_texture_height));

    if (m_renderer){
        SDL_SetRenderDrawBlendMode(m_renderer,SDL_BLENDMODE_BLEND);
    }
    //m_defaultScreen = SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET,m_originalScreen.x,m_originalScreen.y);


    return m_renderer;
}

void ScreenManager::RenderPresent(){
    SDL_RenderPresent(m_renderer);
}
void ScreenManager::PreRender(){
    if (m_defaultScreen){
        SetRenderTarget(m_defaultScreen);
    }
    SDL_SetRenderDrawColor(m_renderer, 0,0,0, 0);
    SDL_RenderClear( m_renderer );
}

void ScreenManager::Render(){
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
}

void ScreenManager::NotifyResized(){
    int newW,newH;
    SDL_GetWindowSize(m_window, &newW, &newH);
    if (GameBehavior::GetInstance().OnResize(newW,newH)){
        m_screen.x = newW;
        m_screen.y = newH;
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
    if (w < ConfigManager::GetInstance().GetScreenW() or h < ConfigManager::GetInstance().GetScreenH()){
        Resize(ConfigManager::GetInstance().GetScreenW(),ConfigManager::GetInstance().GetScreenH());
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
        m_scaleRatio.x = std::min(m_scaleRatio.x,m_scaleRatio.y);
        m_scaleRatio.y = std::min(m_scaleRatio.x,m_scaleRatio.y);
    }

    if (w < m_originalScreen.x*m_scaleRatio.x or h < m_originalScreen.y*m_scaleRatio.y){
        Point ValidScale = m_scaleRatio;
        Point LocalScale = m_scaleRatio;
        while(m_originalScreen.x*LocalScale.x > w or m_originalScreen.y*LocalScale.y > h){
            ValidScale.x -= 1.0/8.0;
            ValidScale.y -= 1.0/8.0;
            LocalScale.x = round(ValidScale.x*8.0)/8.0;
            LocalScale.y = round(ValidScale.y*8.0)/8.0;
        }
        m_scaleRatio = LocalScale;
    }

    m_offsetScreen.x = m_originalScreen.x == w ? 0 : w/2 - (m_originalScreen.x/2)*m_scaleRatio.x;
    //std::cout << m_offsetScreen.x << " or size: " <<m_originalScreen.x << "ratio: " << m_scaleRatio.x << " to: " << w << "\n";
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
int ScreenManager::SetRenderTarget(SDL_Texture *t){
    return SDL_SetRenderTarget(m_renderer,t);
}
