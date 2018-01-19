#include "luaui.hpp"
#include "luatools.hpp"


#ifndef __EMSCRIPTEN__
LuaUi::LuaUi():UIBase(){
    UI_REGISTER(LuaUi);
    other = 0;
    MainWidget = false;

   OnMouseRelease = [=](UIBase* obj,int i,Point p){
        LuaCaller::CallSelfField(LuaManager::L,obj,"OnMouseRelease",i,p);
   };

   OnMouseEnter = [=](UIBase* obj,Point p){
        LuaCaller::CallSelfField(LuaManager::L,obj,"OnMouseEnter",p);
   };

   OnMouseLeave = [=](UIBase* obj,Point p){
        LuaCaller::CallSelfField(LuaManager::L,obj,"OnMouseLeave",p);
   };

   OnMousePress = [=](UIBase* obj,int i,Point p){
        LuaCaller::CallSelfField(LuaManager::L,obj,"OnMousePress",i,p);
   };

}

void LuaUi::Input(){
    UIBase::Input();
}

void LuaUi::Render(Point where){
    if (GetHidden())
        return;
    LuaCaller::CallSelfField(LuaManager::L,this,"OnRender",where);
    UIBase::Render(where);

}
void LuaUi::Update(float dt){
    if (GetHidden())
        return;
    UIBase::Update(dt);
    LuaCaller::CallSelfField(LuaManager::L,this,"OnUpdate",dt);
}

void LuaUi::ClearChildrens(){
    UIBase::ClearChildrens();
}

void LuaUi::Refresh(){
    UIBase::Refresh();
}

std::vector<LuaUi*> LuaUi::GetChilds(){
    std::vector<LuaUi*> objs;
    for (unsigned i = 0; i < Components.size(); ++i) {
        if (!Components[i]->IsDead()){
            objs.emplace_back((LuaUi*)Components[i].get());
        }
    }
    return objs;
}

void LuaUi::AddComponent2(LuaUi *ui){
    UIBase::AddComponent(ui);
}

void LuaUi::NotifyChildrens(){
    box.x = o_pos.x + (mother ? mother->box.x : 0);
    box.y = o_pos.y + (mother ? mother->box.y : 0);
    for (unsigned i = 0; i < Components.size(); ++i) {
        if (!Components[i]->IsDead()){
            Components[i]->NotifyChildrens();
        }
    }
}

#endif
