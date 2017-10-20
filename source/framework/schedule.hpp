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

/** \brief Class to postpone some events
 *  This runs between the Update and Render events
 */


class Scheduler{
    public:

        Scheduler();
        ~Scheduler();



        static Scheduler& GetInstance();
        /** \brief Stop all events
         *
         * \return Number of events that are not running anymore
         *
         */

        uint32_t StopAll();
        /** \brief Stop an specific event
         *
         * \param Event id
         * \return sucess
         *
         */

        bool Stop(uint64_t eventId);
        /** \brief Add an event
         *
         * \param Duration until the execution starts
         * \param An lambda function of typed void();
         * \param Number of repetitions of this same event
         * \return event Id
         *
         */

        uint64_t AddEvent(float duration,ScheduleFunction F,uint32_t repeats=0);
    private:
        friend class Game;
        void Update(float dt);
        uint64_t m_eventId;
        std::map<uint64_t,EventRef> m_events;
};

extern Scheduler g_scheduler;
