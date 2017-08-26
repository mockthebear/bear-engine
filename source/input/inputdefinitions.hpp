#ifndef INPUTDEF_BE
#define INPUTDEF_BE

#define MAX_TOUCHSCREEN_FINGERS 20
#include <map>
/**
 * @brief Enum to pressed status
 */
enum InputState{
    UNKNOW_STATE=-1,
    RELEASED=0,
    JUST_RELEASED,
    PRESSED,
    JUST_PRESSED
};
/**
 * @brief Information returned to manage Touch values from InputManager
 * Everything is public.
 *
 */
class TouchInfo{
    public:
        TouchInfo():state(RELEASED),pressure(0.0f),finger(-1),x(0),y(0),x_f(0.0f),y_f(0.0f){};
        /**
            *Current InputManager from the touch
        */
        InputState state;
        /**
            *[0..1] Pressure from the touch
        */
        float pressure;
        /**
            *Finger id
        */
        int finger;
        /**
            *Position x of the touch
        */
        int x;
        /**
            *Position y of the touch
        */
        int y;
        /**
            *[0-1] value from where the finger is
            *
            *Raw
        */
        float x_f;
        /**
            *[0-1] value from where the finger is
            *
            *Raw
        */
        float y_f;
};

#endif // INPUTDEF_BE
