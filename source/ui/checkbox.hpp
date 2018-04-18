#ifndef BEUICHECKBOXH
#define BEUICHECKBOXH
#include "base.hpp"

class Checkbox : public UIBase{
    public:
        Checkbox(Point pos,bool *check,std::string str,UIBase *owner);
        void Input();
        void SetFlagAddr(bool *addr){
            checked = addr;
        };
        void Render(Point where=Point());
        void SetCheck(bool c);
        void Refresh();
        std::function<void(UIBase*,bool)> OnCheck;
    private:
        bool *checked;
};



#endif // BEUICHECKBOXH

