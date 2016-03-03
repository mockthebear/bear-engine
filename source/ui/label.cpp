#include "label.hpp"
#include "../engine/renderhelp.hpp"
#include "../input/inputmanager.hpp"

Label::Label(Point pos,std::string str,UIBase *owner):UIBase(){
    UI_REGISTER(Label);
    o_pos = pos;
    mother = owner;

    box.x = pos.x + (owner ? owner->box.x : 0);
    box.y = pos.y + (owner ? owner->box.y : 0);
    SetText(Text(style.fontfile,style.fontSize,style.txtstyle,str, {style.fg[0],style.fg[1],style.fg[2]} ));
    box.w += 3;
    box.h += 3;

}

Label::Label(Point pos,Text t,UIBase *owner):UIBase(){
    UI_REGISTER(Label);
    o_pos = pos;
    mother = owner;
    box.x = pos.x + (owner ? owner->box.x : 0);
    box.y = pos.y + (owner ? owner->box.y : 0);
    SetText(t);
    box.w += 3;
    box.h += 3;
}

Label::Label(Point pos,Sprite t,UIBase *owner):UIBase(){
    UI_REGISTER(Label);
    o_pos = pos;
    mother = owner;
    box.x = pos.x + (owner ? owner->box.x : 0);
    box.y = pos.y + (owner ? owner->box.y : 0);
    SetSprite(t);
    box.w += 3;
    box.h += 3;
}

void Label::Render(Point where){
    UIBase::Render(Point(2,0));
}
