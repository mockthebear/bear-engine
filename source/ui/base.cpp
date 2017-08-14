#include "base.hpp"
#include "../input/inputmanager.hpp"
#include "../framework/utils.hpp"


UIStyle UIStyle::BaseStyle = UIStyle();

UIBase::UIBase(){
    mother = nullptr;
    Show();
    SetFocused(false);
    close = false;
    alpha = 255;
    Color[0] = 100;
    Color[1] = 100;
    Color[2] = 110;
    Color[3] = 255;
    style = UIStyle::BaseStyle;
    MouseInside = false;
    GenerateId();
    UiName = utils::format("uiobj%.4d",ID);
}

int UIBase::g_ID = 0;


void UIBase::Render(Point where){
    if (GetHidden())
        return;
    for (unsigned i = 0; i < Components.size(); ++i) {
        if (!Components[i]->IsDead()){
            Components[i]->Render(Point(box.x,box.y));
        }
    }
    if (sp.IsLoaded()){
        sp.Render(box.x,box.y,0);
    }
    if (txt.IsWorking()){
        txt.Render(box.x,box.y);
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
void UIBase::ClearChildrens(){
    Components.clear();
}
void UIBase::Input(){

    int key = InputManager::GetInstance().GetMouseMousePressKey();

    Point mpos = InputManager::GetInstance().GetMouse();
    if (IsInside(mpos.x,mpos.y)){

        if (key != 0 && OnMousePress){
            OnMousePress(this,key,mpos);
        }
        key = InputManager::GetInstance().GetMouseMouseReleaseKey();
        if (key != 0 && OnMouseRelease){
            OnMouseRelease(this,key,mpos);
        }
        if (!MouseInside){
            if (OnMouseEnter){
                OnMouseEnter(this,mpos);
            }
            MouseInside = true;
        }
    }else{
        if (MouseInside){
            if (OnMouseLeave){
                OnMouseLeave(this,mpos);
            }
            MouseInside = false;
        }
    }

    key = InputManager::GetInstance().IsAnyKeyPressed();
    if (key != 0 && OnKeyPress){
        OnKeyPress(this,key);
    }

    /*for (unsigned i = 0; i < Components.size(); ++i) {
        if (!Components[i]->IsDead()){
            Components[i]->Input();
        }
    }*/
}

UIBase *UIBase::GetChildById(std::string name){
    for (unsigned i = 0; i < Components.size(); ++i) {
        if (!Components[i]->IsDead()){
            if (Components[i]->GetId() == name){
                return Components[i].get();
            }
        }
    }
    return nullptr;
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
    box.x = o_pos.x + (mother ? mother->box.x : 0);
    box.y = o_pos.y + (mother ? mother->box.y : 0);
}


void UIBase::AddComponent(UIBase *ui){
    ui->mother = this;
    ui->NotifyChildrens();
    Components.emplace_back(ui);
}
