
#include "touchkeys.hpp"
#include "inputmanager.hpp"
#include "../engine/renderhelp.hpp"
#include <iostream>
TouchKeys::TouchKeys(){
    Key = -1;
}

TouchKeys::TouchKeys(int Key_,PointInt POSITION,PointInt size,std::string str){
    box.x = POSITION.x;
    box.y = POSITION.y;
    box.w = size.x;
    box.h = size.y;
    collisionRect = box;

    Key = Key_;
    Touched = false;
    t = new Text(str,12,{100,200,100});
}
void TouchKeys::Update(float dt,Keyboard &handler,Touch &thandler,Mouse &mhandler){
    if (Key == -1)
        return;
    InputState state;
    Point f;
    for (int i=0;i<MAX_TOUCHSCREEN_FINGERS+1;i++){
        if (i == MAX_TOUCHSCREEN_FINGERS){
            f.x = mhandler.position.x;
            f.y = mhandler.position.y;
            state = mhandler.mouseState[1];
        }else{
            state = thandler.fingers[i].m_state;
            f = thandler.fingers[i].m_truePosition;
            f.x += thandler.m_offset.x;
            f.y += thandler.m_offset.y;
        }
        if ( (state == PRESSED or state == JUST_PRESSED) and box.IsInside(f.x,f.y)){
            if (!Touched)
                handler.KeyPress(Key);
            Touched = true;
        }else{
            if (Touched)
                handler.KeyRelease(Key);
            Touched = false;

        }

    }
}
void TouchKeys::Render(){
    if (Key == -1)
        return;
    RenderHelp::DrawSquareColorANS(box.x,box.y,box.w,box.h,0,0,255,Touched ? 100 : 255);
    t->RenderRS(box.x +2,box.y +2);
}
