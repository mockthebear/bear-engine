#pragma once
#include <functional>
#include <map>

class Scheduler;
typedef std::function<void()> ScheduleFunction;

class EventRef{
    public:
        EventRef():duration(0.0f),counter(0.0f),repeat(0){

        };
        EventRef(float dur,ScheduleFunction& F,uint32_t rep = 0):duration(dur),counter(dur),repeat(rep){
            func = F;
        };
    private:
        friend class Scheduler;
        float duration;
        float counter;
        long int repeat;

        ScheduleFunction func;

};

class Scheduler{
    public:

        Scheduler();
        ~Scheduler();

        void Update(float dt);

        static Scheduler& GetInstance();

        uint32_t StopAll();
        uint64_t AddEvent(float duration,ScheduleFunction F,uint32_t repeats=0);
    private:
        uint64_t m_eventId;
        std::map<uint64_t,EventRef> m_events;
};

extern Scheduler g_scheduler;
