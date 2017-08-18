#pragma once
#include "../ui/base.hpp"
//#include "luatypewrapper.hpp"

class LuaUi : public UIBase{
    public:
        LuaUi();
        ~LuaUi(){};

        void Input();

        void Render(Point where = Point());
        void Update(float dt);
        void ClearChildrens();
        void Refresh();

        void SetAsMain(bool tf){ MainWidget = tf;};


        void AddComponent2(LuaUi *ui);

        float GetY(){return MainWidget ? 0.0f : o_pos.y;};
        float GetX(){return MainWidget ? 0.0f : o_pos.x;};

        float GetTrueY(){return o_pos.y;};
        float GetTrueX(){return o_pos.x;};

        void SetX(float x){
            o_pos.x = x;
            NotifyChildrens();
        };
        void SetY(float y){
            o_pos.y = y;
            NotifyChildrens();
        };

        float GetScreenY(){return box.y;};
        float GetScreenX(){return box.x;};

        REGISTER_GETSETTEROBJ(Width,float,box.w);
        REGISTER_GETSETTEROBJ(Height,float,box.h);

        void NotifyChildrens();

        LuaUi *GetLastComponent_Lua(){
            return (LuaUi *)GetLastComponent();
        }

        LuaUi* GetMother(){
            return (LuaUi*)mother;
        }

        LuaUi *GetChildById_Lua(std::string name){
            return (LuaUi *)GetChildById(name);
        }
        std::vector<LuaUi*> GetChilds();


        /*std::function<void(UIBase*,Point)> OnMouseEnter;
        std::function<void(UIBase*,Point)> OnMouseLeave;
        std::function<void(UIBase*,int,Point)> OnMousePress;
        std::function<void(UIBase*,int,Point)> OnMouseRelease;
        std::function<void(UIBase*,int)> OnKeyPress;*/
    private:
        uint64_t other;
        bool MainWidget;

};
/*
template<> struct GenericLuaReturner<LuaUi*>{
    static void Ret(LuaUi* vr,lua_State *L,bool forceTable = false){
        MakeLuaObject<LuaUi>::Make(L,vr,"LuaUi",forceTable);
    };
};
*/
