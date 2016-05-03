#include "button.hpp"
#include "../engine/renderhelp.hpp"
#include "../input/inputmanager.hpp"

Button::Button(Point pos,std::string str,UIBase *owner):UIBase(){
    UI_REGISTER(Button);
    o_pos = pos;
    mother = owner;
    box.x = pos.x + (owner ? owner->box.x : 0);
    box.y = pos.y + (owner ? owner->box.y : 0);
    SDL_Color color = {style.fg[0],style.fg[1],style.fg[2]};
    SetText(Text(style.fontfile,style.fontSize,style.txtstyle,str,color ));
    box.w += 8;
    box.h += 2;
}

void Button::Input(){
    UIBase::Input();
    Point pos = Point(InputManager::GetInstance().GetMouseX(),InputManager::GetInstance().GetMouseY());
    if (IsInside(pos.x,pos.y)){
        hover = true;
        if (InputManager::GetInstance().IsMousePressed(1)){
            press = true;
        }
        if (InputManager::GetInstance().IsMouseReleased(1)){
            press = false;
        }
    }else{
        hover = false;
        press = false;
    }

};

void Button::Render(Point where){
    RenderHelp::DrawSquareColorA(box.x,box.y,box.w,box.h,0,0,0,255);
    RenderHelp::DrawSquareColorA(box.x+1,box.y+1,box.w-2,box.h-2,
                                 Color[0] + (hover ? 20 : 0) + (press ? -30 : 0),
                                 Color[1] + (hover ? 20 : 0) + (press ? -30 : 0),
                                 Color[2] + (hover ? 20 : 0) + (press ? -30 : 0),
                                 Color[3]);
    UIBase::Render(Point(2,0));
}
