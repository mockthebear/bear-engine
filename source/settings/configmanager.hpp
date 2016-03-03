
#ifndef CONFIGH
#define CONFIGH

#include "definitions.hpp"
#include "../framework/geometry.hpp"

enum ResizeBehavior{
    RESIZE_BEHAVIOR_NORESIZE,
    RESIZE_BEHAVIOR_SCALE,
    RESIZE_BEHAVIOR_SCALE_FREE,
    RESIZE_BEHAVIOR_INCREASE,
};

class ConfigManager{
    public:

        double GetUIscale(){return 1.0;};

        int GetScreenH(){return m_screenSize.y;};
        int GetScreenW(){return m_screenSize.x;};
        PointInt GetScreenSize(){return m_screenSize;};

        void SetScreenSize(int w,int h);

        bool IsWorkingAudio(){return hasAudio;};
        void SetSound(bool var){hasAudio = var;};

        ResizeBehavior GetResizeAction(){return m_rbheavior;};
        void SetResizeAction(ResizeBehavior action){m_rbheavior=action;};

        static ConfigManager& GetInstance();
        ConfigManager();
        ~ConfigManager();
        bool pause;
    private:
        ResizeBehavior m_rbheavior;
        bool hasAudio;
        PointInt m_screenSize;


};
#endif

