#ifndef SCREENMANAGEH
#define SCREENMANAGEH

#include "../settings/definitions.hpp"

#include SDL_LIB_HEADER
#include <string>
#include "../framework/geometry.hpp"
#include "shadermanager.hpp"
#include "targettexture.hpp"
#include "libheader.hpp"
#include "../glm/glm.hpp"


enum ResizeAction{
    RESIZE_SCALE,
    RESIZE_FREE_SCALE,
    RESIZE_KEEP_ASPECT,
};

class ScreenManager{
    public:

        static float ClearColor[4];
        static ScreenManager& GetInstance();


        ScreenManager();
        ~ScreenManager();
        SDL_Window* StartScreen(std::string name);
        SDL_Renderer* StartRenderer();

        PointInt GetDisplaySize(){return m_display;};
        PointInt GetScreenSize(){return m_screen;};
        PointInt GetGameSize(){return m_originalScreen;};
        PointInt GetMaxTextureSize(){return m_maxTextureSize;};
        void ToggleFullScreen();
        void NotifyResized();
        void NotyifyScreenClosed();
        void TerminateScreen();
        void SetMinimumScale(Point s){ MinimumScale = s;};
        Point GetMinimumScale(){ return MinimumScale;};
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
        void RenderPresent();
        void Render();
        bool SetupOpenGL();
        void PreRender();
        double GetScaleRatioH(){return  m_scaleRatio.y;};
        double GetScaleRatioW(){return  m_scaleRatio.x;};
        double GetOffsetW(){return m_offsetScreen.x+shake.x;};
        double GetOffsetH(){return m_offsetScreen.y+shake.y;};
        void ResizeToScale(int w,int h,ResizeAction behave);
        float GetFps(){return m_fps;};

        bool StartPostProcessing();

        void SetTopShader(Shader &shdr){
            storedShader = shdr;
        }

        void DeleteTopShader(){
            storedShader = Shader();
        }

        Shader& GetTopShader(){
            return storedShader;
        }

        GLuint GetVAO(){return m_vertexArrayID;};

        void ResetProjection(int flip = SDL_FLIP_NONE);
        void ForceProjection(Point screenSize,int flipScreen = SDL_FLIP_NONE);

        void StopPostProcessing(){
            postProcess = false;
            m_targetScreen.FreeTexture();
            ResetProjection();
        }

    private:
        friend class Painter;

        Shader storedShader;

        TargetTexture m_targetScreen;

        bool postProcess;

        SDL_GLContext m_glContext;
        float m_fps;
        float m_frameDelay;
        int   m_frames;
        bool shaking;
        float ShakingDuration;
        float ShakingTick_original;
        float ShakingTick;
        float m_ScreenRationMultiplier;
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
        uint32_t m_vertexArrayID;
        bool screenProjection;

};
#endif // SCREENMANAGEH
