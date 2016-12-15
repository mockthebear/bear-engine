#include "textinput.hpp"
#include "../input/inputmanager.hpp"
#include "../engine/renderhelp.hpp"
#include <sstream>
#include <locale>

TextInput::TextInput(Point pos,std::string str,UIBase *owner):UIBase(){
    UI_REGISTER(TextInput);
    o_pos = pos;
    box.x = pos.x + (owner ? owner->box.x : 0);
    box.y = pos.y + (owner ? owner->box.y : 0);
    mother = owner;
    SetText(Text(style.fontfile,style.fontSize,style.txtstyle,str, {style.fg[0],style.fg[1],style.fg[2]} ));
    box.w = 100;
    box.h = style.fontSize + 4;
    Upper = false;
    showCaret = true;
    caret = 4.0f;
}


void TextInput::Input(){
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

        if (key != 0){
            std::stringstream S;

            if (key == SDLK_BACKSPACE){
                if (txt.GetText().size() > 1){
                    txt.SetText(txt.GetText().substr(0,txt.GetText().size()-1));
                }else{
                    txt.SetText(" ");
                    txt.SetText("");
                }
            }else if(key >= 97 && key <= 122){

                if (Upper || InputManager::GetInstance().IsKeyDown(SDLK_LSHIFT) || InputManager::GetInstance().IsKeyDown(SDLK_RSHIFT)){
                    key -= 32;
                    S << txt.GetText() << (char)key;
                }else{
                    S << txt.GetText() << (char)key;
                }
                txt.SetText(S.str());
            }else if( (key >= 42 && key <= 59) || key == 32){
                 S << txt.GetText() << (char)key;
                txt.SetText(S.str());
            }
        }
    }


}
void TextInput::Update(float dt){
    caret -= dt;
    if (caret <= 0){
        caret = 4.0f;
        showCaret = !showCaret;

    }
    UIBase::Update(dt);
}
void TextInput::Render(Point where){

    RenderHelp::DrawSquareColorA(box.x,box.y,box.w,box.h,0,0,0,255);
    RenderHelp::DrawSquareColorA(box.x+1,box.y+1,box.w-2,box.h-2,
                                 100 + (focused ? 30 : 0),
                                 100 + (focused ? 30 : 0),
                                 100 + (focused ? 30 : 0),
                                 255);

    UIBase::Render(Point(2,0));
    if (showCaret && focused)
        RenderHelp::DrawSquareColorA(box.x + txt.GetWidth() +3,box.y+2,1,box.h-4,style.fg[0],style.fg[1],style.fg[2],255);
}
