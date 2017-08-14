#ifndef DISABLE_LUAINTERFACE
#pragma once
#include LUA_INCLUDE
#include "../engine/object.hpp"
#include "luacaller.hpp"
#include "luaui.hpp"

#define isthis(type,arg) if (type == arg) vaav = #arg;

struct QuickDebug{
    static void DisplayCurrent(lua_State *L){
        lua_Debug ar;
        lua_getstack(L, 1, &ar);
        lua_getinfo(L, "nSl", &ar);
        Console::GetInstance().AddText("At %s line %d %s",ar.source, ar.currentline,LuaManager::lastCalled.c_str()  );
    };
    static void DumpLua(lua_State *L){
        for (int i=0;i<10;i++){
            int type = lua_type(L,-i);
            std::string vaav = "?";
            isthis(type,LUA_TNIL);
            isthis(type,LUA_TNUMBER);
            isthis(type,LUA_TBOOLEAN);
            isthis(type,LUA_TSTRING);
            isthis(type,LUA_TTABLE);
            isthis(type,LUA_TFUNCTION);
            isthis(type,LUA_TUSERDATA);
            isthis(type,LUA_TTHREAD);
            isthis(type,LUA_TLIGHTUSERDATA);
            Console::GetInstance().AddText("[Dump: %d] %s",-i,vaav.c_str());
        }
    };
};


template<class T> struct IndexerHelper;
template<class T> struct ClassRegister;

template<typename T1> struct MakeLuaObject{
    static int Make(lua_State *L,T1* obj,std::string name,bool forceOutside = false){
        if (obj == nullptr){
            lua_pushnil(L);
            return 1;
        }
        bool isNil = false;
        lua_getglobal(L, "__REFS"); // insert
        lua_pushnumber(L, 0);
        lua_gettable(L, -2 );
        lua_pushnumber(L,(uint64_t)obj);
        lua_gettable(L, -2 );
        if (lua_isnil(L,-1)){
            isNil = true;
        }else{
            return 1;
        }
        //QuickDebug::DumpLua(L);




        if (lua_istable(L,1) && forceOutside == false){
            if (isNil){
                lua_getglobal(L, "__REFS"); // insert
                lua_pushnumber(L, 0);
                lua_gettable(L, -2 );
                lua_pushnumber(L,(uint64_t)obj);
            }

            luaL_checktype(L, 1, LUA_TTABLE);
            lua_newtable(L);
            lua_pushstring(L,"id");
            lua_pushnumber(L,(uint64_t)obj);
            lua_settable(L,-3);
            lua_pushstring(L,"type");
            lua_pushstring(L,typeid(T1).name());
            lua_settable(L,-3);
            lua_pushstring(L,"data");

            ClassRegister<T1>::MakeTypeObserver(L,obj,IndexerHelper<T1>::Index,IndexerHelper<T1>::Newindex);
            lua_pushvalue(L,1);
            lua_setmetatable(L, -2);
            lua_pushvalue(L,1);
            lua_setfield(L, 1, "__index");
            T1 **usr = static_cast<T1**>(lua_newuserdata(L, sizeof(T1)));
            *usr = obj;

            lua_getglobal(L, name.c_str());
            lua_setmetatable(L, -2);
            lua_setfield(L, -2, "__self");

            if (isNil){
                lua_settable(L, -3 );
                lua_pop(L,1);
                lua_getglobal(L, "__REFS"); // request to return
                lua_pushnumber(L, 0);
                lua_gettable(L, -2 );
                lua_pushnumber(L,(uint64_t)obj);
                lua_gettable(L, -2);
            }

            return 1;
        }else{
            if (isNil){
                lua_getglobal(L, "__REFS"); // insert
                lua_pushnumber(L, 0);
                lua_gettable(L, -2 );
                lua_pushnumber(L,(uint64_t)obj);
            }

            lua_newtable(L);
            lua_pushstring(L,"id");
            lua_pushnumber(L,(uint64_t)obj);
            lua_settable(L,-3);
            lua_pushstring(L,"type");
            lua_pushstring(L,typeid(T1).name());
            lua_settable(L,-3);
            lua_pushstring(L,"data");
            ClassRegister<T1>::MakeTypeObserver(L,obj,IndexerHelper<T1>::Index,IndexerHelper<T1>::Newindex);
            lua_pushvalue(L,1);
            lua_setmetatable(L, -2);


            lua_pushvalue(L,1);
            lua_setmetatable(L, -2);
            lua_pushvalue(L,1);

            lua_setfield(L, 0, "__index");
            T1 **usr = static_cast<T1**>(lua_newuserdata(L, sizeof(T1)));
            *usr = obj;


            lua_getglobal(L, name.c_str());
            lua_setmetatable(L, -2);
            lua_setfield(L, -2, "__self");


            luaL_getmetatable(L, name.c_str());
            lua_setmetatable(L, -2);


            if (isNil){
                lua_settable(L, -3 );
                lua_pop(L,1);
                lua_getglobal(L, "__REFS"); // request to return
                lua_pushnumber(L, 0);
                lua_gettable(L, -2 );
                lua_pushnumber(L,(uint64_t)obj);
                lua_gettable(L, -2);
            }


            return 1;
        }
    };
};


/*
    Function to check if given type is as it is
*/

template<typename T1> struct GenericLuaType{
     static bool Is(lua_State *L,int stack=-1){
        return false;
    };
};


template<typename T1> struct GenericLuaGetter{
     static T1 Call(lua_State *L,int stackPos = -1,bool pop=true){
        T1 pt;
        if (lua_isnil(L,stackPos)){
            Console::GetInstance().AddText("[LuaBase][Warning]Argument %d is nil",lua_gettop(L));
        }else{
             if (!lua_istable(L,stackPos)){
                QuickDebug::DisplayCurrent(L);
                Console::GetInstance().AddText("[LuaBase][Warning][%s->%s]Undefined type. Returning as number.",LuaManager::lastCalled.c_str(),typeid(T1).name(),lua_gettop(L));
                int ret = lua_tonumber(L,stackPos);
                T1 *pt2 = &pt;
                *((int*)pt2) = ret;
                return pt;
            }else{
                std::string disName = typeid(T1).name();
                bool typeFine = false;
                std::string otherType = "?";
                T1** sp = nullptr;
                lua_pushnil(L);
                while(lua_next(L, -2) != 0)
                {
                    if (std::string(lua_tostring(L, -2)) == "__self"){
                        sp = (T1**)lua_touserdata(L,-1);
                        if (!sp){
                            bear::out << "No obj\n";
                        }
                    }
                    if (std::string(lua_tostring(L, -2)) == "type"){
                        otherType = std::string(lua_tostring(L, -1));
                        if (otherType == typeid(T1).name()){
                            typeFine = true;
                        }
                    }
                    lua_pop(L,1);
                }
                if (!sp){
                    Console::GetInstance().AddText("[LuaBase][Warning] __self is not in the table");
                    return T1();
                }

                if (!typeFine){
                    disName = disName + " Instead is " + otherType;
                    Console::GetInstance().AddText("[LuaBase][Warning] Type is not %s.",disName);
                    return T1();
                }
                pt = *(*sp);
                if (pop)
                    lua_pop(L,1);
                return pt;
            }
        }
        if (pop)
            lua_pop(L,1);
        return T1();

    };

    static T1 Empty;
};




/*
    Used to push values
*/
template<typename T1> struct GenericLuaReturner{
    static void Ret(T1 vr,lua_State *L,bool forceTable = false){
        QuickDebug::DisplayCurrent(L);
        Console::GetInstance().AddTextInfo(utils::format("[GenericLuaReturner]Undefined type called %s.",typeid(T1).name()));
        lua_pushnil(L);
    };
};

/*
    Specializations
*/

/*
    Int
*/

template<> struct GenericLuaReturner<int>{
     static void Ret(int vr,lua_State *L,bool forceTable = false){
         lua_pushnumber(L,vr);
    };
};

template<>
    struct GenericLuaGetter<int> {
     static int Call(lua_State *L,int stackPos = -1,bool pop=true){
        int n = 0;
        if (lua_isnil(L,stackPos)){
            QuickDebug::DisplayCurrent(L);
            Console::GetInstance().AddText("[LuaBase][Warning]Argument %d is nil",lua_gettop(L));
            n = -1;
        }
        n = lua_tonumber(L,stackPos);
        if (pop)
            lua_pop(L,1);
        return n;
    };
    static int Empty;
};

template<>
    struct GenericLuaType<int>{
        static bool Is(lua_State *L,int stack=-1){
            return lua_isnumber(L,-1);
        };
};

/*
    uint8
*/

template<> struct GenericLuaReturner<uint8_t>{
     static void Ret(uint8_t vr,lua_State *L,bool forceTable = false){
         lua_pushnumber(L,vr);
    };
};


template<>
    struct GenericLuaGetter<uint8_t> {
     static uint8_t Call(lua_State *L,int stackPos = -1,bool pop=true){
        uint8_t n = 0;
        if (lua_isnil(L,stackPos)){
            QuickDebug::DisplayCurrent(L);
            Console::GetInstance().AddText("[LuaBase][Warning]Argument %d is nil",lua_gettop(L));
            n = -1;
        }else{
            n = lua_tonumber(L,stackPos);
        }
        if (pop)
            lua_pop(L,1);
        return n;
    };
    static uint8_t Empty;
};

template<>
    struct GenericLuaType<uint8_t>{
        static bool Is(lua_State *L,int stack=-1){
            return lua_isnumber(L,-1);
        };
};

/*
    uint32
*/

template<> struct GenericLuaReturner<uint32_t>{
     static void Ret(uint32_t vr,lua_State *L,bool forceTable = false){
         lua_pushnumber(L,vr);
    };
};

template<>
    struct GenericLuaGetter<uint32_t> {
     static uint32_t Call(lua_State *L,int stackPos = -1,bool pop=true){
        uint32_t n = 0;
        if (lua_isnil(L,stackPos)){
            QuickDebug::DisplayCurrent(L);
            Console::GetInstance().AddText("[LuaBase][Warning]Argument %d is nil",lua_gettop(L));
            n = -1;
        }else{
            n = lua_tonumber(L,stackPos);
        }
        if (pop)
            lua_pop(L,1);
        return n;
    };
    static uint32_t Empty;
};

template<>
    struct GenericLuaType<uint32_t>{
        static bool Is(lua_State *L,int stack=-1){
            return lua_isnumber(L,-1);
        };
};

/*
    uint64
*/

template<> struct GenericLuaReturner<uint64_t>{
     static void Ret(uint64_t vr,lua_State *L,bool forceTable = false){
         lua_pushnumber(L,vr);
    };
};

template<>
    struct GenericLuaGetter<uint64_t> {
     static uint64_t Call(lua_State *L,int stackPos = -1,bool pop=true){
        uint64_t n = 0;
        if (lua_isnil(L,stackPos)){
            QuickDebug::DisplayCurrent(L);
            Console::GetInstance().AddText("[LuaBase][Warning]Argument %d is nil",lua_gettop(L));
            n = -1;
        }else{
            n = lua_tonumber(L,stackPos);
        }
        if (pop)
            lua_pop(L,1);
        return n;
    };
    static uint64_t Empty;
};

/*
    void
*/

template<> struct GenericLuaReturner<void>{
     static void Ret(int vr,lua_State *L,bool forceTable = false){
         lua_pushnil(L);
    };
};

/*
    float
*/

template<> struct GenericLuaReturner<float>{
     static void Ret(float vr,lua_State *L,bool forceTable = false){
         lua_pushnumber(L,vr);
    };
};

template<>
    struct GenericLuaGetter<float> {
     static float Call(lua_State *L,int stackPos = -1,bool pop=true){
        float n = 0;
        if (lua_isnil(L,stackPos)){
            QuickDebug::DisplayCurrent(L);
            Console::GetInstance().AddText("[LuaBase][Warning]Argument %d is nil",lua_gettop(L));
            n = -1;
        }else{
            n = lua_tonumber(L,stackPos);
        }
        if (pop)
            lua_pop(L,1);
        return n;
    };
    static float Empty;
};

template<>
    struct GenericLuaType<float>{
        static bool Is(lua_State *L,int stack=-1){
            return lua_isnumber(L,-1);
        };
};

/*
    std::string
*/

template<> struct GenericLuaReturner<std::string>{
     static void Ret(std::string vr,lua_State *L,bool forceTable = false){
         lua_pushstring(L,vr.c_str());
    };
};

template<>
    struct GenericLuaGetter<std::string> {
     static std::string Call(lua_State *L,int stackPos = -1,bool pop=true){
        std::string n;
        if (lua_isnil(L,stackPos)){
            QuickDebug::DisplayCurrent(L);
            Console::GetInstance().AddText(utils::format("[LuaBase][Warning]Argument %d is nil",lua_gettop(L)));
            n = "nil";
        }else{
            n = lua_tostring(L,stackPos);
        }
        if (pop)
            lua_pop(L,1);
        return n;
    };
    static std::string Empty;
};


template<>
    struct GenericLuaType<std::string>{
        static bool Is(lua_State *L,int stack=-1){
            return lua_isstring(L,-1);
        };
};

/*
    Point
*/

template<> struct GenericLuaReturner<Point>{
     static void Ret(Point vr,lua_State *L,bool forceTable = false){
        lua_newtable(L);
        lua_pushstring(L,"x");
        lua_pushnumber(L,vr.x);
        lua_settable(L,-3);
        lua_pushstring(L,"y");
        lua_pushnumber(L,vr.y);
        lua_settable(L,-3);
    };
};


template<>
    struct GenericLuaGetter<Point> {
     static Point Call(lua_State *L,int stackPos = -1,bool pop=true){
        Point pt;
        if (lua_isnil(L,-1)){
            QuickDebug::DisplayCurrent(L);
            Console::GetInstance().AddText("[LuaBase][Warning][Point]Argument %d is nil",lua_gettop(L));
        }else{
            if (!lua_istable(L,-1)){
                QuickDebug::DisplayCurrent(L);
                Console::GetInstance().AddText("[LuaBase][Warning][Point]Argument %d is not a table",lua_gettop(L));
            }else{
                lua_pushnil(L);
                while(lua_next(L, stackPos-1) != 0)
                {
                    if (std::string(lua_tostring(L, -2)) == "x"){
                        pt.x = lua_tonumber(L, -1);
                    }
                    if (std::string(lua_tostring(L, -2)) == "y"){
                        pt.y = lua_tonumber(L, -1);
                    }
                    lua_pop(L,1);
                }
            }
        }
        if (pop)
            lua_pop(L,1);
        return pt;
    };
    static Point Empty;
};

template<>
    struct GenericLuaGetter<SDL_Color> {
     static SDL_Color Call(lua_State *L,int stackPos = -1,bool pop=true){
        SDL_Color pt = {0,0,0,255};
        if (lua_isnil(L,-1)){
            QuickDebug::DisplayCurrent(L);
            Console::GetInstance().AddText("[LuaBase][Warning][SDL_Color]Argument %d is nil",lua_gettop(L));
        }else{
            if (!lua_istable(L,-1)){
                QuickDebug::DisplayCurrent(L);
                Console::GetInstance().AddText("[LuaBase][Warning][SDL_Color]Argument %d is not a table",lua_gettop(L));
            }else{
                lua_pushnil(L);
                while(lua_next(L, stackPos-1) != 0)
                {
                    if (std::string(lua_tostring(L, -2)) == "r"){
                        pt.r = lua_tonumber(L, -1);
                    }
                    if (std::string(lua_tostring(L, -2)) == "g"){
                        pt.g = lua_tonumber(L, -1);
                    }
                    if (std::string(lua_tostring(L, -2)) == "b"){
                        pt.b = lua_tonumber(L, -1);
                    }
                    if (std::string(lua_tostring(L, -2)) == "a"){
                        pt.a = lua_tonumber(L, -1);
                    }

                    lua_pop(L,1);
                }
            }
        }
        if (pop)
            lua_pop(L,1);
        return pt;
    };
    static SDL_Color Empty;
};


template<>
    struct GenericLuaType<Point>{
        static bool Is(lua_State *L,int stack=-1){
            return lua_istable(L,-1);
        };
};

/*
    todo
*/

template<> struct GenericLuaReturner<Sprite*>{
    static void Ret(Sprite* vr,lua_State *L,bool forceTable = false){
        MakeLuaObject<Sprite>::Make(L,vr,"Sprite",forceTable);
    };
};

template<> struct GenericLuaReturner<ColorReplacer*>{
    static void Ret(ColorReplacer* vr,lua_State *L,bool forceTable = false){

        MakeLuaObject<ColorReplacer>::Make(L,vr,"ColorReplacer",forceTable);


    };
};



template<> struct GenericLuaReturner<GameObject*>{
    static void Ret(GameObject* vr,lua_State *L,bool forceTable = false){
        MakeLuaObject<GameObject>::Make(L,vr,"GameObject",forceTable);
    };
};

template<> struct GenericLuaReturner<LuaUi*>{
    static void Ret(LuaUi* vr,lua_State *L,bool forceTable = false){
        MakeLuaObject<LuaUi>::Make(L,vr,"LuaUi",forceTable);
    };
};



template<typename T> struct GenericLuaReturner<std::vector<T>>{
     static void Ret(std::vector<T> vr,lua_State *L){
        lua_newtable(L);
        int index = 1;
        for (auto &num : vr){
            lua_pushnumber( L, index );
            GenericLuaReturner<T>::Ret(num,L,true);
            lua_settable(L,-3);
            index++;
        }
     };
};

template<> struct GenericLuaReturner<Rect>{
     static void Ret(Rect vr,lua_State *L){
        lua_newtable(L);
        lua_pushstring(L,"x");
        lua_pushnumber(L,vr.x);
        lua_settable(L,-3);
        lua_pushstring(L,"y");
        lua_pushnumber(L,vr.y);
        lua_settable(L,-3);
        lua_pushstring(L,"w");
        lua_pushnumber(L,vr.w);
        lua_settable(L,-3);
        lua_pushstring(L,"h");
        lua_pushnumber(L,vr.h);
        lua_settable(L,-3);
    };
};
template<> struct GenericLuaReturner<const char*>{
     static void Ret(const char*c,lua_State *L){
         lua_pushstring(L,c);
    };
};



template<> struct GenericLuaReturner<bool>{
     static void Ret(bool vr,lua_State *L){
         lua_pushboolean(L,vr);
    };
};





template<>
    struct GenericLuaGetter<double> {
     static double Call(lua_State *L,int stackPos = -1,bool pop=true){
        double n = 0;
        if (lua_isnil(L,stackPos)){
            QuickDebug::DisplayCurrent(L);
            Console::GetInstance().AddText("[LuaBase][Warning]Argument %d is nil",lua_gettop(L)  );
            /*Bolo::DumpLua(L);*/
            n = -1;
        }else{
            n = lua_tonumber(L,stackPos);
        }
        if (pop)
            lua_pop(L,1);
        return n;
    };
    static double Empty;
};





template<>
    struct GenericLuaGetter<const char*> {
     static const char transfer[512];
     static const char* Call(lua_State *L,int stackPos = -1,bool pop=true){
        std::string n;
        if (lua_isnil(L,stackPos)){
            Console::GetInstance().AddText(utils::format("[LuaBase][Warning]Argument %d is nil",lua_gettop(L)));
            strcpy((char*)transfer,"nil");
        }else{
            strcpy((char*)transfer,lua_tostring(L,stackPos));
        }
        if (pop)
            lua_pop(L,1);
        return transfer;
    };
    static const char Empty[10];
};

template<>
    struct GenericLuaGetter<char*> {
     static char transfer[512];
     static char* Call(lua_State *L,int stackPos = -1,bool pop=true){
        std::string n;
        if (lua_isnil(L,stackPos)){
            Console::GetInstance().AddText(utils::format("[LuaBase][Warning]Argument %d is nil",lua_gettop(L)));
            strcpy(transfer,"nil");
        }else{
            strcpy(transfer,lua_tostring(L,stackPos));
        }
        if (pop)
            lua_pop(L,1);
        return transfer;
    };
    static char Empty[10];
};

template<>
    struct GenericLuaGetter<bool> {
     static bool Call(lua_State *L,int stackPos = -1,bool pop=true){
        bool n = false;
        if (lua_isnil(L,stackPos)){
            Console::GetInstance().AddText("[LuaBase][Warning]Argument %d is nil",lua_gettop(L));
            n = false;
        }else{
            n = lua_toboolean(L,stackPos);
        }
        if (pop)
            lua_pop(L,1);
        return n;
    };
    static bool Empty;
};

/*
template<>
    struct GenericLuaGetter<Sprite> {
     static Sprite Call(lua_State *L,int stackPos = -1,bool pop=true){
        Sprite pt;
        if (lua_isnil(L,stackPos)){
            Console::GetInstance().AddText("[LuaBase][Warning]Argument %d is nil",lua_gettop(L));
        }else{
            if (!lua_istable(L,stackPos)){

                Console::GetInstance().AddText("[LuaBase][Warning][Sprite]Argument %d is not a table",lua_gettop(L));
            }else{
                bool typeFine = false;
                std::string otherType = "?";
                Sprite** sp = nullptr;
                lua_pushnil(L);
                while(lua_next(L, -2) != 0)
                {
                    if (std::string(lua_tostring(L, -2)) == "__self"){
                        sp = (Sprite**)lua_touserdata(L,-1);
                        if (!sp){
                            bear::out << "No sprite\n";
                        }
                    }
                    if (std::string(lua_tostring(L, -2)) == "type"){
                        otherType = std::string(lua_tostring(L, -1));
                        if (otherType == typeid(Sprite).name()){
                            typeFine = true;
                        }
                    }
                    lua_pop(L,1);
                }
                if (!sp){
                    Console::GetInstance().AddText("[LuaBase][Warning] __self is not in the table");
                    return pt;
                }

                if (!typeFine){
                    Console::GetInstance().AddText("[LuaBase][Warning] Type is not Sprite. Instead is %s.",otherType);
                    return pt;
                }
                pt = *(*sp);
            }
        }
        if (pop)
            lua_pop(L,1);
        return pt;
    };
    static bool Empty;
};
*/

template<class T>
    struct GenericLuaGetter<T*> {
     static T* Call(lua_State *L,int stackPos = -1,bool pop=true){
        T* pt=nullptr;
        if (lua_isnil(L,stackPos)){
            Console::GetInstance().AddText("[LuaBase][Warning]Argument %d is nil",lua_gettop(L));
        }else{
            if (!lua_istable(L,stackPos)){
                Console::GetInstance().AddText("[LuaBase][Warning][%s]Argument %d is not a table",typeid(T).name(),lua_gettop(L));
            }else{
                std::string disName = typeid(T).name();
                bool typeFine = false;
                std::string otherType = "?";
                T** sp = nullptr;
                lua_pushnil(L);
                while(lua_next(L, -2) != 0)
                {
                    if (std::string(lua_tostring(L, -2)) == "__self"){
                        sp = (T**)lua_touserdata(L,-1);
                        if (!sp){
                            bear::out << "No obj\n";
                        }
                    }
                    if (std::string(lua_tostring(L, -2)) == "type"){
                        otherType = std::string(lua_tostring(L, -1));
                        if (otherType == typeid(T).name()){
                            typeFine = true;
                        }
                    }
                    lua_pop(L,1);
                }
                if (!sp){
                    Console::GetInstance().AddText("[LuaBase][Warning] __self is not in the table");
                    return pt;
                }

                if (!typeFine){
                    disName = disName + " Instead is " + otherType;
                    Console::GetInstance().AddText("[LuaBase][Warning] Type is not %s.",disName);
                    return pt;
                }
                pt = (*sp);
            }
        }
        if (pop)
            lua_pop(L,1);
        return pt;
    };
    static bool Empty;
};
/*
template<>
    struct GenericLuaGetter<CustomFont> {
     static CustomFont Call(lua_State *L,int stackPos = -1,bool pop=true){
        CustomFont pt;
        if (lua_isnil(L,stackPos)){
            Console::GetInstance().AddText("[LuaBase][Warning]Argument %d is nil",lua_gettop(L));
        }else{
            if (!lua_istable(L,stackPos)){

                Console::GetInstance().AddText("[LuaBase][Warning][CustomFont]Argument %d is not a table",lua_gettop(L));
            }else{
                bool typeFine = false;
                std::string otherType = "?";
                CustomFont** sp = nullptr;
                lua_pushnil(L);
                while(lua_next(L, -2) != 0)
                {
                    if (std::string(lua_tostring(L, -2)) == "__self"){
                        sp = (CustomFont**)lua_touserdata(L,-1);
                        if (!sp){
                                bear::out << "No CustomFont\n";
                        }
                    }
                    if (std::string(lua_tostring(L, -2)) == "type"){
                        otherType = std::string(lua_tostring(L, -1));
                        if (otherType == typeid(CustomFont).name()){
                            typeFine = true;
                        }
                    }
                    lua_pop(L,1);
                }
                if (!sp){
                    Console::GetInstance().AddText("[LuaBase][Warning] __self is not in the table");
                    return pt;
                }

                if (!typeFine){
                    Console::GetInstance().AddText("[LuaBase][Warning] Type is not CustomFont. Instead is %s.",otherType);
                    return pt;
                }
                pt = *(*sp);
            }
        }
        if (pop)
            lua_pop(L,1);
        return pt;
    };
    static bool Empty;
};


template<>
    struct GenericLuaGetter<Text> {
     static Text Call(lua_State *L,int stackPos = -1,bool pop=true){
        Text pt;
        if (lua_isnil(L,stackPos)){
            Console::GetInstance().AddText("[LuaBase][Warning]Argument %d is nil",lua_gettop(L));
        }else{
            if (!lua_istable(L,stackPos)){

                Console::GetInstance().AddText("[LuaBase][Warning][Text]Argument %d is not a table",lua_gettop(L));
            }else{
                bool typeFine = false;
                std::string otherType = "?";
                Text** sp = nullptr;
                lua_pushnil(L);
                while(lua_next(L, -2) != 0)
                {
                    if (std::string(lua_tostring(L, -2)) == "__self"){
                        sp = (Text**)lua_touserdata(L,-1);
                        if (!sp){
                                bear::out << "No CustomFont\n";
                        }
                    }
                    if (std::string(lua_tostring(L, -2)) == "type"){
                        otherType = std::string(lua_tostring(L, -1));
                        if (otherType == typeid(Text).name()){
                            typeFine = true;
                        }
                    }
                    lua_pop(L,1);
                }
                if (!sp){
                    Console::GetInstance().AddText("[LuaBase][Warning] __self is not in the table");
                    return pt;
                }

                if (!typeFine){
                    Console::GetInstance().AddText("[LuaBase][Warning] Type is not Text. Instead is %s.",otherType);
                    return pt;
                }
                pt = *(*sp);
            }
        }
        if (pop)
            lua_pop(L,1);
        return pt;
    };
    static bool Empty;
};

template<>
    struct GenericLuaGetter<ColorReplacer> {
     static ColorReplacer Call(lua_State *L,int stackPos = -1,bool pop=true){
        ColorReplacer pt;
        if (lua_isnil(L,stackPos)){
            Console::GetInstance().AddText("[LuaBase][Warning]Argument %d is nil",lua_gettop(L));
        }else{
            if (!lua_istable(L,stackPos)){

                Console::GetInstance().AddText("[LuaBase][Warning][ColorReplacer]Argument %d is not a table",lua_gettop(L));
            }else{
                bool typeFine = false;
                std::string otherType = "?";
                ColorReplacer** sp = nullptr;
                lua_pushnil(L);
                while(lua_next(L, -2) != 0)
                {
                    if (std::string(lua_tostring(L, -2)) == "__self"){
                        sp = (ColorReplacer**)lua_touserdata(L,-1);
                        if (!sp){
                                bear::out << "No color replacer\n";
                        }

                    }
                    if (std::string(lua_tostring(L, -2)) == "type"){
                        otherType = std::string(lua_tostring(L, -1));
                        if (otherType == typeid(ColorReplacer).name()){
                            typeFine = true;
                        }
                    }
                    lua_pop(L,1);
                }
                if (!sp){
                    Console::GetInstance().AddText("[LuaBase][Warning] __self is not in the table");
                    return pt;
                }

                if (!typeFine){
                    Console::GetInstance().AddText("[LuaBase][Warning] Type is not ColorReplacer. Instead is %s.",otherType);
                    return pt;
                }
                pt = *(*sp);
            }
        }
        if (pop)
            lua_pop(L,1);
        return pt;
    };
    static bool Empty;
};
*/

template<>
    struct GenericLuaGetter<PointInt> {
     static PointInt Call(lua_State *L,int stackPos = -1,bool pop=true){
        PointInt pt;
        if (lua_isnil(L,stackPos)){
            Console::GetInstance().AddText("[LuaBase][Warning]Argument %d is nil",lua_gettop(L));
        }else{
            if (!lua_istable(L,stackPos)){
                Console::GetInstance().AddText("[LuaBase][Warning]Argument %d is not a table",lua_gettop(L));
            }else{
                lua_pushnil(L);
                while(lua_next(L, stackPos-1) != 0)
                {
                    if (std::string(lua_tostring(L, -2)) == "x"){
                        pt.x = lua_tonumber(L, -1);
                    }
                    if (std::string(lua_tostring(L, -2)) == "y"){
                        pt.y = lua_tonumber(L, -1);
                    }
                    lua_pop(L,1);
                }
            }
        }
        if (pop)
            lua_pop(L,1);
        return pt;
    };
    static bool Empty;
};

template<>
    struct GenericLuaGetter<Rect> {
     static Rect Call(lua_State *L,int stackPos = -1,bool pop=true){
        Rect pt;
        if (lua_isnil(L,stackPos)){
            Console::GetInstance().AddText("[LuaBase][Warning]Argument %d is nil",lua_gettop(L));
        }else{
            if (!lua_istable(L,stackPos)){
                Console::GetInstance().AddText("[LuaBase][Warning]Argument %d is not a table",lua_gettop(L));
            }else{
                lua_pushnil(L);
                while(lua_next(L, stackPos-1) != 0)
                {
                    if (std::string(lua_tostring(L, -2)) == "x"){
                        pt.x = lua_tonumber(L, -1);
                    }
                    if (std::string(lua_tostring(L, -2)) == "y"){
                        pt.y = lua_tonumber(L, -1);
                    }
                    if (std::string(lua_tostring(L, -2)) == "w"){
                        pt.w = lua_tonumber(L, -1);
                    }
                    if (std::string(lua_tostring(L, -2)) == "h"){
                        pt.h = lua_tonumber(L, -1);
                    }
                    lua_pop(L,1);
                }
            }
        }
        if (pop)
            lua_pop(L,1);
        return pt;
    };
    static bool Empty;
};





template<>
    struct GenericLuaType<PointInt>{
        static bool Is(lua_State *L,int stack=-1){
            return lua_istable(L,-1);
        };
};


template<>
    struct GenericLuaType<char*>{
        static bool Is(lua_State *L,int stack=-1){
            return lua_isstring(L,-1);
        };
};

template<>
    struct GenericLuaType<const char*>{
        static bool Is(lua_State *L,int stack=-1){
            return lua_isstring(L,-1);
        };
};




template<>
    struct GenericLuaType<bool>{
        static bool Is(lua_State *L,int stack=-1){
            return lua_isboolean(L,-1);
        };
};


#endif
