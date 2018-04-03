#include "schedule.hpp"
#include "../engine/bear.hpp"
#include <iostream>

Scheduler g_scheduler;

Scheduler& Scheduler::GetInstance(){
    return g_scheduler;
}

Scheduler::Scheduler(){
    m_eventId = 0;
    g_scheduler = Scheduler::GetInstance();
}

Scheduler::~Scheduler(){
    StopAll();
}
void Scheduler::Update(float dt){
    for (auto &it : m_events){
        it.second.counter -= dt;
        if (it.second.counter <= 0){
            if (it.second.func){
                it.second.counter = it.second.duration;
                if (it.second.repeat >= 0 ){
                    ScheduleFunction &f = it.second.func;
                    f();
                    it.second.repeat--;
                }
            }
        }
    }
    for (auto &it : m_events){
        if (it.second.repeat < 0 ){
            m_events.erase(it.first);
            break;
        }
    }
}

uint32_t Scheduler::StopAll(){
    uint32_t count = 0;
    for (auto &it : m_events){
        if (it.second.counter > 0){
            count++;
        }
    }
    m_events.clear();
    return count;
}


uint64_t Scheduler::AddEvent(float dur,ScheduleFunction F,uint32_t rep){
    m_eventId++;
    m_events[m_eventId] = EventRef(dur,F,rep);
    return m_eventId;
}


