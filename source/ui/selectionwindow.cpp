#include "selectionwindow.hpp"
#include "label.hpp"
#include "../engine/renderhelp.hpp"
#include "../input/inputmanager.hpp"

SelectionWindow::SelectionWindow():UIBase(){
    UI_REGISTER(SelectionWindow);
}
SelectionWindow::SelectionWindow(Rect dimensions):UIBase(){
    UI_REGISTER(SelectionWindow);
    box = dimensions;
    optionCounter = 0;
    mother = nullptr;
    box.x = dimensions.x;
    box.y = dimensions.y;
    OnMousePress = [=](UIBase*w,int button,Point pos){
        SetFocused(true);
    };
    selectedOption = -1;
    arrow = Text(style.fontfile,style.fontSize,style.txtstyle,">", {style.fg[0],style.fg[1],style.fg[2]} );
    Back = [=](UIBase *b){
    };
    title = new Label(Point(dimensions.w/2 -16,2),std::string(""),this);
    title->style.fg[0] = 255;
    title->style.txtstyle = TEXT_BLENDED;
    AddComponent(title);


}
SelectionWindow::~SelectionWindow(){

}
void SelectionWindow::SetTitle(std::string name){
    if (title){
        title->SetText(name);
    }
}
void SelectionWindow::Update(float dt){
    if (skip){
        skip = false;
        return;
    }
    if (title)
        title->box.x = box.x + box.w/2 - title->GetTextObject()->GetWidth()/2;

    UIBase::Update(dt);
}
void SelectionWindow::Input(){
    if (skip){
        return;
    }
    UIBase::Input();
    Point pos = Point(InputManager::GetInstance().GetMouseX(),InputManager::GetInstance().GetMouseY());
    if (IsInside(pos.x,pos.y) && InputManager::GetInstance().MousePress(1) ){
        SetFocused(true);
    }
    if (!IsInside(pos.x,pos.y) &&  (InputManager::GetInstance().MouseRelease(1) || InputManager::GetInstance().MousePress(1)) ){
        SetFocused(false);
    }
    if (selectedOption != -1){
        if (InputManager::GetInstance().KeyPress(SDLK_DOWN)){
            selectedOption++;
            if (selectedOption >= optionCounter){
                selectedOption = 0;
            }
        }
        if (InputManager::GetInstance().KeyPress(SDLK_UP)){
            selectedOption--;
            if (selectedOption < 0){
                selectedOption = optionCounter-1;
            }
        }
        if (InputManager::GetInstance().KeyPress(SDLK_SPACE) || InputManager::GetInstance().KeyPress(SDLK_RETURN) || InputManager::GetInstance().KeyPress(SDLK_z) ){
            functions[option[selectedOption]](this,option[selectedOption]);
        }
        if (InputManager::GetInstance().KeyPress(SDLK_x) ){
            Back(this);
        }
    }
}

void SelectionWindow::ClearChildrens(){
    Components.clear();
    option.clear();
    functions.clear();
    optionCounter = 0;
    selectedOption = -1;
}

void SelectionWindow::AddOption(UIBase *ui,std::function<void(UIBase *,UIBase*)> func){

    AddComponent(ui);
    ui->SetPosition(Point(8,16*optionCounter+title->GetTextObject()->GetHeight()+3));
    option[optionCounter] = ui;
    optionCounter++;
    functions[ui] = func;
    if (selectedOption == -1){
        selectedOption = optionCounter-1;
    }
    box.h = std::max(box.h,16*optionCounter+title->GetTextObject()->GetHeight()+3);
}



void SelectionWindow::Render(Point where){
    RenderHelp::DrawSquareColorA(box.x,box.y,box.w,box.h,style.bg[0] ,style.bg[1] ,style.bg[2] ,style.bg[3] );

    RenderHelp::DrawSquareColorA(box.x,box.y,box.w,box.h,style.fg[0] ,style.fg[1] ,style.fg[2] ,style.fg[3],true );
    RenderHelp::DrawLineColorA(box.x+1,box.y+title->GetTextObject()->GetHeight()+3,box.x+box.w-2,box.y+title->GetTextObject()->GetHeight()+3,255,255,255,255);



    where.x = box.x;
    where.y = box.y;
    UIBase::Render(where);
    if (selectedOption != -1){
        arrow.Render(option[selectedOption]->box.x-arrow.GetWidth(),option[selectedOption]->GetY());
    }
}
