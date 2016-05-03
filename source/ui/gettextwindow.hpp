#ifndef GETWINDOWSTEXTBEH


#define GETWINDOWSTEXTBEH

#include "base.hpp"
#include "window.hpp"
#include "label.hpp"
#include "textinput.hpp"
#include "button.hpp"
#include <functional>
class GetTextWindow: public Window{
    public:
        GetTextWindow(PointInt pos,UIBase *owner,std::function<void(std::string str)> CallBack,std::string header,std::string buttonText="ok",std::string defText="");

    private:
        std::function<void(std::string str)> CallBack;

};
#endif // GETWINDOWSTEXTBEH
