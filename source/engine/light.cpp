#include "light.hpp"
#include "camera.hpp"
#include "screenmanager.hpp"
#include "gamebase.hpp"
#include "../framework/threadpool.hpp"
#include "renderhelp.hpp"
#include "../performance/console.hpp"

Light* Light::Slight = NULL;
#ifndef DISABLE_THREADPOOL
pthread_mutex_t Light::GCritical;
#endif

Light::Light(){
    out = NULL;
    #ifndef DISABLE_THREADPOOL
    pthread_mutex_init(&Critical,NULL);
    pthread_mutex_init(&CanRender,NULL);
    pthread_mutex_init(&JobInterval,NULL);
    pthread_mutex_lock(&CanRender);
    #endif
    LightJobs = 0;
    MaxDarkness = 200;
}
Light* Light::Startup(){
    #ifndef DISABLE_THREADPOOL
    pthread_mutex_init(&GCritical,NULL);
    #endif // DISABLE_THREADPOOL
    return GetInstance();
}
Light* Light::GetInstance(){
    #ifndef DISABLE_THREADPOOL
    pthread_mutex_lock(&GCritical);
    #endif
    if (Slight == NULL){
        Slight = new Light();
    }
    #ifndef DISABLE_THREADPOOL
    pthread_mutex_unlock(&GCritical);
    #endif
    return Slight;
}


int Light::GetAround(unsigned char** map,int x,int y){

    float n = 0;
    double k = 0;

    for (int ay=-1;ay<=1;ay++){
         for (int ax=-1;ax<=1;ax++){
            if (IsInLimits(x+ax,y+ay)){
                n += map[y+ay][x+ax]*map[y+ay][x+ax];
                k++;
            }
        }
    }
    return sqrt(n/k);
}
void Light::Gen(parameters *P,Job &j){
    for (int y=j.from;y<j.to;y++){
        for (int x=0;x<sizeX;x++){
            pix[y*sizeX + x ] = RenderHelp::FormatARGB(GetAround(ShadeMap,x,y),0,0,0);
        }
    }
     if (onAutomatic){
        #ifndef DISABLE_THREADPOOL
        pthread_mutex_lock(&Critical);
        #endif
        LightJobs--;
        if (LightJobs == 0){
            #ifndef DISABLE_THREADPOOL
            pthread_mutex_unlock(&CanRender);
            #endif
        }
        #ifndef DISABLE_THREADPOOL
        pthread_mutex_unlock(&Critical);
        #endif
    }

}
void Light::Reduce(parameters *P,Job &j){

    for (int y=j.from;y<j.to;y++){
        for (int x=0;x<sizeX;x++){
            unsigned char MPP = ShadeMap[y][x];
            unsigned char K;
            for (int i=0;i<CurrentAlloc;i++){
                K =  MapMap[i][y][x]; //Segundo borramento!
                if (MPP < MaxDarkness){
                    int offset = MaxDarkness-MPP;
                    K = std::max(K-offset,0);
                }
                MPP = std::min(MPP,K);
            }
            ShadeMap[y][x] = MPP;
            pix[y*sizeX + x ] = ShadeMap[y][x];

        }
    }
    if (onAutomatic){
        #ifndef DISABLE_THREADPOOL
        pthread_mutex_lock(&Critical);
        #endif
        LightJobs--;
        if (LightJobs == 0){
            int pthreads = ThreadPool::GetInstance().GetPthreads();
            for (int i=0;i<pthreads;i++){
                Job j(JOB_GEN,i * (sizeY/(float)pthreads),(i+1) * (sizeY/(float)pthreads));
                ThreadPool::GetInstance().AddJob_(j,true);
                LightJobs++;
            }
        }
        #ifndef DISABLE_THREADPOOL
        pthread_mutex_unlock(&Critical);
        #endif
    }
}
void Light::Shade(parameters *P,Job &j){

    int dis = j.me;
    int Threads = j.tsize;
    unsigned char strenght = j.bright;
    int x = j.from;
    int y = j.to;
    int i,max;
    i = dis * (MaxCycles/Threads);
    max = (dis+1) * (MaxCycles/Threads);
    for (;i<max;i++){
        float STR = (strenght&127)*2;
        int lsx=0,lsy=0;
        j.vect[y][x] = 0;
        for (float step=0;STR>0;step += 0.5){
            int sx = sin(i * (360/(float)MaxCycles) * M_PI / 180.0)*step;
            int sy = cos(i * (360/(float)MaxCycles) * M_PI / 180.0)*step;
            if (lsx == (int)sx and lsy == (int)sy){
                STR -= Permissive;
                continue;
            }
            lsx = sx;
            lsy = sy;
            if (IsInLimits(x+sx,y+sy)){
                if (DataMap[y+sy][x+sx]){
                    //Block
                    unsigned char Lum = MaxDarkness;
                    STR -= STR* ( (float)DataMap[y+sy][x+sx] /255.0f);
                    Lum = std::max(0,  (Lum)-(int)(STR));
                    j.vect[y+sy][x+sx] = Lum;
                }else{
                    unsigned char Lum = MaxDarkness;
                    Lum = std::max(0,  (Lum)-(int)(STR));
                    j.vect[y+sy][x+sx] = Lum;
                    //ShadeMap[y+sy][x+sx] = Lum;

                }
            }
            STR -= Permissive;

        }
    }

    if (onAutomatic){
        #ifndef DISABLE_THREADPOOL
        pthread_mutex_lock(&Critical);
        #endif
        LightJobs--;
        if (LightJobs == 0){
            int pthreads = ThreadPool::GetInstance().GetPthreads();
            for (int i=0;i<pthreads;i++){
                Job j(JOB_REDUCE,i * (sizeY/(float)pthreads),(i+1) * (sizeY/(float)pthreads));
                ThreadPool::GetInstance().AddJob_(j,true);
                LightJobs++;
            }
        }
        #ifndef DISABLE_THREADPOOL
        pthread_mutex_unlock(&Critical);
        #endif

    }
}


bool Light::StartLights(Point size_,Point ExtraSize_,uint16_t dotSize,float permissive,uint16_t maxDarkness){
    size = size_;
    ExtraSize = ExtraSize_;
    size.x += ExtraSize.x;
    size.y += ExtraSize.y;

    sizeX = (size.x )/dotSize;
    sizeY = (size.y )/dotSize;
    blockSize = dotSize;
    MaxDarkness = maxDarkness;
    out = new SmartTexture(0,0,sizeX,sizeY,true,true);

    ShadeMap = new uint8_t*[sizeY];
    DataMap = new uint8_t*[sizeY];
    pix = out->GetPixels();
    for (int y=0;y<sizeY;y++){
        ShadeMap[y] = new uint8_t[sizeX];
        DataMap[y] = new uint8_t[sizeX];
        for (int x=0;x<sizeX;x++){
            ShadeMap[y][x] = MaxDarkness;
            DataMap[y][x] = 0;
            pix[y * (sizeX) + x] = RenderHelp::FormatARGB(100,0,0,0);
        }
    }


    out->UpdateTexture();

    maxAlloc=2;
    MapMap = new uint8_t**[2];
    for (int i=0;i<2;i++){
        MapMap[i] = new uint8_t*[sizeY];
        for (int y=0;y<sizeY;y++){
            MapMap[i][y] = new uint8_t[sizeX];
            for (int x=0;x<sizeX;x++){
                MapMap[i][y][x] = MaxDarkness;
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
    if (x < 0 or x >= sizeX or y < 0 or y >= sizeY){
        return false;
    }
    return true;
}
void Light::AddBlockM(int x,int y,unsigned char strenght){

        x = ( (x-floor(((int)Camera::pos.x/blockSize)*blockSize) + ExtraSize.x/2 -blockSize)/blockSize );
        y = ( (y-floor(((int)Camera::pos.y/blockSize)*blockSize) + ExtraSize.y/2 -blockSize)/blockSize );
        if (IsInLimits(x,y) && DataMap[y][x] == 0){
            DataMap[y][x] = strenght;
        }

}
bool Light::AddLightM(int x, int y,uint8_t strenght){
        x = ( (x-floor(((int)Camera::pos.x/blockSize)*blockSize) + ExtraSize.x/2 -blockSize)/blockSize );
        y = ( (y-floor(((int)Camera::pos.y/blockSize)*blockSize) + ExtraSize.y/2 -blockSize)/blockSize );

        if (IsInLimits(x,y)){
            strenght = ( (strenght/2) & 127) -1;;
            uint8_t** VartoRet = GiveAnAdderess();
            VartoRet[y][x] = VartoRet[y][x] - strenght*2;
            Job j1(JOB_SHADE,x,y,0,2,VartoRet,strenght);
            Job j2(JOB_SHADE,x,y,1,2,VartoRet,strenght);
            LightJobs += 2;
            ThreadPool::GetInstance().AddJob_(j1);
            ThreadPool::GetInstance().AddJob_(j2);
            return true;
        }
    return false;
}

uint8_t **Light::GiveAnAdderess(bool clear){
    uint8_t** VartoRet= MapMap[CurrentAlloc];
    CurrentAlloc++;

    if (CurrentAlloc >= maxAlloc){
        int addSize = 4;
        uint8_t *** MAux = new uint8_t **[maxAlloc + addSize];
        for (int i=0;i<maxAlloc;i++){
            MAux[i] = MapMap[i];
        }
        delete MapMap;
        MapMap = MAux;
        for (int i=maxAlloc;i<maxAlloc+addSize;i++){
            MAux[i] = new uint8_t*[sizeY];
            for (int y=0;y<sizeY;y++){
                MAux[i][y] = new uint8_t[sizeX];
                for (int x=0;x<sizeX;x++){
                    MAux[i][y][x] = 0;
                }
            }
        }
        maxAlloc += addSize;
    }
    for (int y=0;y<sizeY;y++){
        for (int x=0;x<sizeX;x++){
            VartoRet[y][x] = MaxDarkness;
        }
    }
    return VartoRet;
}

void Light::WaitDone(){
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
    if (step == LIGHT_BEGIN){
        LightPoints = 0;
        CurrentAlloc = 0;
        LightJobs = 0;
        pix = out->GetPixels();
        for (int y=0;y<sizeY;y++){
            for (int x=0;x<sizeX;x++){
                ShadeMap[y][x] = MaxDarkness;
                DataMap[y][x] = 0;
            }
        }
    }else if (step == LIGHT_SHADE){
        ThreadPool::GetInstance().Unlock();
    }else if (step == LIGHT_REDUCE){
        int pthreads = ThreadPool::GetInstance().GetPthreads();

        ThreadPool::GetInstance().Lock();
        for (int i=0;i<pthreads;i++){
            Job j(JOB_REDUCE,i * (sizeY/(float)pthreads),(i+1) * (sizeY/(float)pthreads));
            ThreadPool::GetInstance().AddJob_(j);
        }
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
    SDL_RenderCopyEx(BearEngine->GetRenderer(),out->GetTexture(),NULL,&dimensions2,0,nullptr,SDL_FLIP_NONE);

}
