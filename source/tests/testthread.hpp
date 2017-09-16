#include "../settings/definitions.hpp"
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

class Test_Threadpool: public State{
    public:
        Test_Threadpool(){
            requestQuit = requestDelete = false;
            state = 0;
            iterations = 5000000;

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



        void Update(float dt){

            float arg = iterations;
            auto job = [arg](int from,int to,void*){


                int inside = 0;

                float Pointx,Pointy,sq;
                for (int in = from;in < to;in++){
                    Pointx = (rand()%10000000)/5000000.0f  -1;
                    Pointy = (rand()%10000000)/5000000.0f  -1;
                    sq = Pointx*Pointx + Pointy*Pointy;
                    if (sq <= 1)
                        inside++;
                }
                std::stringstream S;
                S << "From ["<<from<<":"<<to<<"] i got " << inside << "\n";
                bear::out << S.str();


            };

            if (state == 0){
                sw.Reset();
                bear::out << "[1] Threads.\n";
                job(0,iterations,nullptr);
                float dur = sw.Get();
                timer.AddBar("1",{255,0,0,255},dur);
                state = 1;
            }else if(state == 1){
                bear::out << "[2] Threads.\n";
                ThreadPool::GetInstance().Begin(2);
                ThreadPool::GetInstance().AddParallelFor(job,0,iterations);
                ThreadPool::GetInstance().SpreadJobs();
                sw.Reset();
                ThreadPool::GetInstance().Unlock();
                ThreadPool::GetInstance().Lock();
                float dur = sw.Get();
                timer.AddBar("2",{255,100,0,255},dur);
                ThreadPool::GetInstance().KillThreads();
                state = 2;
            }else if(state == 2){
                bear::out << "[4] Threads.\n";
                ThreadPool::GetInstance().Begin(4);
                ThreadPool::GetInstance().AddParallelFor(job,0,iterations);
                ThreadPool::GetInstance().SpreadJobs();
                sw.Reset();
                ThreadPool::GetInstance().Unlock();
                ThreadPool::GetInstance().Lock();
                float dur = sw.Get();
                timer.AddBar("4",{255,200,0,255},dur);
                ThreadPool::GetInstance().KillThreads();

                state = 3;
            }else if(state == 3){
                bear::out << "[8] Threads.\n";
                ThreadPool::GetInstance().Begin(8);
                ThreadPool::GetInstance().AddParallelFor(job,0,iterations);
                ThreadPool::GetInstance().SpreadJobs();
                sw.Reset();
                ThreadPool::GetInstance().Unlock();
                ThreadPool::GetInstance().Lock();
                float dur = sw.Get();
                timer.AddBar("8",{255,200,0,255},dur);
                ThreadPool::GetInstance().KillThreads();

                state = 4;
            }else if(state == 4){
                bear::out << "[16] Threads.\n";
                ThreadPool::GetInstance().Begin(16);
                ThreadPool::GetInstance().AddParallelFor(job,0,iterations);
                ThreadPool::GetInstance().SpreadJobs();
                sw.Reset();
                ThreadPool::GetInstance().Unlock();
                ThreadPool::GetInstance().Lock();
                float dur = sw.Get();
                timer.AddBar("16",{0,200,0,255},dur);
                ThreadPool::GetInstance().KillThreads();

                state = 5;
            }else if(state == 5){
                bear::out << "[32] Threads.\n";
                ThreadPool::GetInstance().Begin(32);
                ThreadPool::GetInstance().AddParallelFor(job,0,iterations);
                ThreadPool::GetInstance().SpreadJobs();
                sw.Reset();
                ThreadPool::GetInstance().Unlock();
                ThreadPool::GetInstance().Lock();
                float dur = sw.Get();
                timer.AddBar("32",{0,200,0,255},dur);
                ThreadPool::GetInstance().KillThreads();
                duration = 10.0f;
                state = 6;
            }else if(state == 6){
                duration -= dt;
                if( InputManager::GetInstance().IsAnyKeyPressed() || duration <= 0 ) {
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
    private:
        Stopwatch sw;
        Graph timer;
        float duration;
        int iterations;
        int state;
};

#endif // THREADPOOLTEST
#endif // DISABLE_THREADPOOL
