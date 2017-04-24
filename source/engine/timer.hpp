#ifndef TIMERH
#define TIMERH
#include <stdint.h>
#include <iostream>
#include "../settings/definitions.hpp"
#include "../framework/geometry.hpp"
#include SDL_LIB_HEADER
/**
    @brief Basic timer

    *
    * Used to count some things by the time
    *
    * About the disable feature.
    *
    * When the update is called, first is checked if the game has finished its timer counting.\n
    * As the counting not ended yet, it will count, and wait until the next iteration.
    *
    * Suddenly, the counting finishes. The chek in update came first than the counting, so it will finish
    * without checking in update. So the  Timer::HasFinished() will return true only for the last of the current
    * iteration. \n
    * On the next update, the timer will be set to disabled, and this HasFinished() will return false.
*/
class Timer{
    public:
        /**
            *Start everything with 0\n
            *Is disabled
        */
        Timer();
        /**
            *Create an timer with a given duration
            *
            *Start enabled
            @param duration the duration in ms
            @param repeat default is 0. The times that it will be going repeat
        */
        Timer(float duration,int repeat=0);
        /**
            * Update the timer When disabled the timer will not continue.
            * When the timer finish its counting, and there is no repeat left
            * the timer will be disabled.
        */
        void Update(float dt);
        /**
            Define an duration
        */
        void SetDuration(float duration);
        /**
            Reset the timer
        */
        void Restart();
        /**
            Get the current timer
        */
        float Get();
        /**
            When disabled the timer will not count
        */
        void Disable(){m_enabled=false;};
        /**
            When disabled the timer will count.
        */
        void Enable(){m_enabled=true;};
        /**
            Check if the timer is enabled
        */
        bool IsEnabled(){return m_enabled;};
        /**
            Return if is finished. If finished and is disabled, it will return false
        */
        bool HasFinished();
    private:
        bool  m_enabled;
        float m_duration;
        float m_time;
        int   m_repeat;
};

class Stopwatch{
    public:
        Stopwatch(){
            Reset();
        };
        void Set(int v){
            begin = SDL_GetTicks()-v;
        }
        void Reset(){
            begin = SDL_GetTicks();
        }
        uint32_t Get(){
            return SDL_GetTicks() - begin;
        };
        void Kill(){};
    private:
        uint32_t begin;
};

#endif
