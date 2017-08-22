#ifndef LIGHTH
#define LIGHTH
#include "smarttexture.hpp"
#include "../framework/threadpool.hpp"
#ifndef DISABLE_THREADPOOL
#include <pthread.h>
#endif
enum LightStep{
    LIGHT_BEGIN,
    LIGHT_SHADE,
    LIGHT_REDUCE,
    LIGHT_GEN,
    LIGHT_AUTO,
};
class Light{
    public:
        Light();
        static Light* GetInstance();
        static Light* Startup();
        bool IsStarted(){return out != nullptr;};
        bool Shutdown();
        bool StartLights(Point size,Point ExtraSize,uint16_t dotSize,float permissive=8.8,uint16_t maxDarkness=200);

        void Update(float dt,LightStep step);

        void Render(Point pos = Point(0,0));

        bool AddLightM(int,int,uint8_t);
        void AddBlockM(int,int,uint8_t);
        void AddBlock(Rect,uint8_t);

        int GetBlockSize(){return blockSize;};

        void SetLightRaysCount(int n){
            MaxCycles = std::max(4,std::min(n,1000));
        }
        void Reduce(parameters *P,Job &j);
        void Smooth(parameters *P,Job &j);
        void Shade(parameters *P,Job &j);
        void Gen(parameters *P,Job &j);
        void WaitDone();
        #ifndef DISABLE_THREADPOOL
        static pthread_mutex_t GCritical;
        #endif
    private:
        bool AutoFinished;
        uint8_t MaxDarkness;
        uint8_t *ShadeMap;
        uint8_t *DataMap;
        uint8_t **MapMap;
        uint8_t *GiveAnAdderess(bool clear=false);
        bool IsInLimits(int,int);
        int sizeX,sizeY;
        int blockSize;
        int MaxCycles;
        float Permissive;
        int CurrentAlloc;

        #ifndef DISABLE_THREADPOOL
        pthread_mutex_t Critical;
        pthread_mutex_t CanRender;
        pthread_mutex_t JobInterval;
        #endif

        int GetAround(unsigned char*,int x,int y);
        GenericPoint3<int> Lights[128];
        int LightPoints;
        static Light *Slight;
        int maxAlloc;
        Point size,ExtraSize;
        Uint32 *pix;

        SmartTexture *out;

        //auto things
        bool onAutomatic;
        bool doneAuto;
        int LightJobs;
};
#endif // LIGHTH
