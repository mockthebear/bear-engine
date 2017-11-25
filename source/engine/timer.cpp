#include "timer.hpp"

Timer::Timer(){
    m_duration = 0;
    m_time = 0;
    m_repeat = 0;
    m_enabled = false;
}
Timer::Timer(float duration,int repeat){
    m_duration = duration;
    m_time = 0;
    m_repeat = repeat;
    m_enabled = true;
}
void Timer::Update(float dt){
    if (!m_enabled)
        return;
    if ( (m_repeat > 0 || m_repeat == -1) && HasFinished()){
        //Restart();
    }
    m_time -= dt;
};

void Timer::SetDuration(float duration){
    m_duration = duration;
}

void Timer::Restart(){
    m_time = m_duration;
    m_enabled = true;
    if (m_repeat > 0)
        m_repeat--;
};

float Timer::Get(){
    return m_time;
}
bool Timer::HasFinished(){
    if (!m_enabled)
        return false;
    return m_time <= 0;
}
