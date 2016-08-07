#include "title.hpp"
#include <sstream>
#include <cmath>

#include "title.hpp"


#include "../engine/gamebase.hpp"
#include "../engine/particlecreator.hpp"
#include "../input/inputmanager.hpp"
#include "../framework/debughelper.hpp"
#include "../engine/renderhelp.hpp"
#include "../engine/smarttexture.hpp"
#include "../engine/smarttexturemanager.hpp"
#include "../engine/collision.h"
#include "../engine/light.hpp"
#include "../framework/threadpool.hpp"
#include "../framework/gamefile.hpp"
#include "../framework/utils.hpp"
#include "../framework/resourcemanager.hpp"



#include "controlableobject.hpp"
#include "ball.hpp"
Title::Title(){
    ParticlePool = new SPP<Particle>(1000);
    requestQuit = requestDelete = false;

    SPP<ControlableObject>* ControlableObjectPool = new SPP<ControlableObject>(10);
    PoolId co = Pool.RegisterPool(POOL_CAST(ControlableObjectPool,ControlableObject ));


    SPP<Ball>* BallPool = new SPP<Ball>(10);
    Pool.RegisterPool(POOL_CAST(BallPool,Ball ));

    std::vector<PoolId> group;
    group.emplace_back(co);

    Group = Pool.CreatePoolGroup(group);


    std::cout << Pool.GetMaxInstancesGroup(Group) << "\n";






    message = new Text("data:arial.ttf",30,TEXT_SOLID,"Hello bear. FPS: ?", {10,50,255});
    char *c = ResourceManager::GetInstance().GetFileData("data","jaaj.txt");
    std::cout <<"Text is: ["<< c << "]\n";
    c = ResourceManager::GetInstance().GetFileData("data","kotol.bin");
    int s=0;
    std::cout <<"Value is: ["<< utils::GetU16c(c,s) << "]\n";

    Camera::Initiate(Rect(0,0,SCREEN_SIZE_H,SCREEN_SIZE_W),128,200);
    Camera::speed = 0;

    InputManager::GetInstance().SetTouchOffset(Point(8,0));



    bg = new Sprite("data:wall.jpg");
    test = new Sprite("data:raccoon.png");
    sprpos = Point(400,400);
    sprpos2 = sprpos;

    Light::GetInstance()->StartLights( Point(800,600) ,Point(160,160) ,6,6.5,230);
    Light::GetInstance()->SetLightRaysCount(128);



}


void Title::Begin(){
    Pool.AddInstance(ControlableObject(200,200));
    Pool.AddInstance(ControlableObject(100,100));
    Pool.AddInstance(ControlableObject(300,300));
    Pool.AddInstance(Ball(Point(200,400)));
    int distance = 48;
    InputManager::GetInstance().CreateVirtualButton(SDLK_UP,PointInt(64,64),PointInt(200,500-distance),"Up");

    InputManager::GetInstance().CreateVirtualButton(SDLK_LEFT,PointInt(64,64),PointInt(200-distance,500),"LFT");
    InputManager::GetInstance().CreateVirtualButton(SDLK_DOWN,PointInt(64,64),PointInt(200,500+distance),"Dwn");
    InputManager::GetInstance().CreateVirtualButton(SDLK_RIGHT,PointInt(64,64),PointInt(200+distance,500),"RGHT");
    ThreadPool::GetInstance().CreateThreads();

    char *msg = SDL_GetPrefPath("tutorial","game");
    if (msg != NULL)
        Console::GetInstance().AddText(msg);
    msg = SDL_GetBasePath();
    if (msg != NULL)
        Console::GetInstance().AddText(msg);



}

Title::~Title(){
    delete message;
    Pool.ErasePools();

}


void Title::Update(float dt){

    ThreadPool::GetInstance().ClearJobs();
    Light::GetInstance()->Update(dt,LIGHT_BEGIN);
    Input();

    if( InputManager::GetInstance().IsKeyDown(SDLK_UP) )
        sprpos.y -= 15.0*dt;

    if( InputManager::GetInstance().IsKeyDown(SDLK_DOWN) )
        sprpos.y += 15.0*dt;

    if( InputManager::GetInstance().IsKeyDown(SDLK_LEFT) )
        sprpos.x -= 15.0*dt;

    if( InputManager::GetInstance().IsKeyDown(SDLK_RIGHT) )
        sprpos.x += 15.0*dt;
    Point p = InputManager::GetInstance().GetMouse();
    if( InputManager::GetInstance().MousePress(1) ){

        Sprite spp("data/sheet.png",4,10.0f,1,true);
        Particle *a = (Particle*)ParticlePool->AddInstance(Particle(p.x,p.y,spp,0,20));
        a->SetRotation(0.9);
        a->SetScaling(0.1);
    }

    Light::GetInstance()->AddLightM(p.x,p.y,255);
    Light::GetInstance()->AddBlockM(100,100,255);
    Light::GetInstance()->AddBlockM(106,100,255);
    Light::GetInstance()->AddBlockM(106,106,255);
    Light::GetInstance()->AddBlockM(100,106,255);
    Light::GetInstance()->AddBlockM(100,112,255);
    Light::GetInstance()->AddBlockM(100,118,255);
    Light::GetInstance()->AddBlockM(100,124,255);
    Light::GetInstance()->AddBlockM(100,130,255);

    Pool.Update(dt);
    Map.clear();
    Pool.PreRender(Map);
    auto ff = [=](int thread,int maxThreads,void *){
         ParticlePool->UpdateInstances(dt);
    };
    ThreadPool::GetInstance().AddLambdaJob(ff);
    Light::GetInstance()->Update(dt,LIGHT_SHADE);


    ParticlePool->PreRender(Map);
    Camera::Update(dt);
    std::stringstream Msg;
    Msg << "Hello bear. FPS: " << ScreenManager::GetInstance().GetFps();

    message->SetText(Msg.str());


    for (int i=0;i<Pool.GetMaxInstancesGroup(Group);i++){
        GameObject *obj = Pool.GetInstanceGroup(i,Group);
        if (obj != NULL && !obj->IsDead()){
            for (int e=0;e<Pool.GetMaxInstancesGroup(Group);e++){
                GameObject *obj2 = Pool.GetInstanceGroup(e,Group);
                if (obj != NULL && !obj->IsDead() && obj2 != obj){
                    if (Collision::IsColliding(obj->box,obj2->box)){
                        obj->NotifyCollision(obj2);
                    }
                }
            }
        }
    }





}

void Title::Render(){

    Light::GetInstance()->Update(0,LIGHT_REDUCE);

    RenderHelp::DrawSquareColorA(0,0,800,600,255,255,255,255);
    bg->Render(0,0);

    for (auto it=Map.begin(); it!=Map.end(); ++it){
        for (auto &k : *it->second){
            k->Render();
        }
        delete it->second;
    }

    RenderHelp::DrawSquareColorA(100,100,32,32,0,255,0,180);

    message->Render(400-message->GetWidth()/2,55+message->GetHeight());

    ThreadPool::GetInstance().Help();
    Light::GetInstance()->Update(0,LIGHT_GEN);


    //Console::GetInstance().Render();
    PointInt p = PointInt(sprpos.x,sprpos.y);

    test->Render(p);

    ThreadPool::GetInstance().Help();
    ThreadPool::GetInstance().Lock();

    Light::GetInstance()->Render();


    //Console::GetInstance().Render(Point(0,0));

}

void Title::Input(){
    if (InputManager::GetInstance().ShouldQuit()){
        Console::GetInstance().AddText("DEVE QITAR");
        requestQuit = true;
    }


    if( InputManager::GetInstance().KeyPress(SDLK_ESCAPE) ) {
        Console::GetInstance().AddText("SCAPE");
        requestQuit = true;
    }
    if( InputManager::GetInstance().KeyPress( SDLK_AC_BACK ) ) {
        Console::GetInstance().AddText("ANDLOID BAQ");
        requestQuit = true;
    }
    TouchInfo p = InputManager::GetInstance().GetFinger();

    if (p.x > 100 && p.y > 100 && p.state == JUST_PRESSED && p.x < 132 && p.y < 132){
        InputManager::GetInstance().Vibrate(100);
        Console::GetInstance().AddText("SAD");
    }


}
