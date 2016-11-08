#include "title.hpp"
#include <sstream>
#include <cmath>

#include "title.hpp"


#include "../engine/bear.hpp"
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
#include "../framework/xml.hpp"

#include "../mobile/android/jnihelper.hpp"



#include "controlableobject.hpp"
#include "ball.hpp"
Title::Title(){
    ParticlePool = new SPP<Particle>(6000);
    requestQuit = requestDelete = false;

    PoolId co = Pool.Register<ControlableObject>(10);
    Pool.Register<Ball>(10);

    std::vector<PoolId> group;
    group.emplace_back(co);

    Group = Pool.CreatePoolGroup(group);

    astar = PathFind(SCREEN_SIZE_W,SCREEN_SIZE_H);

    bg = Assets.make<Sprite>("data:wall.jpg");


    message = Text("data:arial.ttf",30,TEXT_SOLID,"Hello bear. FPS: ?", {10,50,255});
    char *c = ResourceManager::GetInstance().GetFileData("data","jaaj.txt");
    std::cout <<"Text is: ["<< c << "]\n";
    c = ResourceManager::GetInstance().GetFileData("data","kotol.bin");
    int s=0;
    std::cout <<"Value is: ["<< utils::GetU16c(c,s) << "]\n";

    Camera::Initiate(Rect(0,0,SCREEN_SIZE_W,SCREEN_SIZE_H),128,200);
    Camera::speed = 0;

    InputManager::GetInstance().SetTouchOffset(Point(8,0));




    JniHelper::CallFunction<void>(std::string("kek"),std::string("kik"),123);


}


void Title::Begin(){
    Pool.AddInstance(ControlableObject(200,200));

    Pool.AddInstance(ControlableObject(100,100));
    Pool.AddInstance(ControlableObject(300,300));
    Pool.AddInstance(Ball(Point(200,400)));



    char *msg = SDL_GetPrefPath("tutorial","game");
    if (msg != NULL)
        Console::GetInstance().AddText(msg);
    msg = SDL_GetBasePath();
    if (msg != NULL)
        Console::GetInstance().AddText(msg);




    Light::GetInstance()->StartLights( Point(SCREEN_SIZE_W,SCREEN_SIZE_H) ,Point(160,160) ,8,6.5,90);
    Light::GetInstance()->SetLightRaysCount(120);

}

Title::~Title(){
    delete ParticlePool;
    Pool.ErasePools();
    Assets.erase();
    Light::GetInstance()->Shutdown();

}


void Title::Update(float dt){

    if( InputManager::GetInstance().KeyPress(SDLK_r) ){
        requestDelete = true;
        Game::GetInstance()->AddState(new Title());
        return;
    }

    if( InputManager::GetInstance().KeyPress(SDLK_SPACE) ){
        Xml p;
        XmlNode *n = p.Parse("data.xml");
        bear::out << n->GetName() << " Has " << n->GetCount() << " members\n";
        for (auto& it : (*n)) {
            bear::out << "["<<it->GetIndex()<<"]"<<it->GetName() << " has" << it->GetCount() <<" members \n";
            if (it->GetValue().length() > 0){
                bear::out << "Eqauls to: ["<<it->GetValue()<<"]\n";
            }
            if (it->GetCount() > 0){
                for (auto& et : (*it)) {
                    bear::out << "----["<<et->GetIndex()<<"]"<<et->GetName() << " has" << et->GetCount() <<" members \n";
                    if (et->GetValue().length() > 0){
                        bear::out << "--------Eqauls to: ["<<et->GetValue()<<"]\n";
                    }
                }
            }
        }
    }
    ThreadPool::GetInstance().ClearJobs();
    Light::GetInstance()->Update(dt,LIGHT_BEGIN);
    Input();


    Point p = InputManager::GetInstance().GetMouse();
    if( InputManager::GetInstance().MousePress(3) ){
        astar.AddBlock(p.x,p.y);
        staticBlock.emplace_back(Rect(p.x,p.y,16,16));

    }
    if( InputManager::GetInstance().MousePress(1) ){
        std::stack<Point> shown = astar.Find(PointInt(32,32),PointInt(p.x,p.y));
        path.clear();
        while (shown.size() > 0){
            path.emplace_back(shown.top());
            shown.pop();
        }
        for (int i=0;i<100;i++){
            float angle = Geometry::toRad(rand()%360);
            Particle *a = (Particle*)ParticlePool->AddInstance(Particle(p.x-50 + rand()%100,p.y-50 + rand()%100,Sprite("data/spark.png",4,0.8f,1,true),0,8));
            if (a){
                a->SetPatternMoveLineFriction(Point(sin(angle)*10.0f,cos(angle)*10.0f),Point(-0.001,-0.001));
                a->SetRotation(1.9);
                a->SetScaling(0.1);
                a->SetAlphaAsDuration();
            }
        }
    }

    Light::GetInstance()->AddLightM(p.x,p.y,255);
    for (auto &it : staticBlock){
        Light::GetInstance()->AddBlock(it,255);
    }

    Light::GetInstance()->AddBlockM(100,106,255);
    Light::GetInstance()->AddBlockM(100,112,255);
    Light::GetInstance()->AddBlockM(100,118,255);

    Light::GetInstance()->AddBlockM(100 + 15*6,106,255);
    Light::GetInstance()->AddBlockM(100 + 15*6,112,255);
    Light::GetInstance()->AddBlockM(100 + 15*6,118,255);

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

    message.SetText(Msg.str());


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

    RenderHelp::DrawSquareColorA(0,0,SCREEN_SIZE_W,SCREEN_SIZE_H,255,255,255,255);
    bg.Render(0,0);
    Light::GetInstance()->Update(0,LIGHT_REDUCE);



    RenderInstances();



    //RenderHelp::DrawSquareColorA(100,100,32,32,0,255,0,180);

    message.Render(300,300,TEXT_RENDER_CENTER);

    ThreadPool::GetInstance().Help();
    Light::GetInstance()->Update(0,LIGHT_GEN);

    //if( InputManager::GetInstance().IsKeyDown(SDLK_TAB) )


    ThreadPool::GetInstance().Help();
    ThreadPool::GetInstance().Lock();




    if (path.size() > 0){
        for (auto &it : path){
            RenderHelp::DrawSquareColorA(it.x,it.y,16,16,0,255,0,180);
        }

    }
    astar.Render();
    Light::GetInstance()->Render();
    RenderWindowses();
    Console::GetInstance().Render();

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
