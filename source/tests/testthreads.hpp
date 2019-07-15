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
        };

        void End(){
            bear::out << "Terminating threads\n";
            ThreadPool::GetInstance().Stop();
            bear::out << "Done\n";
        };

        void Update(float dt){
            duration -= dt;
            /*if( InputManager::GetInstance().IsAnyKeyPressed() != -1 || duration <= 0 ) {
                requestDelete = true;
            }*/
            if (g_input.KeyPress(SDLK_t)){
                ThreadPool::GetInstance().enqueue([](){
                    uint32_t cnt = 0;
                    std::cout << "Begin\n";
                    for (long int i=0;i<100000000;i++){
                        ++cnt;
                    }
                    std::cout << "end\n";
                    return cnt;
                });
            }
            if (g_input.KeyPress(SDLK_r)){
                ThreadPool::GetInstance().enqueue([this](int uwu){
                    this->perc = 1.0f;
                    long int imax = 100000000;
                    for (long int i=0;i<imax;i++){
                        this->perc = i / (float)imax;
                    }
                },2);
            }

        };
        void Render(){
            uint32_t cnt = ThreadPool::GetInstance().CountBusy();
            float usage = cnt / 4.0f;
            RenderHelp::DrawCircleColor(CircleColor(98,220,32,255 * usage,(1.0f-usage) * 255,0,100) );
            RenderHelp::DrawCircleColor(CircleColor(100,100,int(100 * perc) ,255,  255,255,255) );
        };
        void Input();
        void Resume(){};

    private:
        float perc;
        float duration;
};

#endif // DISABLE_UNIT_TESTS

