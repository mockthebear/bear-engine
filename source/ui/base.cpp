#include "base.hpp"
#include "../input/inputmanager.hpp"
UIBase::UIBase(){
    mother = nullptr;
    Show();
    SetFocused(false);
    close = false;
    alpha = 255;
}

int UIBase::g_ID = 0;


void UIBase::Render(Point where){
    if (GetHidden())
        return;
    for (unsigned i = 0; i < Components.size(); ++i) {
        if (!Components[i]->IsDead()){
            Components[i]->Render(Point(where.x + box.x,where.y + box.y));
        }
    }
    if (sp.IsLoaded()){
        sp.Render(where.x +box.x,where.y +box.y);
    }
    if (txt.IsWorking()){
        txt.Render(where.x +box.x,where.y +box.y);
    }
}

void UIBase::Update(float dt){

    if (GetHidden())
        return;
    for (unsigned i = 0; i < Components.size(); ++i) {
        if (!Components[i]->IsDead()){
            Components[i]->Update(dt);
        }
    }
    Input();
}

void UIBase::NotifyChildrens(){
    box.x = o_pos.x + (mother ? mother->box.x : 0);
    box.y = o_pos.y + (mother ? mother->box.y : 0);
    for (unsigned i = 0; i < Components.size(); ++i) {
        if (!Components[i]->IsDead()){
            Components[i]->NotifyChildrens();
        }
    }
}
void UIBase::Input(){

    int key = InputManager::GetInstance().GetMouseMousePressKey();

    Point mpos = Point(InputManager::GetInstance().GetMouseX(),InputManager::GetInstance().GetMouseY());
    if (key != 0 && OnMousePress && IsInside(mpos.x,mpos.y)){
        OnMousePress(this,key,mpos);
    }
    key = InputManager::GetInstance().GetMouseMouseReleaseKey();
    if (key != 0 && OnMouseRelease && IsInside(mpos.x,mpos.y)){
        OnMouseRelease(this,key,mpos);
    }

    key = InputManager::GetInstance().IsAnyKeyPressed();
    if (key != 0 && OnKeyPress){
        OnKeyPress(this,key);
    }
}
void UIBase::Refresh(){
    if (sp.IsLoaded()){
        box.w = std::max(sp.GetWidth(),(int)box.w);
        box.h = std::max(sp.GetHeight(),(int)box.h);
    }
    if (txt.IsWorking()){
        box.w = std::max(txt.GetWidth(),box.w);
        box.h = std::max(txt.GetHeight(),box.h);
    }
}


void UIBase::AddComponent(UIBase *ui){
    Components.emplace_back(ui);
}
