// *** ADDED BY HEADER FIXUP ***
#include <ctime>
// *** END ***
#include "../settings/definitions.hpp"

#ifndef SHADER_
#define SHADER_
#include <stack>
#include <queue>
#ifndef DISABLE_THREADPOOL
#include <pthread.h>
#include <semaphore.h>
#endif
#include <functional>
#include <time.h>




enum JOBTYPE{
    JOB_NOTHING, //0
    JOB_KILL,    //1
    JOB_LAMBDA, //2
    JOB_FOR,    //3
    //Light
    JOB_SEARCH, //4
    JOB_SHADE, //5
    JOB_REDUCE, //6
    JOB_GEN, //7

    //Auto light
    JOB_SEARCH_AUTO,
    JOB_SHADE_AUTO,
    JOB_REDUCE_AUTO,
    JOB_GEN_AUTO,
};
/**
 * @brief Class to manage Jobs at the class ThreadPool
 *
 * Internal use of ThreadPool
*/
class Job{
    public:
        Job(){
            lambda = [](int,int,void*){};
            Type = JOB_NOTHING;
            Parameter=nullptr;
            min=max=0;
            vect=nullptr;
            from=to=0;
            tsize=0;bright=0;
            empty = false;
        };
        Job(JOBTYPE t):Job(){Type = t;};
        Job(std::function<void(int,int,void*)> F):Job(){Type = JOB_LAMBDA;lambda=F;Parameter=nullptr;};
        Job(std::function<void(int,int,void*)> Reduce,int min_p,int max_p):Job(){
            Type = JOB_FOR;
            lambda=Reduce;
            Parameter=nullptr;
            min = min_p;
            max = max_p;
        };

        Job(JOBTYPE t,int i,int b):Job(){Type = t;from = i;to = b;};
        Job(JOBTYPE t, int i, int b, int d,int ts, uint8_t* V, unsigned char br):Job(){Type = t;from = i;to = b;me=d;vect=V;bright=br;tsize=ts;};
        bool empty;
        JOBTYPE Type;
        std::function<void(int,int,void*)> lambda;
        void *Parameter;
        int min, max;
        uint8_t* vect;
        int from,to,me,tsize;
        uint8_t bright;
};

typedef struct{
    int id;
    int Threads;
    void *me;
    bool working;
    bool Begin;
    bool mainThread;
    Job todo;
    std::stack<Job> MyJobs;
} parameters;

/**
 * @brief Singleton Thread pool implementation
 *
 * Bear Engine also came with a thread pool
 * The default amount of threads running is 4, you can change in definitions.hpp
 * Also is good disable by writing
 @code
 #define DISABLE_THREADPOOL
 @endcode
 * on definitions.hpp when you dont need use it.
 * <b>Thread pool is not suported on web version!</b>
 *
 *
 * You should dont use this unless you know how to work with threads.
 * Why threadpool and not simple use things like:
 @code
 #pragma omp parallel for
    for (int i=0;objCount;i++)
        objList[i]->Update(dt);
 @endcode
 * The main reason is because most game objects need to check collision, other objects states and so on
 * You will need some data, and this can't cant allow you to make parallel stuff.
 * The other reason its simple, your game running at 60 fps. This means you re run the game loop
 * 60 times per second. If you just set an OMP code in the for, you are creating and destroying threads
 * like 60 per second. Create a thread cost too much from your device.
 * This is why thread pools. It simple create some threads, and lock it until you need to use.
 * You can also kill the threads, and when you enter in a stage of the game that you will need the threads
 * all the time, you simply create once and let it running.
 *
 * One thing you can process parallel is the particles.
 *
 * The ThreadPool work with Jobs and Workers. There is an algorythm to feed the workers with jobs.
 *
 @code
         void SomeState::Update(float dt){
            ThreadPool::GetInstance().AddParallelFor([=](int t,int mt,void *index){
                int i = *index;
                ParticlePool->GetInstance(i)->Update(dt);
            },0,ParticlePool->GetMaxInstances());
            //Dont deploy the threads yet
            //You may need add one or two
            YourGameLoop(dt);
            ThreadPool::GetInstance().Unlock();
            //Do things...
        } //Function end

        void SomeState::Render(){
            //Once update runs first, then render, you can unlock at the middle of the update. make some things
            //The simply unlock at the middle of render
            RenderObjects();
            ThreadPool::GetInstance().Lock();
            ParticlePool->Render();
        }
 @endcode
 */
class ThreadPool{
    public:
        /**
            Empty\n
        */

        ThreadPool()
        #ifdef USE_LOCK_FREE_STACK
            :Jobs(4,16)
        #endif
        {started=false;Locked=false;fastJobs=nullptr;UsePThreads=4;Params=nullptr;};
        /** @brief This is created in the singleton\n

            @param threads Default is 4

        */
        ThreadPool(int threads);

        bool Begin(int threads);
        /**
            Auto
        */
        ~ThreadPool();
        /** @brief When this is called for the first time at gamebase.cpp.
            *
            * The singleton are made
        */
        static ThreadPool& GetInstance(int threads = 4);
        /** @brief *Singleton
            *
            *Add an lambda function (c++11) as a job.\n
            @param lamba This means void function(int threadId,int maxThreads, void *JobParameter);
        */
        void AddLambdaJob(std::function<void(int,int,void*)> lamba);
        /** @brief Add an lambda function (c++11) as a job.\n
            *
            *Actually this is an order to add some jobs to make an parallel for\n
            @param lamba This means void function(int threadId,int maxThreads, void *Index);
            @param min The value that starts
            @param max The value that ends
            @code
            ThreadPool::GetInstance().AddParallelFor([=](int t,int mt,void *index){
                int i = *index;
                std::cout << "Hi! this is iteration "<<i<<" from thread "<<t << "\n";
            },0,20);
            @endcode
        */
        void AddParallelFor(std::function<void(int,int,void*)> lamba,int min,int max,int jobs=-1);
        /**
            @brief If there is any job undone yet, it removes.
        */
        void ClearJobs();

        void CriticalLock();
        void CriticalUnLock();

        /**
            Dont do anything yet
        */
        void Render();
        /** @brief Lock the main thread until all threads in ThreadPool.
            *
            *Is like the function pthread_join, but dont kill the thread.
            Check ThreadPool::Unlock to unlock them and for examples
        */
        bool IsLocked(){return Locked;};
        bool TryLock();

        void Lock();
        /** @brief Used to deploy threads.
            *
            *Lets say you added 10 Jobs, then you call Unlock to make all\n
            *Threads run.\n
            *To make the main threads wait until everything finishes\n
            *Check ThreadPool::Lock\n
            *\n
            *Why use?\n
            *Lets say you added the code from an ThreadPool::AddParallelFor\n
            *Then you set it to keep running until you finish your game loop stuff.\n
            @code
            ThreadPool::GetInstance().AddParallelFor([=](int t,int mt,void *index){
                int i = *index;
                std::cout << "Hi! this is iteration "<<i<<" from thread "<<t << "\n";
            },0,20);
            ThreadPool::GetInstance().Unlock();
            makeStuff();
            doStuff();
            checkStuff();
            foo();
            bar();
            ThreadPool::GetInstance().Lock();
            @endcode
        */
        void Unlock();
        bool Help();
        /** @brief Dont do anything yet

        */


        void Update(float dt);
        /** @brief Get the amount of threads

        */
        int GetPthreads(){return UsePThreads;};
        /** @brief When you dont need threads, you can simply kill all of them.\n
            *
            *Check ThreadPool::CreateThreads
        */
        bool KillThreads();
        /** @brief When the threads have been killed, you can create them again.
            *
            *Check ThreadPool::KillThreads\n
            *<b>IMPORTANT</b>: dont create an destroy at each game loop.\n
            *Create when you need, an big parallel for, in a game state with particles...
        */
        bool CreateThreads();

        void AddJob_(Job &j,bool ignoreFast=true);
        void AddJobStack(Job &j);

        bool IsDone();

        static void sleep(float milliseconds);

        bool SpreadJobs();

        uint32_t CountJobs();

    private:
        bool Echo;
        bool DynamicJobs;
        bool Locked;
        int scalonator;
        void AddPriorityJob(Job &j,int threadId);
        std::stack<Job> Jobs;
        Job *fastJobs;

        static void *thread_pool_worker(void *OBJ);

        bool started;
        int UsePThreads;
        parameters  *Params;
        #ifndef DISABLE_THREADPOOL
        pthread_t *thread_pool;
        sem_t *mutexes,*runningM;
        pthread_mutex_t Critical;
        #endif

};




#endif // SHADER_

