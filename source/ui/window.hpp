
#include <cstdlib>
#include <iostream>

#include <list>
#include <string>
#include "base.hpp"

#ifndef WINDOWHJ
#define WINDOWHJ

class Window : public UIBase{
    public:

        Window(Rect dimensions,UIBase *owner=nullptr);
        Window();
        ~Window();
        void Input();
        void SetHighlightEnabled(bool enabled){
            highlight = enabled;
        }
        std::function<void(UIBase*,float dt)> OnUpdate;
        void Update(float dt);

        void Render(Point where=Point());
    private:
        bool highlight;



};

/*

class Button: public WindowComponent{
    public:
        //X and Y are relatives to Mother
        Button(int x,int y,std::string str,Style st,Window *mother,int ID);
        Button(int x,int y,int h,int w,std::string str,Style sta,Window *mommy,int ID);
        ~Button();
        void Render();
        void Update(float dt);
        void SetText(std::string t){ txt->SetText(t); text = t; };
        void SetSprite(std::string sprpath);
    private:
        std::string text;
        Text *txt;
        Window *mother;
        Sprite *l,*m,*r;

};

class TextLabel: public WindowComponent{
    public:
        //X and Y are relatives to Mother
        TextLabel(int x,int y,std::string str,Style st,Window *mother,int ID);
        ~TextLabel();
        void Render();
        void Update(float dt);
        void SetText(std::string t){ txt->SetText(t); text = t;};
        void SetSprite(std::string sprpath);
        //void SetAlpha(int alpha);
    private:
        std::string text;
        Text *txt;
        Window *mother;

};


class ImageLabel: public WindowComponent{
    public:
        //X and Y are relatives to Mother
        ImageLabel(int x,int y,std::string str,Style st,Window *mother,int ID);
        ~ImageLabel();
        void Render();
        void Update(float dt);
        void SetSprite(std::string sprpath);
    private:
        std::string text;
        Window *mother;

};

class TextInput: public WindowComponent{
    public:
        //X and Y are relatives to Mother
        TextInput(int x,int y,std::string str,Style st,Window *mother,int ID,int *num);
        TextInput(int x,int y,std::string str,Style st,Window *mother,int ID,char *num);
        ~TextInput();
        void Render();
        void Update(float dt);
        void SetSprite(std::string sprpath){};
    private:
        int type_input;
        int csize;
        char *caddr;
        int *addr;
        int saveat;
        std::string text;
        Text *txt;
        Window *mother;


};


class NumberController: public WindowComponent{
    public:
        //X and Y are relatives to Mother
        NumberController(int x,int y,Style st,Window *mother,int ID,int *num,int min,int max);
        ~NumberController();
        void Render();
        void Update(float dt);
        void SetSprite(std::string sprpath);
    private:
        int n,max,min;
        int *addr;
        std::string text;
        Text *txt;
        Window *mother;


};



class NumberInputWindow: public Window{
    public:
        NumberInputWindow(int x,int y,int startvalue,std::string text,std::function<void(int)> F);
        ~NumberInputWindow();
        void Render();
        void Update(float dt);
        void OnPressed(WindowComponent *component);
    private:
        int AT;
        Style st;
        std::function<void(int)> trigger;
};

class MiniTextInfo: public Window{
    public:
        MiniTextInfo(int x,int y,std::string text);
        MiniTextInfo();
        ~MiniTextInfo();
        void Render();
        void Update(float dt);
        void OnPressed(WindowComponent *component);
        void setText(std::string text);
        void Block(){Blocked=true;};
        void UnBlock(){Blocked=false;};
        void Enable(){enabled=true;};
        void Disable(){enabled=false;};
        bool isEnabled(){return enabled;};
    private:
        bool enabled;
        bool Blocked;
        std::list<std::unique_ptr<Text>> Lines;
};
*/

#endif // WINDOWHJ
