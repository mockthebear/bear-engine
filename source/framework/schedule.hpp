#pragma once
#include <functional>
#include <map>

class Scheduler;
typedef std::function<void()> ScheduleFunction;

enum ExecTimeDef{
    EXEC_UPDATE=0,
    EXEC_RENDER,
};

class EventRef{
    public:
        EventRef():duration(0.0f),counter(0.0f),repeat(0),execTime(EXEC_UPDATE){

        };
        EventRef(float dur,ScheduleFunction& F,uint32_t rep = 0,ExecTimeDef exTime = EXEC_UPDATE):duration(dur),counter(dur),repeat(rep),execTime(exTime){
            func = F;
        };
        void SetExecTime(ExecTimeDef execPar){ execTime = execPar; };
    private:
        friend class Scheduler;
        float duration;
        float counter;
        long int repeat;
        ExecTimeDef execTime;

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

        void Stop(int64_t eventId);
        /** \brief Add an event
         *
         * \param Duration until the execution starts
         * \param An lambda function of typed void();
         * \param Number of repetitions of this same event
         * \return event Id
         *
         */

        uint64_t AddEvent(float duration,ScheduleFunction F,uint32_t repeats=0);

        EventRef &GetEvent(uint64_t eventId){return m_events[eventId];};
        /** \brief Return a reference for a event
         *
         * \param Event id
         * \return event
         *
         */
    private:
        friend class Game;
        void Update(float dt);
        void Render();
        uint64_t m_eventId;
        std::map<uint64_t,EventRef> m_events;
};

extern Scheduler g_scheduler;
