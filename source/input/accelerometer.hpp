#ifndef ACCELEROMETERH
#define ACCELEROMETERH

#include "joystick.hpp"

class Accelerometer{
    public:
        Accelerometer();
        void Update(float dt);
        void Render(PointInt pos=PointInt(100,100));
        void Set(Joystick *j);
        void SetFilterFactor(float factor);
        float GetXPercent();
        float GetYPercent();
        float GetZPercent();
        Point3 GetPercent();

        uint16_t GetX();
        uint16_t GetY();
        uint16_t GetZ();

    private:
        Joystick *m_joy;
        Point3 value;
        float m_factor;
};
#endif // ACCELEROMETERH
