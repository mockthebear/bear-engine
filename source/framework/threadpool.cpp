#include "threadpool.hpp"
#ifndef DISABLE_THREADPOOL

#include <memory>
#include <stdexcept>

ThreadPool& ThreadPool::GetInstance(){
    static ThreadPool tp;
    return tp;
}

void ThreadPool::Wait(){
    m_wait.store(true);
}

void ThreadPool::Deploy(){
    m_wait.store(false);
}

uint32_t ThreadPool::CountBusy(){
    uint32_t cnt = 0;
    for(size_t i = 0;i<m_threads;++i){
        cnt += is_working[i].load() ? 1 : 0;
    }
    return cnt;
}

bool ThreadPool::IsWorking(uint32_t tid){
    if(m_stop || !m_started){
        return false;
    }
    return is_working[tid].load();
}

void ThreadPool::Start(uint32_t threads){
    Stop();
    m_stop = false;
    m_started = true;
    m_wait = false;
    m_threads = threads;
    is_working = std::vector< std::atomic<bool> >(threads);
    for(size_t i = 0;i<threads;++i){
        is_working[i].store(false);
        workers.emplace_back(
            [this,i]{
                for(;;){
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(this->queue_mutex);
                        this->condition.wait(lock,
                            [this]{ return this->m_stop || !this->tasks.empty(); });
                        if(this->m_stop && this->tasks.empty())
                            return;
                        task = std::move(this->tasks.front());
                        this->tasks.pop();
                    }
                    if (m_wait)
                        continue;
                    is_working[i].store(true);
                    task();
                    is_working[i].store(false);
                }
            }
        );
    }
}


bool ThreadPool::Help(){
    if(m_stop || !m_started){
        return false;
    }
    std::function<void()> task;
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        if (tasks.size() == 0){
            return false;
        }
        task = std::move(tasks.front());
        tasks.pop();
    }
    task();
    return true;
}

void ThreadPool::Stop(){
    if (!m_started)
        return;
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        m_stop = true;
    }
    condition.notify_all();
    for(std::thread &worker: workers)
        worker.join();

    m_started = false;
    workers.clear();
    is_working.clear();
}

ThreadPool::~ThreadPool()
{
    Stop();
}

#endif //DISABLE_THREADPOOL
