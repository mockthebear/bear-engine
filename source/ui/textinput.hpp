#ifndef BEUITEXTINPUTH
#define BEUITEXTINPUTH
#include "base.hpp"

class TextInput : public UIBase{
    public:
        TextInput(Point pos,std::string str,UIBase *owner);
        void Input();
        void Update(float dt);
        void Render(Point where=Point());
    private:
        bool Upper;
        float caret;
        bool showCaret;
};



#endif // BEUITEXTINPUTH

