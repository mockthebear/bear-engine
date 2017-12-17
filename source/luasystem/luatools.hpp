#include "../settings/definitions.hpp"
#include "../engine/gamebase.hpp"
#include "../performance/console.hpp"
#include <iostream>
#include <functional>
#include <string.h>
#ifndef DISABLE_LUAINTERFACE


    #ifndef LUATOOLSBEH
    #define LUATOOLSBEH
    #include LUA_INCLUDE
#include "luatypewrapper.hpp"
#include "luacaller.hpp"

#define LUA_LAYER_UI 0



template<class T> class LuaReferenceCounter{
    public:
        static T* makeReference(T *obj){
            uint64_t ref = (uint64_t)obj;
            std::map<uint64_t,std::pair<uint64_t,bool>> &counters = Get();
            if (counters[ref].first == 0){
                counters[ref].second = false;
            }
            counters[ref].first++;
            return obj;
        }
        static T* makeReference(T &obj){
            T *retObj = new T(obj);
            std::map<uint64_t,std::pair<uint64_t,bool>> &counters = Get();
            uint64_t ref = (uint64_t)retObj;
            counters[ref].first = 1;
            counters[ref].second = true;
            return retObj;
        }
        static T* makeReference(const T obj){
            T *retObj = new T(obj);
            std::map<uint64_t,std::pair<uint64_t,bool>> &counters = Get();
            uint64_t ref = (uint64_t)retObj;
            counters[ref].first = 1;
            counters[ref].second = true;
            return retObj;
        }
        static void ClearData(){
            std::map<uint64_t,std::pair<uint64_t,bool>> &counters = Get();
            for (auto &it : counters){
                if (it.second.second){
                    T* ptr = (T*)it.first;
                    delete ptr;
                }
            }
        }
        static uint64_t ReleaseReference(T *obj,bool allowDelete=true){
            uint64_t ref = (uint64_t)obj;
            std::map<uint64_t,std::pair<uint64_t,bool>> &counters = Get();
            if (counters[ref].first == 0){
                counters[ref].first = 1;
            }
            counters[ref].first--;
            if (allowDelete && counters[ref].first == 0 && counters[ref].second){
                delete obj;
            }
            return counters[ref].first;
        }
    private:
        static std::map<uint64_t,std::pair<uint64_t,bool>> &Get(){
            static LuaReferenceCounter ref;
            return ref.internal_counter;
        }
        std::map<uint64_t,std::pair<uint64_t,bool>> internal_counter;
};


/**
    Used as an compile time check for how an value will be casted or just return a blank whether
    the two values are the same or not. And then specify some custom casting

    Cant use plain typecasts because some methods like std::string dont accept
    any cast between int and std::string
*/
template<typename ValueType> struct LuaTyper{
    static ValueType& GetTypeIfSame(ValueType aux,ValueType& karg){
        return karg;
    };
    template<typename K> static ValueType& GetTypeIfSame(ValueType aux,K& karg){
        Console::GetInstance().AddText("Mismatched types: Expected [%s] got [%d]",typeid(ValueType).name(),typeid(K).name());
        static ValueType emptyStaticValue;
        return emptyStaticValue;
    };

};
template<>
struct LuaTyper<std::string>{
    static std::string GetTypeIfSame(std::string aux,const char*& karg){
        return std::string(karg);
    };
};


/**
    Store the values from each type in an tuple. These values can be got from
    lua using GenericLuaGetter<Type>::Call or simply from defined optional values.
*/


template<int N>
    struct readLuaValues {
    template<typename Tuple> static void Read(Tuple& tuple,lua_State *L,int stackpos = -1) {
        typedef typename std::tuple_element<N-1, Tuple>::type ValueType;
        ValueType v = GenericLuaGetter<ValueType>::Call(L,stackpos);
        std::get<N-1>(tuple) = v;
        readLuaValues<N-1>::Read(tuple,L,stackpos);
    }

    template<typename Tuple,typename K,typename ... Opt> static void Read(Tuple& tuple,lua_State *L,int stackpos,K& head, Opt& ... tail) {
        typedef typename std::tuple_element<N-1, Tuple>::type ValueType;
        int argNecessary = lua_gettop(L)-1;
        if (N > argNecessary){
            ValueType v = LuaTyper<ValueType>::GetTypeIfSame(ValueType(),head);
            std::get<N-1>(tuple) = v;
            readLuaValues<N-1>::Read(tuple,L,stackpos, tail...);
        }else{
            ValueType v = GenericLuaGetter<ValueType>::Call(L,stackpos);
            std::get<N-1>(tuple) = v;
            readLuaValues<N-1>::Read(tuple,L,stackpos,head,tail...);
        }
    }

     template<typename Tuple,typename K> static void Read(Tuple& tuple,lua_State *L,int stackpos,K &headEnd) {
        typedef typename std::tuple_element<N-1, Tuple>::type ValueType;
        int argNecessary = lua_gettop(L)-1;
        if (N > argNecessary){
            ValueType v = headEnd;
            std::get<N-1>(tuple) = v;
            readLuaValues<N-1>::Read(tuple,L,stackpos);
        }else{
            ValueType v = GenericLuaGetter<ValueType>::Call(L,stackpos);
            //std::cout << "[2]Read arg from lua" << v << "\n";
            std::get<N-1>(tuple) = v;
            readLuaValues<N-1>::Read(tuple,L,stackpos);
        }
    }


};





template<>
    struct readLuaValues<0> {
        template<typename Tuple> static void Read(Tuple& tuple,lua_State *L,int stackpos=-1) { };
        template<typename Tuple,typename K> static void Read(Tuple& tuple,lua_State *L,int stackpos, K k) { };
        template<typename Tuple,typename K,typename ... Opt> static void Read(Tuple& tuple,lua_State *L,int stackpos,K k,Opt ... aux) { };
};


template<typename Ret> struct finalCaller {
    template<typename F, typename... Args>
       static Ret functionCaller(const F& f,lua_State *L, const Args&... args) {
       Ret ret = f(args...);
       return (Ret)ret;
    };
};

template<> struct finalCaller<void>{
    template<typename F, typename... Args>
       static void functionCaller(const F& f,lua_State *L, const Args&... args) {
       f(args...);
       return;
    };
};


/*
    Internally calling from an class
*/

template<typename Ret,typename Ctype, typename F, typename... Args> struct internal_caller{
       static Ret functionCallerClass(const F& f,lua_State *L, const Args&... args) {

       Ctype* part = LuaManager::GetSelf<Ctype>();
       if (!part){
            return Ret();
       }
       Ret ret = ((part)->*f)(args...);
       return (Ret)ret;
    };
};
/*
    Its void member
*/
template<typename Ctype, typename F, typename... Args> struct internal_caller<void,Ctype, F, Args...>{
       static void functionCallerClass(const F& f,lua_State *L, const Args&... args) {
       Ctype* part = LuaManager::GetSelf<Ctype>();
       if (!part){
            lua_pushstring(L,"[LUA]Could not call function because self reference is a nullpointer.");
            lua_error (L);
            return;
       }
       ( (part)->*f)(args...);
       return;
    };
};

template<int N,typename Ctype, typename Ret> struct expanderClass {
    template<typename ArgStructure,typename Function, typename... Args>
        static Ret expand(const ArgStructure& a,lua_State *L,const Function& f, const Args&... args) {
            return expanderClass<N-1,Ctype,Ret>::expand(a,L,f, std::get<N-1>(a), args...);
    }
};

template<typename Ctype,typename Ret> struct expanderClass <0,Ctype,Ret> {
    template<typename ArgStructure, typename Function, typename... Args>
        static Ret expand(const ArgStructure& a,lua_State *L,const Function& f, const Args&... args) {
            return internal_caller<Ret,Ctype,Function,Args...>::functionCallerClass(f,L,  args...);
    }
};


template<int N, typename Ret> struct expander {
    template<typename ArgStructure,typename Function, typename... Args>
        static Ret expand(const ArgStructure& a,lua_State *L,const Function& f, const Args&... args) {
            return expander<N-1,Ret>::expand(a,L,f, std::get<N-1>(a), args...);
    }
};



template<typename Ret> struct expander <0,Ret> {
    template<typename ArgStructure, typename Function, typename... Args>
        static Ret expand(const ArgStructure& a,lua_State *L,const Function& f, const Args&... args) {
            return finalCaller<Ret>::functionCaller(f,L,  args...);
    }
};
 struct pexpander {
    template<typename R,typename... Args> static int expand(lua_State *L,R r,const Args&... args) {
        GenericLuaReturner<R>::Ret(r,L);
        return 1 + pexpander::expand(L,args...);
    };
    static int expand(lua_State *L) {
        return 0;
    }
    template<typename R> static int expand(lua_State *L,R r) {
        GenericLuaReturner<R>::Ret(r,L);
        return 1;
    };
};


class LuaCaller{
    public:
        template <int N> static int BaseEmpty(lua_State *L){
            if (!L){
                return 0;
            }
            LuaCFunctionLambda **v = (LuaCFunctionLambda **)lua_touserdata(L, lua_upvalueindex(N));
            if (!v || !(*v)){
                Console::GetInstance().AddTextInfo(utils::format("[LuaCaller][LUA]could not call closure %d because null reference",v));
                lua_pushstring(L,"[LUA]could not call closure %d because null reference");
                lua_error (L);
                return 0;
            }
            (*(*v))(L);
            return 1;
        }

        template <int N> static int Base(lua_State *L){
            LuaCFunctionLambda **v = (LuaCFunctionLambda **)lua_touserdata(L, lua_upvalueindex(N));
            if (!v || !(*v)){
                Console::GetInstance().AddTextInfo(utils::format("[LuaCaller][LUA]could not call closure %d because null reference",v));
                lua_pushstring(L,"[LUA]could not call closure %d because null reference");
                lua_error (L);
                return 0;
            }
            int **self = LuaManager::GetSelfReference<int>();
            (*(*v))(L);
            if (!self){

                Console::GetInstance().AddTextInfo(utils::format("[LuaCaller][LUA]could not call function because of an deleted reference[addr %d] %d -> %s",v,N,LuaManager::lastCalled.c_str()));
            }
            return 1;
        }

        static void Startup(lua_State *L){
            if (!L){
                return;
            }
            lua_newtable(L);
            lua_setglobal(L, "__REFS");
            char baseMaker[] =  "function New(state,objtype,...)\n"
                                "   local obj = objtype(...)\n"
                                "   __REFS[state][obj.id] = obj\n"
                                "   return obj\n"
                                "end\n"
                                "function CallFromField(state,index,str,...)\n"
                                "    if not __REFS[state] then print('No such state '..state..' inserted int the call of '..str); return false; end \n"
                                "    if __REFS[state][index] then\n"
                                "        if __REFS[state][index][str] ~= nil then\n"
                                "            return __REFS[state][index][str](__REFS[state][index],...)\n"
                                "        end\n"
                                "    end\n"
                                "    return false\n"
                                "end\n"
                                "function CallOnField(state,index,str,...)\n"
                                "    if not __REFS[state] then print('No such state '..state..' inserted int the call of '..str); return false; end \n"
                                "    if __REFS[state][index] then\n"
                                "        if _G[str] and type(_G[str]) == \"function\" then\n"
                                "            _G[str](__REFS[state][index],...)\n"
                                "        end\n"
                                "    end\n"
                                "    return false\n"
                                "end\n"
                                "function MakeState(state,masterState)\n"
                                "   __REFS[state] = {}\n"
                                "   if __REFS[masterState] and __REFS[masterState][state] then\n"
                                "   __REFS[state][state] = __REFS[masterState][state];\n"
                                "   end\n"
                                "end\n"
                                "function ClearInstances(state)\n"
                                "   __REFS[state] = {}\n"
                                "   collectgarbage()\n"
                                "end\n"
                                "__REFS[0] = {}\n";
            luaL_loadstring(L, baseMaker);
            lua_pcall(L, 0, LUA_MULTRET, 0);

        };
        template <typename LState> static bool CallClear(lua_State *L, LState *state){
            if (!L){
                return false;
            }
            lua_getglobal(L, "ClearInstances");
            if(!lua_isfunction(L, -1) ){
                return false;
            }
            lua_pushinteger(L, uint64_t(state));
            return LuaManager::Pcall(1);;
        }


        template <typename LState> static bool StartupState(lua_State *L,LState *state,DefinedState *s2){
            if (!L){
                return false;
            }
            lua_getglobal(L, "MakeState");
            if(!lua_isfunction(L, -1) ){
                return false;
            }
            //
            lua_pushinteger(L, uint64_t(state));
            lua_pushinteger(L, uint64_t(s2));

            return LuaManager::Pcall(2);
        }
        static bool LoadFile(lua_State *L,std::string name){
            if (!L){
                return false;
            }
            if ( luaL_loadfile(L, name.c_str()) != 0 ) {
                Console::GetInstance().AddTextInfo(utils::format("[Lua error]: %s",lua_tostring(L, -1)));
                lua_pop(L, 1);
                return false;
            }
            return true;
        }
        template <typename ... Types> static bool Pcall(lua_State *L,Types ... args){
            if (!L){
                return false;
            }
            pexpander::expand(L,args...);
            return LuaManager::Pcall(sizeof...(Types), LUA_MULTRET);
        }


        template <typename ... Types> static bool CallGlobalField(lua_State *L,std::string field,Types ... args){
            if (!L){
                return false;
            }
            lua_getglobal(L, field.c_str());
            if(!lua_isfunction(L, -1) ){
                lua_pop(L, 1);
                return false;
            }
            LuaManager::lastCalled = field;
            pexpander::expand(L,args...);
            LuaManager::Pcall(sizeof...(Types), 1);
            bool ret = lua_toboolean(L,-1);
            lua_pop(L,1);
            return ret;
        }


        template <typename Obj,typename ... Types> static bool CallField(lua_State *L,Obj *obj,std::string field,Types ... args){
            if (!L){
                return false;
            }
            lua_getglobal(L, "CallOnField");
            if(!lua_isfunction(L, -1) ){
                return false;
            }
            LuaManager::lastCalled = field;
            uint64_t index = uint64_t(obj);
            lua_pushinteger(L, uint64_t(&Game::GetCurrentState()));
            lua_pushinteger(L, index);
            lua_pushstring(L, field.c_str());
            pexpander::expand(L,args...);

            LuaManager::Pcall(3 + (sizeof...(Types)), 1, 0);

            if (lua_isnil(L, -1)){
                return false;
            }
            bool ret = lua_toboolean(L,-1);
            lua_pop(L,1);
            return ret;
        }

        template <typename Obj,typename ... Types> static bool CallOtherField(lua_State *L,uint64_t old,Obj *obj,std::string field,Types ... args){
            if (!L){
                return false;
            }
            lua_getglobal(L, "CallFromField");
            if(!lua_isfunction(L, -1) ){
                return false;
            }
            LuaManager::lastCalled = field;
            uint64_t index = uint64_t(obj);
            lua_pushinteger(L, old);
            lua_pushinteger(L, index);

            lua_pushstring(L, field.c_str());
            pexpander::expand(L,args...);

            LuaManager::Pcall(3 + (sizeof...(Types)), 1, 0);

            if (lua_isnil(L, -1)){
                return false;
            }
            bool ret = lua_toboolean(L,-1);
            lua_pop(L,1);
            return ret;
        }

        template <typename Obj,typename ... Types> static bool CallSelfField(lua_State *L,Obj *obj,std::string field,Types ... args){
            if (!L){
                return false;
            }
            lua_getglobal(L, "CallFromField");

            if(!lua_isfunction(L, -1) ){
                lua_pop(L,1);
                return false;
            }
            LuaManager::lastCalled = field;
            uint64_t index = uint64_t(obj);
            lua_pushinteger(L, uint64_t(&Game::GetCurrentState()));
            lua_pushinteger(L, index);

            lua_pushstring(L, field.c_str());
            pexpander::expand(L,args...);

            LuaManager::Pcall(3 + (sizeof...(Types)), 1, 0);

            if (lua_isnil(L, -1)){
                return false;
            }
            bool ret = lua_toboolean(L,-1);
            lua_pop(L,lua_gettop(L)-1);

            return ret;
        }

        static void DeleteField(lua_State *L,uint64_t obj){
            if (!L){
                return;
            }
            lua_getglobal(L, "__REFS"); // insert
            lua_pushnumber(L, uint64_t(&Game::GetCurrentState()));
            lua_gettable(L, -2 );
            lua_pushinteger(L, (obj));
            lua_pushnil(L);
            lua_settable(L, -3);
        };
};


template<typename T1,typename ... Types> void FunctionRegister(lua_State *L,std::string str,T1 func(Types ... args) ){

    LuaCFunctionLambda f = [func,str](lua_State *L2) -> int {
        LuaManager::lastCalled = str;
        int argCount = sizeof...(Types);
        if (argCount > lua_gettop(L2)){
            Console::GetInstance().AddTextInfo(utils::format("[LUA][1]Too much arguments on function %s. Expected %d got %d",str,argCount,lua_gettop(L2)));
        }
        if (argCount < lua_gettop(L2)){
            Console::GetInstance().AddTextInfo(utils::format("[LUA][1]Too few arguments on function %s. Expected %d got %d",str,argCount,lua_gettop(L2)));
        }
        std::tuple<Types ...> ArgumentList;
        readLuaValues<sizeof...(Types)>::Read(ArgumentList,L2,-1);
        T1 rData = expander<sizeof...(Types),T1>::expand(ArgumentList,L2,func);
        GenericLuaReturner<T1>::Ret(rData,L2);
        return 1;
    };
    LuaCFunctionLambda** baseF = static_cast<LuaCFunctionLambda**>(lua_newuserdata(L, sizeof(LuaCFunctionLambda) ));
    (*baseF) = new LuaCFunctionLambda(f);
    LuaManager::AddReference((*baseF));
    lua_pushcclosure(L, LuaCaller::BaseEmpty<1>,1);
    lua_setglobal(L, str.c_str());
};





template<typename T1,typename ClassObj,typename ... Types> struct internal_register{
     /*static void LambdaRegisterStack(lua_State *L,std::string str,int stackPos,T1 (ClassObj::*func)(Types ... args) ){
        LuaCFunctionLambda f = [func,str](lua_State *L2) -> int {
            LuaManager::lastCalled = str;
            int argCount = sizeof...(Types);
            int argNecessary = lua_gettop(L2)-2;
            if (argCount < argNecessary){
                Console::GetInstance().AddTextInfo(utils::format("[LUA][3]Too few arguments on function %s. Expected %d got %d",str,argCount,argNecessary));
            }
            if (argCount > argNecessary){
                Console::GetInstance().AddTextInfo(utils::format("[LUA][3]Too much arguments on function %s. Expected %d got %d",str,argCount,argNecessary));
            }
            std::tuple<Types ...> ArgumentList;
            if (sizeof...(Types) > 0)
                lua_pop(L2, 1);
            readLuaValues<sizeof...(Types)>::Read(ArgumentList,L2,-1);
            T1 rData = expanderClass<sizeof...(Types),ClassObj,T1>::expand(ArgumentList,L2,func);
            GenericLuaReturner<T1>::Ret(rData,L2);
            return 1;
        };
        LuaCFunctionLambda** baseF = static_cast<LuaCFunctionLambda**>(lua_newuserdata(L, sizeof(LuaCFunctionLambda) ));
        (*baseF) = new LuaCFunctionLambda(f);
        LuaManager::AddReference((*baseF));
        lua_pushcclosure(L, LuaCaller::Base<1>,1);
        lua_setfield(L, stackPos,  str.c_str());

    };*/
    template <typename ... Opt> static void LambdaRegisterStackOpt(lua_State *L,std::string str,int stackPos,T1 (ClassObj::*func)(Types ... args),Opt ... optionalArgs ){
        LuaCFunctionLambda f = [func,str,optionalArgs...](lua_State *L2) -> int {
            LuaManager::lastCalled = str;
            int argCount = sizeof...(Types);
            int argNecessary = std::max(lua_gettop(L2)-2, int(sizeof...(Opt)));
            if (argCount < argNecessary){
                Console::GetInstance().AddTextInfo(utils::format("[LUA][2]Too few arguments on function %s. Expected %d got %d",str,argCount,argNecessary));
            }
            if (argCount > argNecessary){
                if (argCount > lua_gettop(L2)-2 + int(sizeof...(Opt)) )
                    Console::GetInstance().AddTextInfo(utils::format("[LUA][2]Too much arguments on function %s. Expected %d got %d and of those only %d are optional",str,argCount,argNecessary,int(sizeof...(Opt))));
            }
            std::tuple<Types ...> ArgumentList;
            if (sizeof...(Types) > 0)
                lua_pop(L2, 1);
            readLuaValues<sizeof...(Types)>::Read(ArgumentList,L2,-1,optionalArgs...);
            T1 rData = expanderClass<sizeof...(Types),ClassObj,T1>::expand(ArgumentList,L2,func);
            GenericLuaReturner<T1>::Ret(rData,L2);
            return 1;
        };
        LuaCFunctionLambda** baseF = static_cast<LuaCFunctionLambda**>(lua_newuserdata(L, sizeof(LuaCFunctionLambda) ));
        (*baseF) = new LuaCFunctionLambda(f);
        LuaManager::AddReference((*baseF));
        lua_pushcclosure(L, LuaCaller::Base<1>,1);
        lua_setfield(L, stackPos,  str.c_str());

    };
};

template<typename ClassObj,typename ... Types> struct internal_register<void,ClassObj,Types...>{
     /*static void LambdaRegisterStack(lua_State *L,std::string str,int stackPos,void (ClassObj::*func)(Types ... args) ){
        LuaCFunctionLambda f = [func,str](lua_State *L2) -> int {
            LuaManager::lastCalled = str;
            int argCount = sizeof...(Types);
            int argNecessary = lua_gettop(L2)-2;
            if (argCount < argNecessary){
                Console::GetInstance().AddTextInfo(utils::format("[LUA][3]Too few arguments on function %s. Expected %d got %d",str,argCount,argNecessary));
            }
            if (argCount > argNecessary){
                Console::GetInstance().AddTextInfo(utils::format("[LUA][3]Too much arguments on function %s. Expected %d got %d",str,argCount,argNecessary));
            }
            std::tuple<Types ...> ArgumentList;
            if (sizeof...(Types) > 0)
                lua_pop(L2, 1);
            readLuaValues<sizeof...(Types)>::Read(ArgumentList,L2,-1);
            expanderClass<sizeof...(Types),ClassObj,void>::expand(ArgumentList,L2,func);
            GenericLuaReturner<void>::Ret(0,L2);
            return 1;
        };
        LuaCFunctionLambda** baseF = static_cast<LuaCFunctionLambda**>(lua_newuserdata(L, sizeof(LuaCFunctionLambda) ));
        (*baseF) = new LuaCFunctionLambda(f);
        LuaManager::AddReference((*baseF));
        lua_pushcclosure(L, LuaCaller::Base<1>,1);
        lua_setfield(L, stackPos,  str.c_str());

    };*/
    template <typename ... Opt> static void LambdaRegisterStackOpt(lua_State *L,std::string str,int stackPos,void (ClassObj::*func)(Types ... args),Opt ... optionalArgs ){
        //#if defined(__GNUC__) || defined(__GNUG__)
        //LuaCFunctionLambda f = [func,str](lua_State *L2) -> int {
        //#else
        LuaCFunctionLambda f = [func,str,optionalArgs...](lua_State *L2) -> int {
        //#endif // defined
            LuaManager::lastCalled = str;
            int argCount = sizeof...(Types);
            int argNecessary = std::max(lua_gettop(L2)-2, int(sizeof...(Opt)));
            if (argCount < argNecessary){
                Console::GetInstance().AddTextInfo(utils::format("[LUA][1]Too much arguments on function %s. Expected %d got %d",str,argCount,argNecessary));
            }
            if (argCount > argNecessary){
                if (argCount > lua_gettop(L2)-2 + int(sizeof...(Opt)) )
                    Console::GetInstance().AddTextInfo(utils::format("[LUA][1]Too few arguments on function %s. Expected %d got %d and opt %d",str,argCount,argNecessary,int(sizeof...(Opt))));
            }
            std::tuple<Types ...> ArgumentList;
            if (sizeof...(Types) > 0)
                lua_pop(L2, 1);

            //#if defined(__GNUC__) || defined(__GNUG__)
            //readLuaValues<sizeof...(Types)>::Read(ArgumentList,L2,-1);
            //#else
            readLuaValues<sizeof...(Types)>::Read(ArgumentList,L2,-1,optionalArgs...);
            //#endif // defined
            expanderClass<sizeof...(Types),ClassObj,void>::expand(ArgumentList,L2,func);
            GenericLuaReturner<void>::Ret(0,L2);

            return 1;
        };
        LuaCFunctionLambda** baseF = static_cast<LuaCFunctionLambda**>(lua_newuserdata(L, sizeof(LuaCFunctionLambda) ));
        (*baseF) = new LuaCFunctionLambda(f);
        LuaManager::AddReference((*baseF));
        lua_pushcclosure(L, LuaCaller::Base<1>,1);
        lua_setfield(L, stackPos,  str.c_str());

    };
};


template<typename T1,typename ... Types> void LambdaClassRegister(lua_State *L,std::string str,int stackPos,std::function<T1(Types ... args)> func){
    LuaCFunctionLambda f = [func,str](lua_State *L2) -> int {
        LuaManager::lastCalled = str;
        int argCount = sizeof...(Types);
        if (argCount > lua_gettop(L2)){
            Console::GetInstance().AddTextInfo(utils::format("[LUA][4]Too few arguments on function %s. Expected %d got %d",str,argCount,lua_gettop(L2)));
        }
        if (argCount < lua_gettop(L2)-2){
            Console::GetInstance().AddTextInfo(utils::format("[LUA][4]Too much arguments on function %s. Expected %d got %d",str,argCount,lua_gettop(L2)-2));
        }
        std::tuple<Types ...> ArgumentList;
        readLuaValues<sizeof...(Types)>::Read(ArgumentList,L2);
        T1 rData = expander<sizeof...(Types),T1>::expand(ArgumentList,L2,func);
        GenericLuaReturner<T1>::Ret(rData,L2);
        return 1;
    };
    LuaCFunctionLambda** baseF = static_cast<LuaCFunctionLambda**>(lua_newuserdata(L, sizeof(LuaCFunctionLambda) ));
    (*baseF) = new LuaCFunctionLambda(f);
    LuaManager::AddReference((*baseF));
    lua_pushcclosure(L, LuaCaller::BaseEmpty<1>,1);
    lua_setfield(L, stackPos,  str.c_str());
}

template<typename ... Types> void LambdaClassRegister(lua_State *L,std::string str,int stackPos,std::function<void(Types ... args)> func){
    LuaCFunctionLambda f = [func,str](lua_State *L2) -> int {
        LuaManager::lastCalled = str;
        int argCount = sizeof...(Types);
        if (argCount > lua_gettop(L2)){
            Console::GetInstance().AddTextInfo(utils::format("[LUA][5]Too few arguments on function %s. Expected %d got %d",str,argCount,lua_gettop(L2)));
        }
        if (argCount < lua_gettop(L2)-2){
            Console::GetInstance().AddTextInfo(utils::format("[LUA][5]Too much arguments on function %s. Expected %d got %d",str,argCount,lua_gettop(L2)-2));
        }
        std::tuple<Types ...> ArgumentList;
        readLuaValues<sizeof...(Types)>::Read(ArgumentList,L2);
        expander<sizeof...(Types),void>::expand(ArgumentList,L2,func);
        //GenericLuaReturner<T1>::Ret(rData,L2);
        return 0;
    };
    LuaCFunctionLambda** baseF = static_cast<LuaCFunctionLambda**>(lua_newuserdata(L, sizeof(LuaCFunctionLambda) ));
    (*baseF) = new LuaCFunctionLambda(f);
    LuaManager::AddReference((*baseF));
    lua_pushcclosure(L, LuaCaller::BaseEmpty<1>,1);
    lua_setfield(L, stackPos,  str.c_str());
}




template<typename T1,typename ... Types> void LambdaRegister(lua_State *L,std::string str,std::function<T1(Types ... args)> func){
    LuaCFunctionLambda f = [func,str](lua_State *L2) -> int {
        LuaManager::lastCalled = str;
        int argCount = sizeof...(Types);
        if (argCount > lua_gettop(L2)){
            Console::GetInstance().AddTextInfo(utils::format("[LUA][6]Too few arguments on function %s. Expected %d got %d",str,argCount,lua_gettop(L2)));
        }
        if (argCount < lua_gettop(L2)){
            Console::GetInstance().AddTextInfo(utils::format("[LUA][6]Too much arguments on function %s. Expected %d got %d",str,argCount,lua_gettop(L2)));
        }
        std::tuple<Types ...> ArgumentList;
        readLuaValues<sizeof...(Types)>::Read(ArgumentList,L2);
        T1 rData = expander<sizeof...(Types),T1>::expand(ArgumentList,L2,func);
        GenericLuaReturner<T1>::Ret(rData,L2);
        return 1;
    };
    LuaCFunctionLambda** baseF = static_cast<LuaCFunctionLambda**>(lua_newuserdata(L, sizeof(LuaCFunctionLambda) ));
    (*baseF) = new LuaCFunctionLambda(f);
    LuaManager::AddReference((*baseF));
    lua_pushcclosure(L, LuaCaller::BaseEmpty<1>,1);
    lua_setglobal(L, str.c_str());
};

class MasterGC{
    public:
    template <typename T> static int Close(lua_State *L){
        lua_getfield(L, 1, "__self");
        T** part = (T**)lua_touserdata(L, -1);
        if (part){
            LuaReferenceCounter<T>::ReleaseReference(*part);
            (*part) = nullptr;
            //delete part;
        }
        return 0;
    }
    template <typename T> static int CloseAndClear(lua_State *L){
        lua_getfield(L, 1, "__self");

        T** part = (T**)lua_touserdata(L, -1);
        if (part){
            LuaCaller::CallSelfField(LuaManager::L,(*part),"Close");
            LuaCaller::DeleteField(L,(*part));
            LuaReferenceCounter<T>::ReleaseReference(*part);
            //(*part)->Kill();
            /*
                Erase reference
            */
            (*part) = nullptr;
            //delete part;
        }
        return 0;
    }
    template <typename T> static int Destroy(lua_State *L){
        lua_getfield(L, 1, "__self");
        T** part = (T**)lua_touserdata(L, -1);
        if (part && *part){
            LuaReferenceCounter<T>::ReleaseReference(*part);
            if (LuaManager::IsDebug)
                Console::GetInstance().AddTextInfo(utils::format("[LUA][Destroy]PTR: %d",part));
            lua_getglobal(L, "__REFS"); // insert
            lua_pushnumber(L, uint64_t(&Game::GetCurrentState()));
            if (!lua_isnil(L,-1)){
                lua_gettable(L, -2 );
                lua_pushnumber(L,(uint64_t(*part)));
                lua_pushnil(L);
                lua_settable(L,-3);
            }else{
                lua_pop(L,1);
            }
        }
        return 1;
    }
    template <typename T> static int GC(lua_State *L){
        T** part = (T**)lua_touserdata(LuaManager::L, -1);
        if (LuaManager::IsDebug)
            Console::GetInstance().AddTextInfo(utils::format("[LUA][MasterGC]PTR: %d",part));
        if (part && *part){
            LuaReferenceCounter<T>::ReleaseReference(*part);
            (*part) = nullptr;
            //delete part;
            return 1;
        }
        return 0;

    }
};


template<typename T1,typename ObjT> struct TypeObserver{
    /**
        Modify field
    */
    static int Newindex(lua_State *L){
        std::string field = lua_tostring(L,-2);
        ObjT         data = GenericLuaGetter<ObjT>::Call(L,-1);
        std::map<std::string,ObjT T1::*> &fieldData = TypeObserver<T1,ObjT>::getAddr();
        //std::map<std::string,std::string> &fieldName = TypeObserver<T1>::getAddrNames();
        T1 *self = LuaManager::GetSelf<T1>();
        if (!self){
            Console::GetInstance().AddTextInfo(utils::format("[LUA]Setting field %s where its userdata is nullptr.",field));
            return 0;
        }
        if (fieldData[field]){
            ObjT T1::* fieldptr = fieldData[field];
            self->*fieldptr = data;
        }
        return 0;
    }
    /**
        Request field
    */
    static int Index(lua_State *L){
        std::string field = lua_tostring(L,-1);
        std::map<std::string,ObjT T1::*> &fieldData = TypeObserver<T1,ObjT>::getAddr();
        T1 *self = LuaManager::GetSelf<T1>();
        if (!self){
            Console::GetInstance().AddTextInfo(utils::format("[LUA]Requesting field %s where its userdata is nullptr.",field));
			return 0;
        }
        if (fieldData[field]){
            ObjT T1::* fieldptr = fieldData[field];
            GenericLuaReturner<ObjT>::Ret(self->*fieldptr,L);
            return 1;
        }
        return 0;
    };
    template<typename mType> static int RegisterMethod(lua_State *L,std::string luaname,mType T1::*fieldaddr){
        std::map<std::string,ObjT T1::*> &fieldData = TypeObserver<T1,ObjT>::getAddr();
       // std::map<std::string,std::string> &fieldName = TypeObserver<T1>::getAddrNames();
        fieldData[luaname] = (ObjT T1::*)fieldaddr;
        //fieldName[luaname] = typeid(mType).name();
        return 1;
    }
    static bool HasField(std::string fieldName){
        std::map<std::string,ObjT T1::*> &fieldData = TypeObserver<T1,ObjT>::getAddr();
        return fieldData[fieldName] != nullptr;
    }
    std::map<std::string,ObjT T1::*> addr;
    static std::map<std::string,std::string> &getAddrNames(){
        static TypeObserver<T1,ObjT> st;
        return st.getAddrNames();
    }
    static std::map<std::string,ObjT T1::*> &getAddr(){
        static TypeObserver<T1,ObjT> st;
        return st.addr;
    }
};



/*
    Used to call the proper newindex/index or so of the given and correct type
*/
template<typename T1> struct IndexerHelper{
    static int Newindex(lua_State *L){
        std::string field = lua_tostring(L,-2);
        if (TypeObserver<T1,int>::HasField(field)){
            return TypeObserver<T1,int>::Newindex(L);

        }else if (TypeObserver<T1,Rect>::HasField(field)){
            return TypeObserver<T1,Rect>::Newindex(L);

        }else if (TypeObserver<T1,Point>::HasField(field)){
            return TypeObserver<T1,Point>::Newindex(L);

        }else if (TypeObserver<T1,float>::HasField(field)){
            return TypeObserver<T1,float>::Newindex(L);

        }else if (TypeObserver<T1,std::string>::HasField(field)){
            return TypeObserver<T1,std::string>::Newindex(L);

        }else if (TypeObserver<T1,bool>::HasField(field)){
            return TypeObserver<T1,bool>::Newindex(L);
        }
        return 0;
    };
    static int Index(lua_State *L){
        std::string field = lua_tostring(L,-1);
        if (TypeObserver<T1,int>::HasField(field)){
            return TypeObserver<T1,int>::Index(L);

        }else if (TypeObserver<T1,Rect>::HasField(field)){
            return TypeObserver<T1,Rect>::Index(L);

        }else if (TypeObserver<T1,Point>::HasField(field)){
            return TypeObserver<T1,Point>::Index(L);

        }else if (TypeObserver<T1,float>::HasField(field)){
            return TypeObserver<T1,float>::Index(L);

        }else if (TypeObserver<T1,std::string>::HasField(field)){
            return TypeObserver<T1,std::string>::Index(L);

        }else if (TypeObserver<T1,bool>::HasField(field)){
            return TypeObserver<T1,bool>::Index(L);
        }
        return 0;
    };
};


struct GlobalMethodRegister{;
    static void RegisterGlobalTable(lua_State *L,std::string name){
        lua_newtable(L);
        lua_setglobal(L, name.c_str());
    }
    template<typename RetType,typename ... Types> static void RegisterGlobalTableMethod(lua_State *L,std::string name,std::string methodName,std::function<RetType(Types ... args)> func){
        lua_getglobal(L, name.c_str());
        if (lua_isnil(L,-1)){

            Console::GetInstance().AddTextInfo(utils::format("[LUA]Requesting global table %s is nil.",name.c_str()));
            lua_getfield(L, 1, "debug");
            if (!lua_istable(L, -1)) {
                lua_pop(L, 1);
                return;
            }
            lua_getfield(L, -1, "traceback");
            if (!lua_isfunction(L, -1)) {
                lua_pop(L, 2);
                return;
            }
            lua_pushvalue(L, 1);  /* pass error message */
            lua_pushinteger(L, 2);  /* skip this function and traceback */
            lua_call(L, 2, 1);  /* call debug.traceback */
            Console::GetInstance().AddTextInfo(utils::format("Trace: %s",lua_tostring(L, -1)));
            return;
        }
        LambdaClassRegister(L,methodName,-2,func);
        lua_pop(L, 1);
    }
};




template<typename T1> struct ClassRegister{
     static void RegisterObject(lua_State *L,std::string name, T1 *obj_reference,bool Force=true){
        if (!obj_reference){
            lua_pushstring(L,"[LUA] on RegisterObject, null reference.");
            lua_error (L);
        }
        T1 *obj = LuaReferenceCounter<T1>::makeReference(obj_reference);

        //
        if (Force){
            lua_getglobal(L, name.c_str());
            luaL_checktype(L, 1, LUA_TTABLE);
        }
        lua_getglobal(L, "__REFS"); // insert
        lua_pushnumber(L, uint64_t(&Game::GetCurrentState()));
        lua_gettable(L, -2 );
        lua_pushnumber(L,(uint64_t)obj);




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

        lua_settable(L,-3);

        lua_getglobal(L, "__REFS"); // request to return
        lua_pushnumber(L, uint64_t(&Game::GetCurrentState()));
        lua_gettable(L, -2 );
        lua_pushnumber(L,(uint64_t)obj);
        lua_gettable(L, -2);


     };


     static void RegisterClassOutside(lua_State *L,std::string name,
		 std::function<T1*(lua_State*)> makerF = std::function<T1*(lua_State*)>(),
                                      LuaCFunctionLambda *gc_func = nullptr){

        lua_newtable(L);
        lua_pushvalue(L, -1);
        lua_setglobal(L, name.c_str());
        int methods = lua_gettop(L);
        lua_newtable(L);
        int methodsTable = lua_gettop(L);

        static LuaCFunctionLambda Flambb = [name,makerF](lua_State* Ls) -> int{
			T1 *obj;
			if (makerF) {
				obj = makerF(Ls);
			}else{
				obj = new T1();
			}
			if (!obj){
                return 0;
			}
			RegisterObject(Ls,name,obj,false);
            return 1;
        };

        LuaManager::eraseLambdas.emplace_back([](){
               LuaReferenceCounter<T1>::ClearData();
        });
        LuaCFunctionLambda** baseF = static_cast<LuaCFunctionLambda**>(lua_newuserdata(L, sizeof(LuaCFunctionLambda) ));
        (*baseF) = &Flambb;
        lua_pushcclosure(L, LuaCaller::BaseEmpty<1>,1);
        lua_setfield(L, methodsTable, "__call");
        lua_setmetatable(L, methods);
        luaL_newmetatable(L, name.c_str());
        int metatable = lua_gettop(L);
        lua_pushvalue(L, methods);
        lua_setfield(L, metatable, "__metatable");
        lua_pushvalue(L, methods);
        lua_setfield(L, metatable, "__index");
        lua_pop(L, 2);
        lua_getglobal(L, name.c_str());
        if (gc_func && *gc_func){
            baseF = static_cast<LuaCFunctionLambda**>(lua_newuserdata(L, sizeof(LuaCFunctionLambda) ));
            (*baseF) = &(*gc_func);
            lua_pushcclosure(L, LuaCaller::Base<1>,1);
        }else{
            lua_pushcfunction(L, MasterGC::GC<T1>);
        }
        lua_setfield(L, -2, "__gc");
        lua_pop(L, 1);
        //Destroy

        lua_getglobal(L, name.c_str());
        lua_pushcfunction(L, MasterGC::Destroy<T1>);
        lua_setfield(L, -2,  "destroy");
        lua_pop(L, 1);
    };


    static void RegisterInheritedClass(lua_State *L,std::string name,
		 std::function<T1*(lua_State*)> makerF = std::function<T1*(lua_State*)>(),
                                      LuaCFunctionLambda *gc_func = nullptr){
        RegisterClassOutside(L,name,makerF,gc_func );

        ClassRegister<T1>::RegisterClassMethod(LuaManager::L,name.c_str(),"GetX"        ,&GameObject::GetX);
        ClassRegister<T1>::RegisterClassMethod(LuaManager::L,name.c_str(),"SetX"        ,&GameObject::SetX);
        ClassRegister<T1>::RegisterClassMethod(LuaManager::L,name.c_str(),"Is"          ,&GameObject::Is);
        ClassRegister<T1>::RegisterClassMethod(LuaManager::L,name.c_str(),"GetMyRef"    ,&GameObject::GetMyRef);
        ClassRegister<T1>::RegisterClassMethod(LuaManager::L,name.c_str(),"SetX"        ,&GameObject::SetX);
        ClassRegister<T1>::RegisterClassMethod(LuaManager::L,name.c_str(),"SetY"        ,&GameObject::SetY);
        ClassRegister<T1>::RegisterClassMethod(LuaManager::L,name.c_str(),"GetY"        ,&GameObject::GetY);
        ClassRegister<T1>::RegisterClassMethod(LuaManager::L,name.c_str(),"GetX"        ,&GameObject::GetX);
        ClassRegister<T1>::RegisterClassMethod(LuaManager::L,name.c_str(),"SetWidth"    ,&GameObject::SetWidth);
        ClassRegister<T1>::RegisterClassMethod(LuaManager::L,name.c_str(),"GetWidth"    ,&GameObject::GetWidth);
        ClassRegister<T1>::RegisterClassMethod(LuaManager::L,name.c_str(),"SetHeight"   ,&GameObject::SetHeight);
        ClassRegister<T1>::RegisterClassMethod(LuaManager::L,name.c_str(),"GetHeight"   ,&GameObject::GetHeight);
        ClassRegister<T1>::RegisterClassMethod(LuaManager::L,name.c_str(),"SetBox"      ,&GameObject::SetBox);
        ClassRegister<T1>::RegisterClassMethod(LuaManager::L,name.c_str(),"GetBox"      ,&GameObject::GetBox);
        ClassRegister<T1>::RegisterClassMethod(LuaManager::L,name.c_str(),"GetBox"      ,&GameObject::GetBox);
        ClassRegister<T1>::RegisterClassMethod(LuaManager::L,name.c_str(),"GetSolid"    ,&GameObject::GetSolid);
        ClassRegister<T1>::RegisterClassMethod(LuaManager::L,name.c_str(),"SetSolid"    ,&GameObject::SetSolid);
        ClassRegister<T1>::RegisterClassMethod(LuaManager::L,name.c_str(),"Kill"        ,&GameObject::Kill);

        ClassRegister<T1>::RegisterClassMethod(LuaManager::L,name.c_str(),"hasPerspective"        ,&GameObject::hasPerspective);
        ClassRegister<T1>::RegisterClassMethod(LuaManager::L,name.c_str(),"canForceRender"        ,&GameObject::canForceRender);
        ClassRegister<T1>::RegisterClassMethod(LuaManager::L,name.c_str(),"IsHash"        ,&GameObject::IsHash);
        ClassRegister<T1>::RegisterClassMethod(LuaManager::L,name.c_str(),"GetHash"        ,&GameObject::GetHash);





    };

    static void RegisterClassVirtual(lua_State *L,std::string name,
		 std::function<T1*(lua_State*)> makerF, LuaCFunctionLambda *gc_func){

        lua_newtable(L);
        lua_pushvalue(L, -1);
        lua_setglobal(L, name.c_str());
        int methods = lua_gettop(L);
        lua_newtable(L);
        int methodsTable = lua_gettop(L);

        static LuaCFunctionLambda Flambb = [name,makerF](lua_State* Ls) -> int{
			T1 *obj = nullptr;
			if (makerF) {
				obj = makerF(Ls);
			}
			RegisterObject(Ls,name,obj,false);
            return 1;
        };

        LuaCFunctionLambda** baseF = static_cast<LuaCFunctionLambda**>(lua_newuserdata(L, sizeof(LuaCFunctionLambda) ));
        (*baseF) = &Flambb;
        lua_pushcclosure(L, LuaCaller::BaseEmpty<1>,1);
        lua_setfield(L, methodsTable, "__call");
        lua_setmetatable(L, methods);
        luaL_newmetatable(L, name.c_str());
        int metatable = lua_gettop(L);
        lua_pushvalue(L, methods);
        lua_setfield(L, metatable, "__metatable");
        lua_pushvalue(L, methods);
        lua_setfield(L, metatable, "__index");
        lua_pop(L, 2);
    };


    template<typename RetType,typename ... Types> static void RegisterClassLambdaMethod(lua_State *L,std::string name,std::string methodName,std::function<RetType(Types ... args)> func){
        lua_getglobal(L, name.c_str());
        LambdaClassRegister(L,methodName,-2,func);
        lua_pop(L, 1);
    }


    template<typename RetType,typename ClassObj,typename ... Types> static void RegisterClassMethod(lua_State *L,std::string name,std::string methodName,RetType (ClassObj::*func)(Types ... args)){
        lua_getglobal(L, name.c_str());
        int top = lua_gettop (L);
        internal_register<RetType,ClassObj,Types...>::LambdaRegisterStackOpt(L,methodName,top,func);
        lua_pop(L, 1);
    };

    template<typename RetType,typename ClassObj,typename ... Types,typename ... Otps> static void RegisterClassMethod(lua_State *L,std::string name,std::string methodName,RetType (ClassObj::*func)(Types ... args),Otps ...optArgs){
        lua_getglobal(L, name.c_str());
        int top = lua_gettop (L);
        internal_register<RetType,ClassObj,Types...>::LambdaRegisterStackOpt(L,methodName,top,func,optArgs...);
        lua_pop(L, 1);
    };

    static void MakeTypeObserver(lua_State *L,T1 *obj,int (*cind)(lua_State *L),int (*nind)(lua_State *L)){
        lua_newtable(L);
        lua_pushstring(L,"__self");
        T1 **usr = static_cast<T1**>(lua_newuserdata(L, sizeof(T1)));
        *usr = obj;
        lua_settable(L,-3);

        lua_newtable(L);
        int ps = lua_gettop (L);
        lua_pushcfunction(L, cind);
        lua_setfield(L, ps, "__index");
        lua_pushcfunction(L, nind);
        lua_setfield(L, ps, "__newindex");
        lua_setmetatable(L, -2);
        lua_settable(L,-3);
    }


};


#endif // LUATOOLSBEH
#endif // DISABLE_LUAINTERFACE
