#include "testthread.hpp"


int  Test_Threadpool::iterations = 0;
unsigned int  Test_Threadpool::inside = 0;

void job(int from,int to,void*){

    int inside = 0;
    int total = to-from;

    unsigned int s = from;




    float Pointx,Pointy,sq;
    for (int in = from;in < to;in++){
        Pointx = (rand_r(&s)%10000000)/5000000.0f  -1;
        Pointy = (rand_r(&s)%10000000)/5000000.0f  -1;
        sq = Pointx*Pointx + Pointy*Pointy;
        if (sq <= 1)
            inside++;
    }
    ThreadPool::GetInstance().CriticalLock();
    std::stringstream S;
    Test_Threadpool::inside += inside;
    S << "From ["<<from<<":"<<to<<"] i got " << ((inside/(float)total)*4.0f) << "\n";
    bear::out << S.str();
    ThreadPool::GetInstance().CriticalUnLock();
};
