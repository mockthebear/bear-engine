#ifndef BEUINUMBERINPUTH
#define BEUINUMBERINPUTH
#include "base.hpp"

class NumberInput : public UIBase{
    public:
        NumberInput(Point pos,int number=0,UIBase *owner=nullptr);
        NumberInput(Point pos,int *number,UIBase *owner=nullptr);
        int GetNumber(){return atoi(txt.GetText().c_str());};
        void Input();
        void Render(Point where=Point());
        void Update(float dt);
    private:
        int *addr;
        float caret;
        bool showCaret;
};



#endif // BEUINUMBERINPUTH


