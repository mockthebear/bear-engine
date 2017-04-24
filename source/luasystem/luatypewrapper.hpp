#ifndef DISABLE_LUAINTERFACE
    #pragma once
    #include LUA_INCLUDE






/*
    Function to check if given type is as it is
*/

template<typename T1> struct GenericLuaType{
     static bool Is(lua_State *L,int stack=-1){
        return false;
    };
};

/*
    Function used to get values
*/
template<typename T1> struct GenericLuaGetter{
     static T1 Call(lua_State *L,int stackPos = -1,bool pop=true){
       Console::GetInstance().AddText("[GenericLuaGetter] unresolved type %s return as integer\n",(typeid(T1).name()));
	   return T1();//lua_tonumber(L,stackPos);
    };
    static T1 Empty;
};


/*
    Used to push values
*/
template<typename T1> struct GenericLuaReturner{
    static void Ret(T1 vr,lua_State *L){
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
     static void Ret(int vr,lua_State *L){
         lua_pushnumber(L,vr);
    };
};

template<>
    struct GenericLuaGetter<int> {
     static int Call(lua_State *L,int stackPos = -1,bool pop=true){
        int n = 0;
        if (lua_isnil(L,stackPos)){
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
     static void Ret(uint8_t vr,lua_State *L){
         lua_pushnumber(L,vr);
    };
};


template<>
    struct GenericLuaGetter<uint8_t> {
     static uint8_t Call(lua_State *L,int stackPos = -1,bool pop=true){
        uint8_t n = 0;
        if (lua_isnil(L,stackPos)){
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
     static void Ret(uint32_t vr,lua_State *L){
         lua_pushnumber(L,vr);
    };
};

template<>
    struct GenericLuaGetter<uint32_t> {
     static uint32_t Call(lua_State *L,int stackPos = -1,bool pop=true){
        uint32_t n = 0;
        if (lua_isnil(L,stackPos)){
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
     static void Ret(uint64_t vr,lua_State *L){
         lua_pushnumber(L,vr);
    };
};

template<>
    struct GenericLuaGetter<uint64_t> {
     static uint64_t Call(lua_State *L,int stackPos = -1,bool pop=true){
        uint64_t n = 0;
        if (lua_isnil(L,stackPos)){
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
     static void Ret(int vr,lua_State *L){
         lua_pushnil(L);
    };
};

/*
    float
*/

template<> struct GenericLuaReturner<float>{
     static void Ret(float vr,lua_State *L){
         lua_pushnumber(L,vr);
    };
};

template<>
    struct GenericLuaGetter<float> {
     static float Call(lua_State *L,int stackPos = -1,bool pop=true){
        float n = 0;
        if (lua_isnil(L,stackPos)){
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
     static void Ret(std::string vr,lua_State *L){
         lua_pushstring(L,vr.c_str());
    };
};

template<>
    struct GenericLuaGetter<std::string> {
     static std::string Call(lua_State *L,int stackPos = -1,bool pop=true){
        std::string n;
        if (lua_isnil(L,stackPos)){
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
     static void Ret(Point vr,lua_State *L){
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
            Console::GetInstance().AddText("[LuaBase][Warning]Argument %d is nil",lua_gettop(L));
        }else{
            if (!lua_istable(L,-1)){
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
    static Point Empty;
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
            Console::GetInstance().AddText("[LuaBase][Warning]Argument %d is nil",lua_gettop(L));
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


template<>
    struct GenericLuaGetter<Sprite> {
     static Sprite Call(lua_State *L,int stackPos = -1,bool pop=true){
        Sprite pt;
        if (lua_isnil(L,stackPos)){
            Console::GetInstance().AddText("[LuaBase][Warning]Argument %d is nil",lua_gettop(L));
        }else{
            if (!lua_istable(L,stackPos)){

                Console::GetInstance().AddText("[LuaBase][Warning]Argument %d is not a table",lua_gettop(L));
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
                                std::cout << "No sprite caralho\n";
                        }

                        std::cout << *sp << "\n";
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
