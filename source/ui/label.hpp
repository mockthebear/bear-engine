#ifndef BEUILABELH
#define BEUILABELH
#include "base.hpp"

class Label : public UIBase{
    public:
        Label(Point pos,std::string str,UIBase *owner);
        Label(Point pos,Text t,UIBase *owner);
        Label(Point pos,Sprite sp,UIBase *owner);
        void Render(Point where=Point());
};



#endif // BEUILABELH
