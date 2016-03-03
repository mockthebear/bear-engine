#ifndef TOUCHKEYSBEH
#define TOUCHKEYSBEH
#include "../framework/geometry.hpp"
#include "../engine/text.hpp"
#include "keyboard.hpp"
#include "touch.hpp"
#include "mouse.hpp"
#include <string>
class TouchKeys{
    public:
        TouchKeys();
        TouchKeys(int Key,PointInt POSITION,PointInt size,std::string text);
        void Update(float dt,Keyboard &handler,Touch &thandler,Mouse &mhandler);
        void Render();
    private:
        Text *t;
        Rect box,collisionRect;
        int Key;
        bool Touched;
};
#endif // TOUCHKEYSBEH
