#include "../settings/definitions.hpp"
#ifndef DISABLE_UNIT_TESTS



#include "../engine/genericstate.hpp"
#include "../framework/threadpool.hpp"
#pragma once



class Test_Thread: public State{
    public:
        Test_Thread(){
            requestQuit = requestDelete = false;
            bear::out << "Test thread\n";
            duration = 10000.0f;
            perc = 1.0f;
        };
        ~Test_Thread(){

        };
        void Begin(){
            ScreenManager::GetInstance().SetScreenName("Test threads");
            bear::out << "Starting four threads\n";
            ThreadPool::GetInstance().Start(4);
            info = Text("Test info: press T to add a heavy job. Press R to add a visual heavy job.",19,{255, 255, 220});
            fpsInfo = Text("FPS: 000",24,{255, 255, 220});
        };

        void End(){
            bear::out << "Terminating threads\n";
            ThreadPool::GetInstance().Stop();
            bear::out << "Done\n";
        };

        void Update(float dt){
            duration -= dt;
            /**/
            if (g_input.KeyPress(SDLK_t)){
                ThreadPool::GetInstance().enqueue([](){
                    uint32_t cnt = 0;
                    for (long int i=0;i<500000000;i++){
                        ++cnt;
                    }
                    return cnt;
                });
            }else if (g_input.KeyPress(SDLK_r)){
                ThreadPool::GetInstance().enqueue([this](int uwu){
                    this->perc = 1.0f;
                    long int imax = 500000000;
                    for (long int i=0;i<imax;i++){
                        this->perc = i / (float)imax;
                    }
                },2);
            }else if( InputManager::GetInstance().IsAnyKeyPressed() != -1 || duration <= 0 ) {
                requestDelete = true;
            }

            fpsInfo.SetText(utils::format("FPS %3f", ScreenManager::GetInstance().GetFps()));

        };
        void Render(){
            uint32_t cnt = ThreadPool::GetInstance().CountBusy();
            float usage = cnt / 4.0f;
            RenderHelp::DrawCircleColor(CircleColor(98,220,32,255 * usage,(1.0f-usage) * 255,0,100) );
            RenderHelp::DrawCircleColor(CircleColor(100,100,int(100 * perc) ,255,  255,255,255) );

            info.Render(PointInt(32,280));
            fpsInfo.Render(PointInt(32,320));
        };
        void Input();
        void Resume(){};

    private:
        Text info, fpsInfo;
        float perc;
        float duration;
};

#endif // DISABLE_UNIT_TESTS

