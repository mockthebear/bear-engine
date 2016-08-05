#include "../settings/definitions.hpp"
#include "../performance/console.hpp"
#include <iostream>
#ifndef DISABLE_LUAINTERFACE


#ifndef LUATOOLSBEH
#define LUATOOLSBEH
#include LUA_INCLUDE


typedef std::function<int(lua_State*)> LuaCFunctionLambda;

class LuaMember{
    public:
        LuaMember(){
            field = "nil";
            type = "nil";
        };
        std::string field,type;
        static std::string GetType(lua_State *L){
            int tp = lua_type(L,-1);
            std::string newstr = lua_typename(L, tp);
            return newstr;
        }
};

template<typename T1> struct GenericLuaCaller{
     static T1 Call(lua_State *L,bool pop=true){
        return -1;
    };
    static T1 Empty;
};


template<typename T1> struct GenericLuaType{
     static bool Is(lua_State *L,int stack=-1){
        return false;
    };
};

template<>
    struct GenericLuaType<int>{
        static bool Is(lua_State *L,int stack=-1){
            return lua_isnumber(L,-1);
        };
};

template<>
    struct GenericLuaType<std::string>{
        static bool Is(lua_State *L,int stack=-1){
            return lua_isstring(L,-1);
        };
};

template<>
    struct GenericLuaType<float>{
        static bool Is(lua_State *L,int stack=-1){
            return lua_isnumber(L,-1);
        };
};

template<>
    struct GenericLuaType<bool>{
        static bool Is(lua_State *L,int stack=-1){
            return lua_isboolean(L,-1);
        };
};


template<>
    struct GenericLuaCaller<int> {
     static int Call(lua_State *L,bool pop=true){
        int n = 0;
        if (lua_isnil(L,-1)){
            Console::GetInstance().AddText("[LuaBase][Warning]Argument %d is nil",lua_gettop(L));
            n = -1;
        }else{
            n = lua_tonumber(L,-1);
        }
        if (pop)
            lua_pop(L,1);
        return n;
    };
    static int Empty;
};

template<>
    struct GenericLuaCaller<float> {
     static float Call(lua_State *L,bool pop=true){
        float n = 0;
        if (lua_isnil(L,-1)){
            Console::GetInstance().AddText("[LuaBase][Warning]Argument %d is nil",lua_gettop(L));
            n = -1;
        }else{
            n = lua_tonumber(L,-1);
        }
        if (pop)
            lua_pop(L,1);
        return n;
    };
    static float Empty;
};
template<>
    struct GenericLuaCaller<std::string> {
     static std::string Call(lua_State *L,bool pop=true){
        std::string n;
        if (lua_isnil(L,-1)){
            Console::GetInstance().AddText(utils::format("[LuaBase][Warning]Argument %d is nil",lua_gettop(L)));
            n = "nil";
        }else{
            n = lua_tostring(L,-1);
        }
        if (pop)
            lua_pop(L,1);
        return n;
    };
    static std::string Empty;
};

template<>
    struct GenericLuaCaller<bool> {
     static bool Call(lua_State *L,bool pop=true){
        bool n = false;
        if (lua_isnil(L,-1)){
            Console::GetInstance().AddText("[LuaBase][Warning]Argument %d is nil",lua_gettop(L));
            n = -1;
        }else{
            n = lua_toboolean(L,-1);
        }
        if (pop)
            lua_pop(L,1);
        return n;
    };
    static bool Empty;
};




template<typename T1> struct GenericLuaReturner{
     static void Ret(T1 vr,lua_State *L){
    };
};

template<> struct GenericLuaReturner<int>{
     static void Ret(int vr,lua_State *L){
         lua_pushnumber(L,vr);
    };
};

template<> struct GenericLuaReturner<float>{
     static void Ret(float vr,lua_State *L){
         lua_pushnumber(L,vr);
    };
};

template<> struct GenericLuaReturner<std::string>{
     static void Ret(std::string vr,lua_State *L){
         lua_pushstring(L,vr.c_str());
    };
};

template<> struct GenericLuaReturner<bool>{
     static void Ret(bool vr,lua_State *L){
         lua_pushboolean(L,vr);
    };
};

template<int N>
    struct readLuaValues {
        template<typename Tuple>
        static void Read(Tuple& tuple,lua_State *L) {
            typedef typename std::tuple_element<N-1, Tuple>::type ValueType;
            std::get<N-1>(tuple) = GenericLuaCaller<ValueType>::Call(L);
            readLuaValues<N-1>::Read(tuple,L);
        }
    };
template<>
    struct readLuaValues<0> {
        template<typename Tuple>
        static void Read(Tuple& tuple,lua_State *L) { }
};

template<typename Ret, typename F, typename... Args>
   Ret functionCaller(const F& f,lua_State *L, const Args&... args) {
    Ret ret = f(args...);
    return (Ret)ret;
}


template<int N, typename Ret> struct expander {
    template<typename ArgStructure,typename Function, typename... Args>
        static Ret expand(const ArgStructure& a,lua_State *L,const Function& f, const Args&... args) {
            return expander<N-1,Ret>::expand(a,L,f, std::get<N-1>(a), args...);
    }
};

template<typename Ret> struct expander <0,Ret> {
    template<typename ArgStructure, typename Function, typename... Args>
        static Ret expand(const ArgStructure& a,lua_State *L,const Function& f, const Args&... args) {
            return functionCaller<Ret>(f,L,  args...);
    }
};

class LuaCaller{
    public:
        static int Base(lua_State *L){
            LuaCFunctionLambda **v = (LuaCFunctionLambda **)lua_touserdata(L, lua_upvalueindex(1));
            (*(*v))(L);
            return 1;
        }
};

template<typename T1,typename ... Types> void RandomRegister(lua_State *L,std::string str,T1 func(Types ... args) ){
    static LuaCFunctionLambda f = [func,str](lua_State *L2) -> int {
        int argCount = sizeof...(Types);
        if (argCount > lua_gettop(L2)){
            Console::GetInstance().AddTextInfo(utils::format("Too few arguments on function %s. Expected %d got %d",str,lua_gettop(L2),argCount));
        }
        if (argCount < lua_gettop(L2)){
            Console::GetInstance().AddTextInfo(utils::format("Too much arguments on function %s. Expected %d got %d",str,lua_gettop(L2),argCount));
        }
        std::tuple<Types ...> ArgumentList;
        readLuaValues<sizeof...(Types)>::Read(ArgumentList,L2);
        T1 rData = expander<sizeof...(Types),T1>::expand(ArgumentList,L2,func);
        GenericLuaReturner<T1>::Ret(rData,L2);
        return 1;
    };
    LuaCFunctionLambda** baseF = static_cast<LuaCFunctionLambda**>(lua_newuserdata(L, sizeof(LuaCFunctionLambda) ));
    (*baseF) = &f;
    lua_pushcclosure(L, LuaCaller::Base,1);
    lua_setglobal(L, str.c_str());
};

template<typename T1,typename ... Types> void LambdaRegister(lua_State *L,std::string str,std::function<T1(Types ... args)> func){
    static LuaCFunctionLambda f = [func,str](lua_State *L2) -> int {
        int argCount = sizeof...(Types);
        if (argCount > lua_gettop(L2)){
            Console::GetInstance().AddTextInfo(utils::format("Too few arguments on function %s. Expected %d got %d",str,lua_gettop(L2),argCount));
        }
        if (argCount < lua_gettop(L2)){
            Console::GetInstance().AddTextInfo(utils::format("Too much arguments on function %s. Expected %d got %d",str,lua_gettop(L2),argCount));
        }
        std::tuple<Types ...> ArgumentList;
        readLuaValues<sizeof...(Types)>::Read(ArgumentList,L2);
        T1 rData = expander<sizeof...(Types),T1>::expand(ArgumentList,L2,func);
        GenericLuaReturner<T1>::Ret(rData,L2);
        return 1;
    };
    LuaCFunctionLambda** baseF = static_cast<LuaCFunctionLambda**>(lua_newuserdata(L, sizeof(LuaCFunctionLambda) ));
    (*baseF) = &f;
    lua_pushcclosure(L, LuaCaller::Base,1);
    lua_setglobal(L, str.c_str());
};

#endif // LUATOOLSBEH
#endif // DISABLE_LUAINTERFACE
