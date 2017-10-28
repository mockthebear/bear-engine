#include "numberinput.hpp"

#include "../engine/renderhelp.hpp"
#include "../input/inputmanager.hpp"

NumberInput::NumberInput(Point pos,int *start,UIBase *owner):UIBase(){
    UI_REGISTER(NumberInput);
    o_pos = pos;
    mother = owner;
    box.x = pos.x + (owner ? owner->box.x : 0);
    box.y = pos.y + (owner ? owner->box.y : 0);

    char input[128];
    addr = start;
    sprintf_s(input,"%d",*addr);
    SetText(Text(style.fontfile,style.fontSize,style.txtstyle,input,{style.fg[0],style.fg[1],style.fg[2]} ));
    box.w = 100;
    box.h = style.fontSize + 4;
    showCaret = true;
    caret = 4.0f;
}

NumberInput::NumberInput(Point pos,int start,UIBase *owner):UIBase(){
    UI_REGISTER(NumberInput);
    addr = nullptr;
    mother = owner;
    box.x = pos.x + (owner ? owner->box.x : 0);
    box.y = pos.y + (owner ? owner->box.y : 0);
    char input[128];
    sprintf_s(input,"%d",start);
    SetText(Text(style.fontfile,style.fontSize,style.txtstyle,input, {style.fg[0],style.fg[1],style.fg[2]} ));
    box.w = 100;
    box.h = style.fontSize + 4;
    showCaret = true;
    caret = 4.0f;
}


void NumberInput::Input(){
    UIBase::Input();
    Point pos = Point(InputManager::GetInstance().GetMouseX(),InputManager::GetInstance().GetMouseY());
    if (IsInside(pos.x,pos.y)){
        if (InputManager::GetInstance().MousePress(1)){
            SetFocused(true);
        }
    }else{
        if (InputManager::GetInstance().MousePress(1)){
            SetFocused(false);
        }
    }
    if (focused){
        int key = InputManager::GetInstance().IsAnyKeyPressed();

        if (key != -1){
            std::stringstream S;

            if (key == SDLK_BACKSPACE){
                if (txt.GetText().size() > 1){
                    txt.SetText(txt.GetText().substr(0,txt.GetText().size()-1));
                }else{
                    txt.SetText("0");
                }
            }else if(key >= 42 && key <= 59){
                if (txt.GetText() != "0"){
                    S << txt.GetText() << (char)key;
                }else{
                    S << (char)key;
                }
                txt.SetText(S.str());

            }
            if (addr != nullptr){
                (*addr) = GetNumber();
            }
        }
    }


}
void NumberInput::Update(float dt){
    caret -= dt;
    if (caret <= 0){
        caret = 4.0f;
        showCaret = !showCaret;

    }
    UIBase::Update(dt);
}
void NumberInput::Render(Point where){

    RenderHelp::DrawSquareColorA(box.x,box.y,box.w,box.h,0,0,0,255);
    RenderHelp::DrawSquareColorA(box.x+1,box.y+1,box.w-2,box.h-2,
                                 100 + (focused ? 30 : 0),
                                 100 + (focused ? 30 : 0),
                                 100 + (focused ? 30 : 0),
                                 255);
    if (txt.IsWorking()){
        txt.Render(box.x + 3,box.y);

    }
    if (showCaret && focused)
        RenderHelp::DrawSquareColorA(box.x + txt.GetWidth() +3,box.y+2,1,box.h-4,style.fg[0],style.fg[1],style.fg[2],255);

}
