#ifndef SCREENMANAGEH
#define SCREENMANAGEH

#include "../settings/definitions.hpp"

#include SDL_LIB_HEADER
#include <string>
#include "../framework/geometry.hpp"
#include <GL/glew.h>
enum ResizeAction{
    RESIZE_SCALE,
    RESIZE_FREE_SCALE,
};

class ScreenManager{
    public:
        ScreenManager();
        ~ScreenManager();
        SDL_Window* StartScreen(std::string name);
        SDL_Renderer* StartRenderer();
        static ScreenManager& GetInstance();
        PointInt GetDisplaySize(){return m_display;};
        PointInt GetScreenSize(){return m_screen;};
        PointInt GetGameSize(){return m_originalScreen;};
        PointInt GetMaxTextureSize(){return m_maxTextureSize;};
        void NotifyResized();
        void NotyifyScreenClosed();
        void TerminateScreen();
        void SetResizeAction();
        void SetMinimumScale(Point s){ MinimumScale = s;};
        Point GetMinimumScale(){ return MinimumScale;};
        int SetRenderTarget(SDL_Texture *t,bool trueNull=false);
        void Resize(int w,int h);
        void SetScreenName(std::string name);
        void SetWindowSize(int w,int h);

        void Update(float dt);

        void ScreenShake(Point amount,float tick,float duration){
            savedShake = amount;
            ShakingTick = ShakingTick_original = tick;
            ShakingDuration = duration;
            shaking = true;
        };
        void ResetViewpPort();
        bool MakeDefaultScreenAsTexture();
        bool ClearScreenTexture();
        void RenderPresent();
        SDL_Texture * GetDefaultRenderer(){return m_defaultScreen;};
        void Render();
        bool SetupOpenGL();
        void PreRender();
        double GetScaleRatioH(){return m_defaultScreen != nullptr ? 1 : m_scaleRatio.y;};
        double GetScaleRatioW(){return m_defaultScreen != nullptr ? 1 : m_scaleRatio.x;};
        double GetOffsetW(){return (m_defaultScreen != nullptr ? 0 : m_offsetScreen.x)+shake.x;};
        double GetOffsetH(){return (m_defaultScreen != nullptr ? 0 : m_offsetScreen.y)+shake.y;};
        void ResizeToScale(int w,int h,ResizeAction behave);
        float GetFps(){return m_fps;};

        GLuint GetDefaultFrameBuffer();
        bool StartPostProcessing();
    private:
        bool postProcess;
        GLuint frameBuffer,fbo_texture,rbo_depth,fbo,vbo_fbo_vertices;

        SDL_GLContext m_glContext;
        SDL_Texture *m_defaultScreen;
        float m_fps;
        float m_frameDelay;
        int   m_frames;
        bool shaking;
        float ShakingDuration;
        float ShakingTick_original;
        float ShakingTick;
        Point lastValidScale;
        Point savedShake;
        Point shake;
        Point MinimumScale;
        SDL_Renderer* m_renderer;
        SDL_Window* m_window;
        PointInt m_display;
        PointInt m_screen;
        PointInt m_originalScreen;
        PointInt m_offsetScreen;
        PointInt m_maxTextureSize;
        Point m_scaleRatio;
        Point m_trueScaleRatio;
        SDL_DisplayMode m_displayMode;

};
#endif // SCREENMANAGEH
