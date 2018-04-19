#include "gettextwindow.hpp"
#include "../engine/renderhelp.hpp"


GetTextWindow::GetTextWindow(PointInt pos,UIBase *owner,std::function<void(std::string str)> CallBack,
                             std::string header,std::string buttonText,std::string defText):Window(Rect(pos.x,pos.y,200,200),owner){
   UI_REGISTER(GetTextWindow);
   close = false;
   this->CallBack = CallBack;
   AddComponent(new Label(Point(10,2),header,this));
   TextInput *T = new TextInput(Point(10,40),defText,this);
   T->SetFocused(true);
   T->box.w = 180;
   AddComponent(T);
   AddComponent(new Button(Point(70,180),buttonText,this));
   GetLastComponent()->OnKeyPress = [=](UIBase* obj,int key){
        if (key == SDLK_RETURN || key == SDLK_RETURN2){
            Close();
            CallBack(T->GetText());
        }
        if (key == SDLK_ESCAPE){
            Close();
        }
   };

   GetLastComponent()->OnMouseRelease = [=](UIBase* obj,int,Point){
        Close();
        CallBack(T->GetText());
   };


}


