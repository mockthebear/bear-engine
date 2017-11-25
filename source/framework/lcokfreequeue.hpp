#pragma once
#include <iostream>
#include <atomic>
 #include <thread>
#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <tuple>
#include <sstream>
#include <string>

/*
    MEM:
    //1
    [flag busy:0][flag okay:0][data]
    [flag busy:0][flag okay:0][data]
    //2
    [flag busy][flag okay][data]
    [flag busy][flag okay][data]
    //3
    [flag busy][flag okay][data]
    [flag busy][flag okay][data]

*/

#define FLAG_ON_USE 0
#define FLAG_READY 1
#define FLAG_OK_READ 2

template <typename T>class LockFreeQueue{
    public:
        LockFreeQueue(uint32_t threads,uint32_t sizerPerThread){
            this->threads = threads;
            this->sizerPerThread = sizerPerThread;
            scalonator = 0;
            jCounter = 0;
            queue = new T*[threads];
            flags = new std::atomic_flag**[threads];
            for (uint32_t i=0;i<threads;i++){
                queue[i] = new T[sizerPerThread];
                queue[i] = new T[sizerPerThread];
                flags[i] = new std::atomic_flag*[sizerPerThread];
                for (uint32_t e=0;e<sizerPerThread;e++){
                    flags[i][e] = new std::atomic_flag[3];
                    flags[i][e][FLAG_ON_USE].clear(std::memory_order_release);
                    flags[i][e][FLAG_READY].clear(std::memory_order_release);
                    flags[i][e][FLAG_OK_READ].clear(std::memory_order_release);
                }
            }
        }

        void clear(){

        }
        void erase(){
            for (uint32_t i=0;i<threads;i++){
                delete [] queue[i];
            }
            delete queue;
        };
        bool Push(T t,int notThis=0){
            //uint32_t rthd = notThis == 0 ? rand()%threads : notThis%threads;
            uint32_t rthd = notThis == 0 ? (scalonator++)%threads : notThis%threads;

            for (uint32_t ind=0;ind < sizerPerThread;ind++){
                if (!flags[rthd][ind][FLAG_ON_USE].test_and_set(std::memory_order_acquire)){
                    bool isReady = flags[rthd][ind][FLAG_READY].test_and_set(std::memory_order_acquire);
                    /*
                        False means ok
                        True means busy
                    */
                    if (!isReady){

                        queue[rthd][ind] = t;
                        flags[rthd][ind][FLAG_OK_READ].test_and_set(std::memory_order_acquire);
                        flags[rthd][ind][FLAG_ON_USE].clear(std::memory_order_release);
                        jCounter++;
                        return true;
                    }else{
                        //flags[rthd][ind][FLAG_READY].clear(std::memory_order_release);
                        flags[rthd][ind][FLAG_ON_USE].clear(std::memory_order_release);
                    }
                }
            }

            getchar();
            //JobCount();
            return Push(t,rthd+1);

        }

        T Get(int thisThread){
            T disReturn;
            if (thisThread >= threads){
                disReturn.empty = true;
                return disReturn;
            }
            for (uint32_t ind=0;ind < sizerPerThread;ind++){
                if (!flags[thisThread][ind][FLAG_ON_USE].test_and_set(std::memory_order_acquire)){
                    bool flagLevel = flags[thisThread][ind][FLAG_READY].test_and_set(std::memory_order_acquire);
                    if (flagLevel){
                        if (flags[thisThread][ind][FLAG_OK_READ].test_and_set(std::memory_order_acquire)){
                            //Now i got the data :D
                            disReturn = queue[thisThread][ind];
                            flags[thisThread][ind][FLAG_OK_READ].clear(std::memory_order_release);
                            flags[thisThread][ind][FLAG_READY].clear(std::memory_order_release);
                            flags[thisThread][ind][FLAG_ON_USE].clear(std::memory_order_release);
                            return disReturn;
                        }
                    }else{
                        flags[thisThread][ind][FLAG_READY].clear(std::memory_order_release);
                    }
                    flags[thisThread][ind][FLAG_ON_USE].clear(std::memory_order_release);
                }
            }

            thisThread++;

            return Get(thisThread);
        }
        bool empty(){
            return JobCount() == 0;
        }
        int JobCount(){
            int counter = 0;
            for (uint32_t thisThread=0;thisThread<threads;thisThread++){
                for (uint32_t ind=0;ind < sizerPerThread;ind++){
                    if (flags[thisThread][ind][FLAG_READY].test_and_set(std::memory_order_acquire)){
                        if (!flags[thisThread][ind][FLAG_OK_READ].test_and_set(std::memory_order_acquire)){
                            flags[thisThread][ind][FLAG_OK_READ].clear(std::memory_order_release);
                        }else{
                            counter++;
                        }
                    }else{
                        flags[thisThread][ind][FLAG_READY].clear(std::memory_order_release);
                    }
                }
            }


            return counter;
        }
    private:
        int jCounter;
        uint32_t scalonator;
        uint32_t threads;
        uint32_t sizerPerThread;
        std::atomic_flag ***flags;
        T** queue;
};
