
#include <cstdlib>
#include <iostream>

#include <map>
#include <string>
#include "base.hpp"

#ifndef SelectionWindowHJA
#define SelectionWindowHJA

class SelectionWindow : public UIBase{
    public:
        SelectionWindow();
        SelectionWindow(Rect sz);
        ~SelectionWindow();
        void Update(float dt);
        void Input();
        void SetTitle(std::string name);
        void Render(Point where=Point());
        void AddOption(UIBase *ui,std::function<void(UIBase *,UIBase*)> func);
        void ClearChildrens();
        void SkipFrame(){skip = true;}
        std::function <void(UIBase*)> Back;
    private:
        int optionCounter;
        int selectedOption;
        UIBase *title;
        std::map<uint32_t,UIBase *> option;
        std::map<UIBase *,std::function<void(UIBase *,UIBase*)>> functions;
        Text arrow;
        bool skip;



};

#endif

