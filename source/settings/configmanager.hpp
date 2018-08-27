
#ifndef CONFIGH
#define CONFIGH

#include "definitions.hpp"
#include "../engine/sprite.hpp"
#include "../framework/geometry.hpp"
#include <vector>
#include <string>

enum ResizeBehavior{
    RESIZE_BEHAVIOR_NORESIZE=0,
    RESIZE_BEHAVIOR_SCALE,
    RESIZE_BEHAVIOR_SCALE_FREE,
    RESIZE_BEHAVIOR_INCREASE,
    RESIZE_BEHAVIOR_KEEP_ASPECT_RATIO,
};

class ConfigManager{
    public:

        double GetUIscale(){return 1.0;};

        int GetScreenH(){return m_screenSize.y;};
        int GetScreenW(){return m_screenSize.x;};
        PointInt GetScreenSize(){return m_screenSize;};

        void SetScreenSize(int w,int h);
        void SetWindowIcon(std::string icon,ColorReplacer &r);
        void SetWindowIcon(std::string icon);

        bool IsWorkingAudio(){return hasAudio;};
        void SetSound(bool var){hasAudio = var;};

        ResizeBehavior GetResizeAction(){return m_rbheavior;};
        void SetResizeAction(ResizeBehavior action){m_rbheavior=action;};
        static int MaxFps;
        static int MinFps;
        static float MinimumDT;
        static ConfigManager& GetInstance();
        ConfigManager();
        ~ConfigManager();

        void RegisterArgs(int argc,char *argv[]);
        void DisplayArgs();

        std::vector<std::string> GetArgs(){return args;};

        void SetScreenMode(TextureLoadMethodEnum md){
            screenMode.mode = md;
        }

        TextureLoadMethod screenMode;

        bool pause;
    private:
        ResizeBehavior m_rbheavior;
        bool hasAudio;
        PointInt m_screenSize;

        std::vector<std::string> args;


};
#endif

