
#include "../settings/definitions.hpp"

#include "threadpool.hpp"
#include "../engine/light.hpp"
#include "../framework/utils.hpp"

#include "../settings/configmanager.hpp"
#include "../performance/console.hpp"
#include "../engine/gamebase.hpp"
#include <math.h>
#include <stdlib.h>
#include <pthread.h>

#include <iostream>





ThreadPool& ThreadPool::GetInstance(int threads){
    static ThreadPool teste(threads);
    return teste;
}

void ThreadPool::sleep(float milliseconds) // cross-platform sleep function
{
    clock_t time_end;
    time_end = clock() + milliseconds * CLOCKS_PER_SEC/1000;
    while (clock() < time_end){}
}
ThreadPool::ThreadPool(int threads){

    Light::Startup();
    #ifndef DISABLE_THREADPOOL
    UsePThreads=threads;
    thread_pool = new  pthread_t[UsePThreads];
    mutexes = new  pthread_mutex_t[UsePThreads];
    runningM = new  pthread_mutex_t[UsePThreads];
    Params = new  parameters[UsePThreads];

    started = false;
    pthread_mutex_init(&Critical,NULL);
    for (int i=0;i<UsePThreads;i++){
        Params[i].id = i;
        Params[i].me = (void*)this;
        Params[i].working = false;
        Params[i].Threads = UsePThreads;
        pthread_mutex_init(&mutexes[i], NULL);
        pthread_mutex_init(&runningM[i], NULL);
        pthread_mutex_unlock(&mutexes[i]);
        pthread_mutex_lock(&runningM[i]);
    }
    Locked = true;
    #else
    std::cout << "Threads are disabled.";
    UsePThreads=1;
    Params = new  parameters[UsePThreads];
    Params[0].id = 0;
    Params[0].me = (void*)this;
    Params[0].Threads = UsePThreads;
    #endif

    CreateThreads();

}
bool ThreadPool::CreateThreads(){
    if (!started){
        started = true;
        #ifndef DISABLE_THREADPOOL
        for (int i=0;i<UsePThreads;i++){
            pthread_create(&thread_pool[i], NULL, &ThreadPool::thread_pool_worker, (void*)&Params[i]);
        }

        Lock();
        #endif
        return true;
    }
    return false;
}
bool ThreadPool::KillThreads(){
    if (started){
        #ifndef DISABLE_THREADPOOL
        Job kill = Job(JOB_KILL);
        for (int i=0;i<UsePThreads;i++){
            AddJob_(kill);
        }
        Unlock();
        for (int e=0;e<UsePThreads;e++){
            std::cout << "Waiting "<<e<<"\n";
            pthread_join(thread_pool[e], NULL);
        }
        #endif
        started = false;
        return true;
    }
    return false;
}

ThreadPool::~ThreadPool(){
    std::cout << "[ThreadPool] Closing threads!\n";
    if (started){
        KillThreads();
    }
    std::cout << "[ThreadPool] Bye!\n";
}

void ThreadPool::AddLambdaJob(std::function<void(int,int,void*)> F){
    Job newJ;
    newJ.lambda = F;
    newJ.Type = JOB_LAMBDA;
    AddJob_(newJ);
}

void ThreadPool::AddJobStack(Job &j){
    #ifndef DISABLE_THREADPOOL
    pthread_mutex_lock(&Critical);
    #endif

    Jobs.push(j);

    #ifndef DISABLE_THREADPOOL
    pthread_mutex_unlock(&Critical);
    #endif
}
void ThreadPool::AddJob_(Job &j,bool ignoreFast){

    AddJobStack(j);

}


void ThreadPool::AddParallelFor(std::function<void(int,int,void*)> F,int min,int max){
    for (int i=0;i<UsePThreads;i++){
       Job newJ(F,min + ((max/UsePThreads)*i),min + ((max/UsePThreads)*(i+1)) );
       AddJob_(newJ);
    }
}



void ThreadPool::ClearJobs(){
    while (not Jobs.empty()){
        Jobs.pop();
    }
}


void ThreadPool::Lock(){
    if (!started){
        std::cout << "[ThreadPool]Lock/Unlock with no threads running.\n";
        return;
    }
    Locked = true;
    bool done = false;


    #ifndef DISABLE_THREADPOOL
    for (int e=0;e<UsePThreads;e++){
        int counter = 0;
        pthread_mutex_lock(&runningM[e]);
    }

    #endif
}
bool ThreadPool::TryLock(){
    if (!started){
        std::cout << "[ThreadPool]Lock/Unlock with no threads running.\n";
        return false;
    }
    #ifndef DISABLE_THREADPOOL
    bool *flags = new bool[UsePThreads];
    bool sucess = true;
    for (int e=0;e<UsePThreads;e++){
        if (pthread_mutex_trylock(&runningM[e])){
            flags[e] = true;
        }else{
            flags[e] = false;
            sucess = false;
        }
    }
    if (!sucess){
        for (int e=0;e<UsePThreads;e++){
            if (flags[e])
                pthread_mutex_unlock(&runningM[e]);
        }
        return false;
    }

    //Locked = true;
    //bool done = false;





    #endif
    return true;
}
void ThreadPool::Unlock(){
    if (!started){
        std::cout << "[ThreadPool]Lock/Unlock with no threads running.\n";
        return;
    }
    Locked = false;
    #ifndef DISABLE_THREADPOOL
    for (int e=0;e<UsePThreads;e++){
        pthread_mutex_unlock(&mutexes[e]);
    }
    #else
    ThreadPool::thread_pool_worker(&Params[0]);
    #endif

}

bool ThreadPool::IsDone(){
    bool finished = false;
    #ifndef DISABLE_THREADPOOL
    pthread_mutex_lock(&Critical);
    #endif
    if (Jobs.empty()){
        finished = true;
    }
    #ifndef DISABLE_THREADPOOL
    pthread_mutex_unlock(&Critical);
    #endif
    if (finished)
        return true;
    return false;

}
void ThreadPool::Update(float dt){}

void *ThreadPool::thread_pool_worker(void *OBJ){
    parameters *P = (parameters*)OBJ;
    ThreadPool *This = (ThreadPool*)P->me;
    Light *luz = Light::GetInstance();
    P->working = false;
    while(true){
        //Call once
        #ifndef DISABLE_THREADPOOL
        pthread_mutex_lock(&This->mutexes[P->id]);
        #endif
        P->working = true;
        Job todo;
        while (P->working){

            todo.Type = JOB_NOTHING;
            #ifndef DISABLE_THREADPOOL
            pthread_mutex_lock(&This->Critical);
            #endif
            if (not This->Jobs.empty()){
                P->working = true;
                todo = This->Jobs.top();
                This->Jobs.pop();
            }else{
                P->working = false;
            }
            #ifndef DISABLE_THREADPOOL
            pthread_mutex_unlock(&This->Critical);
            #endif
            if (todo.Type != JOB_NOTHING){
                if (todo.Type == JOB_LAMBDA){
                    todo.lambda(P->id,P->Threads,NULL);
                }else if (todo.Type == JOB_FOR){
                    for (int i=todo.min;i<todo.max;i++)
                        todo.lambda(P->id,P->Threads,&i);
                }else if (todo.Type == JOB_KILL){
                    #ifndef DISABLE_THREADPOOL
                    pthread_mutex_lock(&This->Critical);
                    std::cout << "[ThreadPool]{Thread:"<< P->id << "} closing.\n";
                    pthread_mutex_unlock(&This->runningM[P->id]);
                    pthread_mutex_unlock(&This->Critical);
                    pthread_exit(NULL);
                    #endif // DISABLE_THREADPOOL
                }else if (todo.Type == JOB_SHADE){
                    luz->Shade(P,todo);
                }else if (todo.Type == JOB_REDUCE){
                    luz->Reduce(P,todo);
                }else if (todo.Type == JOB_GEN){
                    luz->Gen(P,todo);
                }
            }else{
                #ifdef DISABLE_THREADPOOL
                    return NULL;
                #endif
            }
        }
        #ifndef DISABLE_THREADPOOL
        pthread_mutex_unlock(&This->runningM[P->id]);
        #endif
    }
    return NULL;
}

