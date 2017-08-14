#include "window.hpp"
#include "../engine/renderhelp.hpp"
#include "../input/inputmanager.hpp"

Window::Window():UIBase(){
    UI_REGISTER(Window);
}
Window::Window(Rect dimensions,UIBase *owner):UIBase(){
    UI_REGISTER(Window);
    box = dimensions;
    mother = owner;
    o_pos.x = box.x;
    o_pos.y = box.y;
    box.x = dimensions.x + (owner ? owner->box.x : 0);
    box.y = dimensions.y + (owner ? owner->box.y : 0);


    OnMousePress = [=](UIBase*w,int button,Point pos){
        SetFocused(true);
    };
    highlight = true;
    OnUpdate =[=](UIBase*,float dt){

    };
}
Window::~Window(){

}
void Window::Update(float dt){
    UIBase::Update(dt);
    OnUpdate(this,dt);
}
void Window::Input(){
    UIBase::Input();
    Point pos = Point(InputManager::GetInstance().GetMouseX(),InputManager::GetInstance().GetMouseY());
    if (IsInside(pos.x,pos.y) && InputManager::GetInstance().MousePress(1) ){
        SetFocused(true);
    }
    if (!IsInside(pos.x,pos.y) &&  (InputManager::GetInstance().MouseRelease(1) || InputManager::GetInstance().MousePress(1)) ){
        SetFocused(false);
    }
}



void Window::Render(Point where){
    RenderHelp::DrawSquareColorA(box.x,box.y,box.w,box.h,style.bg[0] ,style.bg[1] ,style.bg[2] ,style.bg[3] );

    RenderHelp::DrawSquareColorA(box.x,box.y,box.w,box.h,style.fg[0] ,style.fg[1] ,style.fg[2] ,style.fg[3],true );

    where.x = box.x;
    where.y = box.y;
    UIBase::Render(where);
    //Point pos = Point(InputManager::GetInstance().GetMouseX(),InputManager::GetInstance().GetMouseY());
    //RenderHelp::DrawSquareColorA(pos.x,pos.y,3,3,255,0,0,100);
}
