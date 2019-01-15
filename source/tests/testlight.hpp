#include "../settings/definitions.hpp"
#ifdef RUN_TESTS



#include "../engine/genericstate.hpp"
#pragma once

#include "../engine/sprite.hpp"
#include "../engine/light.hpp"
#include "../engine/timer.hpp"
#include "../engine/renderhelp.hpp"
#include "../performance/graph.hpp"
#include "../performance/linegrap.hpp"
#include "../framework/threadpool.hpp"


class Test_Light: public State{
    public:
        Test_Light(){
            requestQuit = requestDelete = false;
            duration = 600.0f;
            tmr = 5.0f;
        };
        ~Test_Light(){
        };
        void Begin(){
            ScreenManager::GetInstance().SetScreenName("Test light");
            LightPixelSize = 4;

            bear::out << "Test light. Creating threads\n";
            ThreadPool::GetInstance().KillThreads();
            ThreadPool::GetInstance().Begin(4);
            bear::out << "Creating camera\n";
            Camera::Initiate(Rect(0,0,SCREEN_SIZE_W,SCREEN_SIZE_H),128,200);
            bear::out << "Setup light\n";
            Light::GetInstance()->StartLights( Point(SCREEN_SIZE_W,SCREEN_SIZE_H) ,Point(160,160) ,LightPixelSize,6.5,90);
            bear::out << "Done\n";
            Light::GetInstance()->SetLightRaysCount(220);
            bear::out << "Loading assets\n";
            if (!ResourceManager::GetInstance().Load("test.burr","light")){
                bear::out << "Could not find test.burr\n";
                getchar();
            }
            background = Assets.make<Sprite>("light:wall.jpg");
            bear::out << "Loaded background\n";
            timer.Start(Point(32,300),64,true);
            bear::out << "Start linegraph\n";
            lg = LineGraph(Point(400,100),200);

            frameRate = Text("fps: ??",28);
            timer.AddBar("Begin",{255,0,0,255},0);
            timer.AddBar("Shade and reduce",{0,255,0,255},0);
            timer.AddBar("Gen",{0,0,255,255},0);
            timer.AddBar("Render",{155,255,255,255},0);

            makeB.SetDuration(1.0);
            makeL.SetDuration(10.0);
            makeL.Enable();
            makeB.Enable();
        };

        void Update(float dt){

            sw.Reset();
            makeB.Update(dt);
            makeL.Update(dt);
            /*
                This segment reset any matrix
            */
            ThreadPool::GetInstance().ClearJobs();
            Light::GetInstance()->Update(dt,LIGHT_BEGIN);
            timer.UpdateBar(0,sw.Get());

            duration -= dt;
            if( InputManager::GetInstance().IsAnyKeyPressed() != -1 || duration <= 0 ) {
                requestDelete = true;
            }

            if (makeB.HasFinished()){
                makeB.Restart();
                staticBlock.emplace_back(Rect(rand()%SCREEN_SIZE_W,rand()%SCREEN_SIZE_H,LightPixelSize,LightPixelSize));

            }
            if (makeL.HasFinished()){
                makeL.Restart();
                LightPoints.emplace_back(std::make_tuple<Point,uint8_t>(Point(rand()%SCREEN_SIZE_W,rand()%SCREEN_SIZE_H),rand()%255 ));

            }

            Point p = InputManager::GetInstance().GetMouse();
            Light::GetInstance()->AddLightM(p.x,p.y,255);
            for (auto &it : LightPoints){
                Light::GetInstance()->AddLightM(std::get<0>(it).x,std::get<0>(it).y,std::get<1>(it));
            }
            for (auto &it : staticBlock){
                Light::GetInstance()->AddBlock(it,255);
            }

            sw.Reset();
            /*
                Here its just an threadpool unlock.
            */
            Light::GetInstance()->Update(dt,LIGHT_SHADE);
            frameRate.SetText(utils::format("fps: %d",(int)ScreenManager::GetInstance().GetFps()));
            tmr -= dt;
            if (tmr <= 0){
                tmr = 5.0f;
                lg.AddData(ScreenManager::GetInstance().GetFps());

            }

        };
        void Render(){
            /*
                Lock threads and add matrix reduction jobs
            */
            Light::GetInstance()->Update(0,LIGHT_REDUCE);
            timer.UpdateBar(1,sw.Get());
            /*
                Render scenario
            */

            background.Render(0,0);
            RenderInstances();
            frameRate.Render(32,32);

            for (auto &it : staticBlock){
                RenderHelp::DrawSquareColor(it,0,0,0,255);
            }




            /*
                Lock and spread jobs to generate the final matrix
            */
            Light::GetInstance()->Update(0,LIGHT_GEN);
            sw.Reset();

            /*
                Should lock for rendering
            */
            ThreadPool::GetInstance().Lock();
            timer.UpdateBar(2,sw.Get());
            sw.Reset();




            Light::GetInstance()->Render();
            timer.UpdateBar(3,sw.Get());
            timer.Render(Point(32,64));
            lg.Render(Point(32,400));
        };
        void Input();
        void Resume(){};
        void End(){
            Light::GetInstance()->Shutdown();
            ResourceManager::GetInstance().Erase("light");
        };
    private:
        float tmr;
        Timer makeB;
        Timer makeL;
        Stopwatch sw;
        Graph timer;
        Text frameRate;
        LineGraph lg;
        Sprite background;
        std::vector<std::tuple<Point,uint8_t>> LightPoints;
        std::vector<Rect> staticBlock;
        float duration;
        uint32_t LightPixelSize;
};

#endif // RUN_TESTS
