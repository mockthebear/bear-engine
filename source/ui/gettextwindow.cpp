#include "gettextwindow.hpp"
#include "../engine/renderhelp.hpp"


GetTextWindow::GetTextWindow(PointInt pos,UIBase *owner,std::function<void(std::string str)> CallBack,
                             std::string header,std::string buttonText,std::string defText):Window(Rect(pos.x,pos.y,200,200),owner){
   UI_REGISTER(GetTextWindow);
   close = false;
   this->CallBack = CallBack;
   AddComponent(new Label(Point(80,2),header,this));
   TextInput *T = new TextInput(Point(70,40),defText,this);
   AddComponent(T);
   AddComponent(new Button(Point(70,180),buttonText,this));
   GetLastComponent()->OnMouseRelease = [=](UIBase* obj,int,Point){
        Close();
        CallBack(T->GetText());
   };


}


