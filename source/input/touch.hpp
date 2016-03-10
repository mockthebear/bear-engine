#ifndef TOUCHH_BE
#define TOUCHH_BE
#include "inputdefinitions.hpp"
#include "../framework/geometry.hpp"
/**
 * @brief Private class from Touch to hold shinenigans about Finger
 * This is used to hold finger data
 */
class Finger{
    public:
        Finger();
        void Update(float dt);
        void Render();
        PointInt m_position;
        GenericPoint<float> m_truePosition;
        InputState m_state;
        InputState m_tapState;
        float m_ressure;
        bool tapping;
        float tapDelay;
};
/**
 * @brief Private class from InputManager to control Touch things
 * This is used to hold finger data
 */
class Touch{
    public:
        Touch();
        ~Touch();
        void Update(float dt);
        void Render();
        void UpdateTouchPosition(int id,float x,float y,float pressure);
        void UpdateTouchStatus(int id,bool isDown);
        PointInt GetFingerPosition(int id=-1);
        TouchInfo GetFinger(int id=-1);
        Finger fingers[MAX_TOUCHSCREEN_FINGERS];
        int m_touchedCount;

        void SetOffset(Point offset){m_offset = offset;};
        Point m_offset;
        int m_hasTap;
    private:

        int m_lastFinger;
};
#endif // TOUCHH_BE
