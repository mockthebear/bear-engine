#include "checkbox.hpp"
#include "../engine/renderhelp.hpp"
#include "../engine/assetmanager.hpp"
#include "../input/inputmanager.hpp"

Checkbox::Checkbox(Point pos,bool *check,std::string str,UIBase *owner):UIBase(){
    UI_REGISTER(Checkbox);
    o_pos = pos;
    mother = owner;
    box.x = pos.x + (owner ? owner->box.x : 0);
    box.y = pos.y + (owner ? owner->box.y : 0);
    SetSprite(UIAssetManager::GetInstance().make<Sprite>("ui:checkbox.png"));
    checked = check;
    SetCheck( (*check) );

    box.h = 16;
    txt = Text(style.fontfile,style.fontSize,style.txtstyle,str, {style.fg[0],style.fg[1],style.fg[2]} );
    Refresh();

}
void Checkbox::Refresh(){
    box.w = txt.GetWidth() + sp.GetWidth() + 3;
    box.h += 3;
    SetCheck((*checked));
}
void Checkbox::Render(Point where){
    if (sp.IsLoaded()){
        sp.Render(PointInt(box.x,box.y));
    }
    if (txt.IsWorking()){
        txt.Render(box.x + 3 + sp.GetWidth(),box.y);
    }
}
void Checkbox::SetCheck(bool c){
    *checked = c;
    if (*checked){
        sp.SetClip(0,0,16,16);
    }else{
        sp.SetClip(0,16,16,16);
    }
}
void Checkbox::Input(){
    Point pos = Point(InputManager::GetInstance().GetMouseX(),InputManager::GetInstance().GetMouseY());
    if (IsInside(pos.x,pos.y)){
        if (InputManager::GetInstance().MousePress(1)){
             (*checked) = !(*checked);
        }
    }
    SetCheck((*checked));
    UIBase::Input();
}

