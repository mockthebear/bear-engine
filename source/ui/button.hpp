#ifndef BEUIBUTTONH
#define BEUIBUTTONH
#include "base.hpp"

class Button : public UIBase{
    public:
        Button(Point pos,std::string str,UIBase *owner);
        void Input();
        void Render(Point where=Point());
    private:
        bool hover;
        bool press;
};



#endif // BEUIBUTTONH
