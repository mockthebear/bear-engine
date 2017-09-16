#include "light.hpp"
#include "camera.hpp"
#include "screenmanager.hpp"
#include "gamebase.hpp"
#include "../framework/threadpool.hpp"
#include "renderhelp.hpp"
#include "../performance/console.hpp"

Light* Light::Slight = nullptr;
#ifndef DISABLE_THREADPOOL
pthread_mutex_t Light::GCritical;
#endif

Light::Light(){
    out = nullptr;
    onAutomatic = false;
    #ifndef DISABLE_THREADPOOL
    pthread_mutex_init(&Critical,nullptr);
    pthread_mutex_init(&CanRender,nullptr);
    pthread_mutex_init(&JobInterval,nullptr);
    pthread_mutex_lock(&CanRender);
    #endif
    LightJobs = 0;
    MaxDarkness = 200;
    maxAlloc=2;
    MapMap = nullptr;
}
Light* Light::Startup(){
    #ifndef DISABLE_THREADPOOL
    pthread_mutex_init(&GCritical,nullptr);
    #endif // DISABLE_THREADPOOL
    return GetInstance();
}
Light* Light::GetInstance(){
    #ifndef DISABLE_THREADPOOL
    pthread_mutex_lock(&GCritical);
    #endif
    if (Slight == nullptr){
        Slight = new Light();
    }
    #ifndef DISABLE_THREADPOOL
    pthread_mutex_unlock(&GCritical);
    #endif
    return Slight;
}


int Light::GetAround(unsigned char* map,int x,int y){

    uint32_t n = 0;
    uint32_t k = 0;
    uint32_t auxV = 0;
    int ay,ax;
    int auxY;
    for (ay=-1;ay<=1;ay++){
         auxY = (y+ay) * sizeX;
         for (ax=-1;ax<=1;ax++){
            if (!(x+ax < 0 || x+ax >= sizeX || y+ay < 0 || y+ay >= sizeY)){
                auxV = map[auxY + x+ax];
                n += auxV*auxV;
                k++;
            }
        }
    }
    return sqrt(n/k);
}
void Light::Gen(parameters *P,Job &j){
    uint8_t shaded;
    float yellowish;
    int y;
    int x;
    int yoff;
    for (y=j.from;y<j.to;y++){
        yoff = y*sizeX;
        for (x=0;x<sizeX;x++){
            shaded = GetAround(ShadeMap,x,y);
            yellowish = (255-shaded)/4.0;

            pix[yoff + x ] = RenderHelp::FormatRGBA2(yellowish,yellowish,190-yellowish/2,shaded);

        }
    }
}
void Light::Reduce(parameters *P,Job &j){

    for (int y=j.from;y<j.to;y++){
        for (int x=0;x<sizeX;x++){
            unsigned char MPP = ShadeMap[y * sizeX+  x];
            unsigned char K;
            for (int i=0;i<CurrentAlloc;i++){
                K =  MapMap[i][y * sizeX+  x]; //Segundo borramento!
                if (MPP < MaxDarkness){
                    int offset = MaxDarkness-MPP;
                    K = std::max(K-offset,0);
                }
                MPP = std::min(MPP,K);
            }
            ShadeMap[y * sizeX+  x] = MPP;
            //pix[y*sizeX + x ] = ShadeMap[y * sizeX+  x];

        }
    }
}
void Light::Shade(parameters *P,Job &j){

    int dis = j.me;
    int Threads = j.tsize;
    unsigned char strenght = j.bright;
    int x = j.from;
    int y = j.to;
    int i,max;
    int sx;
    int sy;


    float STR;
    float OSTR;
    float removal;
    int lsx,lsy;

    uint8_t val;

    uint16_t Lum;

    uint8_t *vec = j.vect;

    const float AngularConstant = (360.0f/(float)MaxCycles) * M_PI / 180.0f;

    i = dis * (MaxCycles/Threads);
    max = (dis+1) * (MaxCycles/Threads);
    int process = 0;
    for (;i<max;i++){
        STR = (strenght&127)*2;
        OSTR = (strenght&127)*2;
        removal = 1.0f;
        lsx=0;
        lsy=0;
        vec[y * sizeX+  x] = 0;

        //  step means the distance for each iteration
        //  Stops when the strenght of the light is less or equal 0

        for (float step= 0.0f;STR >= 0.0f;step += 0.5f){
            sx = sin(i * AngularConstant)*step;
            sy = cos(i * AngularConstant)*step;


            //    If the last step is the same position of the last iteration
            //    skip one step.

            if (lsx == sx && lsy == sy){
                STR = (OSTR - step*Permissive*removal);
                continue;
            }
            lsx = sx;
            lsy = sy;

            //    If is inside the limits

            if (IsInLimits(x+sx,y+sy)){
                process++;
                val = DataMap[ (y+sy)*sizeX + x+sx];
                //    If there is something to block
                if (val){
                    //Block
                    Lum = MaxDarkness;
                    Lum = std::max(0.0f,  (Lum)-(STR));
                    vec[(y+sy)*sizeX +x+sx] = uint8_t(Lum);
                    OSTR -= OSTR* ( val /255.0f);
                }else{
                    Lum = MaxDarkness;
                    Lum = std::max(0.0f,  (Lum)-(STR));
                    vec[(y+sy)*sizeX +x+sx] = uint8_t(Lum);
                }
            }


            STR = (OSTR - step*Permissive*removal);
        }
    }
}

bool Light::Shutdown(){
    if(!IsStarted()){
        return false;
    }
    Console::GetInstance().AddTextInfo("Deleting light.");
    delete out;

    std::cout << maxAlloc << " : "<<MapMap<<"\n";
    if (MapMap != nullptr){
        for (int i=0;i<maxAlloc;i++){
            std::cout << i << "\n";
            delete []MapMap[i];
        }
        delete []MapMap;
    }
    delete []ShadeMap;
    delete []DataMap;
    MapMap = nullptr;
    out = nullptr;
    Console::GetInstance().AddTextInfo("Light deleted.");
    return true;
}
bool Light::StartLights(Point size_,Point ExtraSize_,uint16_t dotSize,float permissive,uint16_t maxDarkness){
    if (out != nullptr){
        Shutdown();
    }
    size = size_;
    ExtraSize = ExtraSize_;
    size.x += ExtraSize.x;
    size.y += ExtraSize.y;

    sizeX = (size.x )/dotSize;
    sizeY = (size.y )/dotSize;
    blockSize = dotSize;
    MaxDarkness = maxDarkness;
    out = new SmartTexture(0,0,sizeX,sizeY,true,true);
    ShadeMap = new uint8_t[sizeY*sizeX];
    DataMap = new uint8_t[sizeY*sizeX];
    pix = out->GetPixels();
    for (int y=0;y<sizeY;y++){
        /*ShadeMap[y] = new uint8_t[sizeX];
        DataMap[y] = new uint8_t[sizeX];
        */
        for (int x=0;x<sizeX;x++){
            ShadeMap[x + y * sizeX] = MaxDarkness;
            DataMap[x + y * sizeX] = 0;
            pix[y * (sizeX) + x] = RenderHelp::FormatRGBA(255,0,0,0);
        }
    }


    out->UpdateTexture();

    if (MapMap == nullptr){
        maxAlloc=2;
        MapMap = new uint8_t*[2];
        for (int i=0;i<2;i++){
            MapMap[i] = new uint8_t[sizeY*sizeX];
            for (int y=0;y<sizeY;y++){
                for (int x=0;x<sizeX;x++){
                    MapMap[i][y * sizeX+  x] = MaxDarkness;
                }
            }
        }
    }
    LightPoints = 0;
    CurrentAlloc = 0;

    MaxCycles = 90;
    Permissive = permissive;
    LightJobs = 0;
    return true;
}
bool Light::IsInLimits(int x,int y){
    if(!IsStarted()){
        return false;
    }
    if (x < 0 || x >= sizeX || y < 0 || y >= sizeY){
        return false;
    }
    return true;
}
void Light::AddBlockM(int x,int y,unsigned char strenght){
        if(!IsStarted()){
            return;
        }
        x = ( (x-floor(((int)Camera::pos.x/blockSize)*blockSize) + ExtraSize.x/2 -blockSize)/blockSize );
        y = ( (y-floor(((int)Camera::pos.y/blockSize)*blockSize) + ExtraSize.y/2 -blockSize)/blockSize );
        if (IsInLimits(x,y) && DataMap[y * sizeX+  x] == 0){
            DataMap[y * sizeX+  x] = strenght;
        }

}

void Light::AddBlock(Rect r,uint8_t strenght){
    if(!IsStarted()){
        return;
    }
    int x = ( (r.x-floor(((int)Camera::pos.x/blockSize)*blockSize) + ExtraSize.x/2 -blockSize)/blockSize );
    int y = ( (r.y-floor(((int)Camera::pos.y/blockSize)*blockSize) + ExtraSize.y/2 -blockSize)/blockSize );
    r.w /= blockSize;
    r.h /= blockSize;
    for (int w = 0;w<r.w;w++){
        for (int h = 0;h<r.h;h++){
            if (IsInLimits(x+w,y+h) && DataMap[(y+h)*sizeX +  x+w] == 0){
                DataMap[(y+h)*sizeX +  x+w] = strenght;
            }
        }
    }
}
bool Light::AddLightM(int x, int y,uint8_t strenght){
        if(!IsStarted()){
            return false;
        }
        x = ( (x-floor(((int)Camera::pos.x/blockSize)*blockSize) + ExtraSize.x/2 -blockSize)/blockSize );
        y = ( (y-floor(((int)Camera::pos.y/blockSize)*blockSize) + ExtraSize.y/2 -blockSize)/blockSize );

        if (IsInLimits(x,y)){
            strenght = ( (strenght/2) & 127) -1;;
                uint8_t* VartoRet = GiveAnAdderess();
            VartoRet[y * sizeX+  x] = VartoRet[y * sizeX+  x] - strenght*2;
            Job j1(JOB_SHADE,x,y,0,2,VartoRet,strenght);
            Job j2(JOB_SHADE,x,y,1,2,VartoRet,strenght);
            LightJobs += 2;
            ThreadPool::GetInstance().AddJob_(j1);
            ThreadPool::GetInstance().AddJob_(j2);
            return true;
        }
    return false;
}

uint8_t *Light::GiveAnAdderess(bool clear){
    if(!IsStarted()){
        return nullptr;
    }
    uint8_t* VartoRet= MapMap[CurrentAlloc];
    CurrentAlloc++;

    if (CurrentAlloc >= maxAlloc){
        int addSize = 4;
        uint8_t ** MAux = new uint8_t *[maxAlloc + addSize];
        for (int i=0;i<maxAlloc;i++){
            MAux[i] = MapMap[i];
        }
        delete [] MapMap;
        MapMap = MAux;
        for (int i=maxAlloc;i<maxAlloc+addSize;i++){
            MAux[i] = new uint8_t[sizeY*sizeX];
            for (int y=0;y<sizeY;y++){
                for (int x=0;x<sizeX;x++){
                    MAux[i][y * sizeX+  x] = 0;
                }
            }
        }
        maxAlloc += addSize;
        VartoRet= MapMap[CurrentAlloc-1];
    }
    for (int y=0;y<sizeY;y++){
        for (int x=0;x<sizeX;x++){
            VartoRet[y * sizeX+  x] = MaxDarkness;
        }
    }

    return VartoRet;
}

void Light::WaitDone(){
    if(!IsStarted()){
        return;
    }
    #ifndef DISABLE_THREADPOOL
    if (onAutomatic){
        float st = SDL_GetTicks();
        while(pthread_mutex_trylock(&CanRender)){
            if (SDL_GetTicks()-st > 1000){
                pthread_mutex_unlock(&Critical);
                break;
            }
        }
    }else{
        ThreadPool::GetInstance().Lock();
    }
    #endif
}
void Light::Update(float dt,LightStep step){
    if(!IsStarted()){
        return;
    }
    if (step == LIGHT_BEGIN){
        LightPoints = 0;
        CurrentAlloc = 0;
        LightJobs = 0;
        pix = out->GetPixels();
        for (int y=0;y<sizeY;y++){
            for (int x=0;x<sizeX;x++){
                ShadeMap[y * sizeX+  x] = MaxDarkness;
                DataMap[y * sizeX+  x] = 0;
            }
        }
    }else if (step == LIGHT_SHADE){
        ThreadPool::GetInstance().SpreadJobs();
        ThreadPool::GetInstance().Unlock();
    }else if (step == LIGHT_REDUCE){
        int pthreads = ThreadPool::GetInstance().GetPthreads();

        ThreadPool::GetInstance().Lock();
        for (int i=0;i<pthreads;i++){
            Job j(JOB_REDUCE,i * (sizeY/(float)pthreads),(i+1) * (sizeY/(float)pthreads));
            ThreadPool::GetInstance().AddJob_(j);
        }
        ThreadPool::GetInstance().SpreadJobs();
        ThreadPool::GetInstance().Unlock();

    }else if(step == LIGHT_GEN){
        LightJobs = 0;
        ThreadPool::GetInstance().Lock();
        int pthreads = ThreadPool::GetInstance().GetPthreads();
        for (int i=0;i<pthreads;i++){
            Job j(JOB_GEN,i * (sizeY/(float)pthreads),(i+1) * (sizeY/(float)pthreads));
            ThreadPool::GetInstance().AddJob_(j);
            LightJobs++;
       }
       ThreadPool::GetInstance().SpreadJobs();
       ThreadPool::GetInstance().Unlock();
       CurrentAlloc = 0;
    }else if(step == LIGHT_AUTO){
        onAutomatic = true;
        AutoFinished = false;
        if (LightJobs == 0){
            Update(dt,LIGHT_GEN);
            doneAuto = true;
        }else{
            Update(dt,LIGHT_SHADE);
        }

    }

}

void Light::Render(Point pos){
    if(!IsStarted()){
        return;
    }
    out->UpdateTexture();
    int extraX = ((int)(floor(Camera::pos.x))%blockSize);
    int extraY = ((int)(floor(Camera::pos.y))%blockSize);
    double scaleRatioW = ScreenManager::GetInstance().GetScaleRatioW();
    double scaleRatioH = ScreenManager::GetInstance().GetScaleRatioH();

    SDL_Rect dimensions2;
    dimensions2.x = pos.x + ScreenManager::GetInstance().GetOffsetW() - (extraX + ExtraSize.x/2)*scaleRatioW;
    dimensions2.y = pos.y -(extraY + ExtraSize.y/2  )*scaleRatioH;

    dimensions2.h = size.y*scaleRatioH;
    dimensions2.w = size.x*scaleRatioW;
    SDL_RenderCopyEx(BearEngine->GetRenderer(),out->GetTexture(),nullptr,&dimensions2,0,nullptr,SDL_FLIP_NONE);

}
