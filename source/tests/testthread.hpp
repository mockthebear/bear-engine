#include "../settings/definitions.hpp"
#ifdef RUN_TESTS



#include "../engine/genericstate.hpp"
#ifndef DISABLE_THREADPOOL
#ifndef THREADPOOLTEST
#define THREADPOOLTEST


#include <random>

#include "../game/state.hpp"
#include "../performance/console.hpp"
#include "../performance/graph.hpp"
#include "../framework/threadpool.hpp"
#include "../engine/timer.hpp"
#include "../engine/screenmanager.hpp"
#include "../input/inputmanager.hpp"

void job(int from,int to,void*);

class Test_Threadpool: public State{
    public:
        Test_Threadpool(){
            requestQuit = requestDelete = false;
            state = 0;
            iterations = 80000000;
            inside = 0;

        };
        ~Test_Threadpool(){

        };
        void Begin(){
            ScreenManager::GetInstance().SetScreenName("Test thread");
            bear::out << "[Threadpool test]\n";
            timer.Start(Point(400,400),32,false);


            ThreadPool::GetInstance().KillThreads();
            bear::out << "[Killed all threads. Starting]\n";
        };


        void LaunchTest(uint32_t threads){
            bear::out << "["<<threads<<"] Threads.\n";
            inside = 0;
            ThreadPool::GetInstance().Begin(threads);
            ThreadPool::GetInstance().AddParallelFor(job,0,iterations);
            ThreadPool::GetInstance().SpreadJobs();
            sw.Reset();
            ThreadPool::GetInstance().Unlock();
            ThreadPool::GetInstance().Lock();
            float dur = sw.Get();
            timer.AddBar(utils::format("%d",threads),{Uint8(state%2 == 0 ? 0 : 255),Uint8(state%2 == 0 ? 255 : 0),Uint8(state%3 == 0 ? 255 : 0),Uint8(255)},dur);
            ThreadPool::GetInstance().KillThreads();
            bear::out << "Pi is: "<< (inside/(double)iterations)*4.0 << "\n";
        }



        void Update(float dt){



            if (state == 0){
                LaunchTest(1);
                state = 1;
            }else if(state == 1){
                LaunchTest(2);
                state = 2;
            }else if(state == 2){
                LaunchTest(4);

                state = 3;
            }else if(state == 3){
                LaunchTest(8);

                state = 4;
            }else if(state == 4){
                LaunchTest(16);

                state = 5;
            }else if(state == 5){
                LaunchTest(32);
                duration = 10.0f;
                state = 6;
            }else if(state == 6){
                duration -= dt;
                if( InputManager::GetInstance().IsAnyKeyPressed() != -1 || duration <= 0 ) {
                    requestDelete = true;
                }
            }
        };
        void Render(){
            timer.Render(Point(132,32));
        };
        void Input();

        void Resume(){};
        void End(){

        };
        static unsigned int inside;
        static int iterations;
    private:
        Stopwatch sw;
        Graph timer;
        float duration;
        int state;
};

#endif // THREADPOOLTEST
#endif // DISABLE_THREADPOOL
#endif // RUN_TESTS
