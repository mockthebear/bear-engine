#ifndef BEUIBASEH
#define BEUIBASEH
#include <memory>
#include <vector>
#include <functional>
#include <unordered_map>
#include "../engine/text.hpp"
#include "../engine/sprite.hpp"

enum TypeComponent{
    COMPONENT_GENERIC=0,
    COMPONENT_BUTTON,
    COMPONENT_TEXTLABEL,


    COMPONENT_ITEM,

    COMPONENT_END,
};

#define UI_REGISTER(Type) hashIt(typeid(Type).hash_code()); GenerateId()


class UIStyle{
    public:
        UIStyle(){
            fontSize=15;
            extrax = extray = 0;
            fg[0] =fg[1] =fg[2] =fg[3] = 0;
            bg[0] = 100;
            bg[1] = 120;
            bg[2] = 100;
            bg[3] = 255;
            ncolors = 0;

            txtstyle = TEXT_SOLID;
            fontfile = "data/UnB-Office_Regular.ttf";
        };
        int fontSize,ncolors,extrax,extray;
        unsigned char fg[4],bg[4];
        TextStyle txtstyle;
        bool custom;
        std::string fontfile;

    static UIStyle BaseStyle;

};

class UIBase{
    public:
        UIBase();
        ~UIBase(){};



        virtual void Input();

        virtual void Render(Point where = Point());
        virtual void Update(float dt);
        virtual void ClearChildrens();
        void SetFocused(bool t){focused=t;};
        void SetAlpha(int t){
            alpha=t;
            if (txt.IsWorking()){
                txt.SetAlpha(alpha);
            }
        };
        void Close(){
            close = true;
        }
        void SetPosition(Point p){
            box.x = p.x + (mother ? mother->box.x : 0);
            box.y = p.y + (mother ? mother->box.y : 0);
        }

        bool SetText(std::string str){
            if (txt.IsWorking()){
                txt.SetStyle(style.txtstyle);
                txt.SetColor({style.fg[0],style.fg[1],style.fg[2]});
                txt.SetText(str);
                Refresh();
            }
            return false;
        }
        std::string GetText(){
            return txt.GetText();
        }
        void SetText(Text t){
            txt = t;
            Refresh();
        }
        int GetID(){
            return ID;
        };
        bool IsDead(){
            return close;
        }
        bool Is(int thing){
            return thing == GetHash_internal();
        };
        bool IsInside(int x,int y){
            return box.IsInside(x,y);
        };
        void SetSprite(Sprite sp_){
            sp = sp_;
            Refresh();
        };
        void SetSprite(std::string path){
            SetSprite(Sprite(path.c_str()));
        };
        bool GetHidden(){
            return hidden;
        };
        void Hide(){
            hidden = true;
        };
        void Show(){
            hidden = false;
        };
        Text* GetTextObject(){
            return &txt;
        }
        void Refresh();
        void AddComponent(UIBase *ui);
        virtual int GetY(){return box.y;};
        Rect box;

        virtual void NotifyChildrens();

        std::function<void(UIBase*,int,Point)> OnMousePress;
        std::function<void(UIBase*,int,Point)> OnMouseRelease;
        std::function<void(UIBase*,int)> OnKeyPress;
        UIBase *GetLastComponent(){
            return Components.back().get();
        }

        std::vector<std::unique_ptr<UIBase>> Components;
        UIStyle style;


        uint8_t Color[4];
    protected:

        Point o_pos;
        bool hidden,close;
        static int g_ID;
        int ID,alpha;
        bool focused;

        Sprite sp;
        Text txt;
        UIBase *mother;


        void hashIt(int var){hash = var;};
        inline int GetHash_internal(){return hash;};
        void GenerateId(){ID = g_ID++;};
    private:
        int hash;
};







#endif // BEUIBASEH
