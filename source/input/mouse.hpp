#ifndef MOUSEH_BE
#define MOUSEH_BE
#include "../framework/geometry.hpp"
#include "inputdefinitions.hpp"
#include "../settings/definitions.hpp"

class Mouse{
    public:
        Mouse();
        ~Mouse();
        void Update(float dt);
        void Wheel(int x,int y);
        void ButtonDown(int button);
        void ButtonUp(int button);
        void Motion(int x,int y);
        void Enter(int window);
        void Leave(int window);

        int lastKey;
        int lastRKey;
        bool isInside;
        GenericPoint<int> position;
        GenericPoint<int> wheel;
        InputState mouseState[16];

};
#endif // MOUSEH_BE
