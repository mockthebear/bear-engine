#include "../settings/definitions.hpp"
#include "../performance/console.hpp"
#include <iostream>
#include <functional>
#include <string.h>
#ifndef DISABLE_LUAINTERFACE


    #ifndef LUATOOLSBEH
    #define LUATOOLSBEH
    #include LUA_INCLUDE
    #include "luatypewrapper.hpp"

typedef std::function<int(lua_State*)> LuaCFunctionLambda;
/*
    Function used to store/get self and lua state
*/

class LuaData{
    public:
        template <typename T> static T* GetSelf(){
            lua_getfield(L, 1, "__self");
            T** data = (T**)lua_touserdata(LuaData::L, -1);
            if (!data){
                return nullptr;
            }
            return (*data);
        }
        template <typename T> static T** GetSelfReference(){
            lua_getfield(L, 1, "__self");
            T** data = (T**)lua_touserdata(LuaData::L, -1);
            if (!data){
                return nullptr;
            }
            return (data);
        }
    static bool IsDebug;
    static lua_State *L;
};


/*
    Expander to variadic elements using tuple
*/

template<int N>
    struct readLuaValues {
    template<typename Tuple> static void Read(Tuple& tuple,lua_State *L,int stackpos = -1) {
        typedef typename std::tuple_element<N-1, Tuple>::type ValueType;
        ValueType v = GenericLuaGetter<ValueType>::Call(L,stackpos);
        std::get<N-1>(tuple) = v;
        readLuaValues<N-1>::Read(tuple,L,stackpos);
    }
};

template<>
    struct readLuaValues<0> {
        template<typename Tuple> static void Read(Tuple& tuple,lua_State *L,int stackpos = -1) { }
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

       Ctype* part = LuaData::GetSelf<Ctype>();
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
       Ctype* part = LuaData::GetSelf<Ctype>();
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
    template<typename R,typename... Args> static void expand(lua_State *L,R r,const Args&... args) {
        GenericLuaReturner<R>::Ret(r,L);
        pexpander::expand(L,args...);
    };
    static void expand(lua_State *L) {
    }
    template<typename R> static void expand(lua_State *L,R r) {
        GenericLuaReturner<R>::Ret(r,L);
    };
};


class LuaCaller{
    public:
        template <int N> static int BaseEmpty(lua_State *L){
            LuaCFunctionLambda **v = (LuaCFunctionLambda **)lua_touserdata(L, lua_upvalueindex(N));
            if (!v || !(*v)){
                Console::GetInstance().AddTextInfo(utils::format("[LUA]could not call closure %d because null reference",v));
                lua_pushstring(L,"LUA]could not call closure %d because null reference");
                lua_error (L);
                return 0;
            }
            (*(*v))(L);
            return 1;
        }

        template <int N> static int Base(lua_State *L){
            LuaCFunctionLambda **v = (LuaCFunctionLambda **)lua_touserdata(L, lua_upvalueindex(N));
            if (!v || !(*v)){
                Console::GetInstance().AddTextInfo(utils::format("[LUA]could not call closure %d because null reference",v));
                lua_pushstring(L,"LUA]could not call closure %d because null reference");
                lua_error (L);
                return 0;
            }
            int **self = LuaData::GetSelfReference<int>();
            if (!self){
                Console::GetInstance().AddTextInfo(utils::format("[LUA]could not call function because of an deleted reference",v));
                lua_pushnil(L);
                return 1;
            }
            (*(*v))(L);
            return 1;
        }

        static void Startup(lua_State *L){
            lua_newtable(L);
            lua_setglobal(L, "__REFS");
            char baseMaker[] =  "function New(objtype,...)\n"
                                "   local obj = objtype(...)\n"
                                "   print(\"made \",obj.id,obj)\n"
                                "   __REFS[obj.id] = obj\n"
                                "   return obj\n"
                                "end\n"
                                "function CallFromField(index,str,...)\n"
                                "    if __REFS[index] then\n"
                                "        if __REFS[index][str] then\n"
                                "            return __REFS[index][str](__REFS[index],...)\n"
                                "        end\n"
                                "    end\n"
                                "    return false\n"
                                "end\n"
                                "function CallOnField(index,str,...)\n"
                                "    if __REFS[index] then\n"
                                "        if _G[str] and type(_G[str]) == \"function\" then\n"
                                "            _G[str](__REFS[index],...)\n"
                                "        end\n"
                                "    end\n"
                                "    return false\n"
                                "end\n";
            luaL_loadstring(L, baseMaker);
            lua_pcall(L, 0, LUA_MULTRET, 0);

        };
        static bool LoadFile(lua_State *L,std::string name){
            if ( luaL_loadfile(L, name.c_str()) != 0 ) {
                Console::GetInstance().AddTextInfo(utils::format("[Lua error]: %s",lua_tostring(L, -1)));
                lua_pop(L, 1);
                return false;
            }
            return true;
        }
        template <typename ... Types> static bool Pcall(lua_State *L,Types ... args){
            pexpander::expand(L,args...);
            if (lua_pcall(L, sizeof...(Types), LUA_MULTRET, 0) != 0){
                Console::GetInstance().AddTextInfo(utils::format("[Lua error]: %s",lua_tostring(L, -1)));
                lua_pop(L, 1);
                return false;
            }
            return true;
        }


        template <typename ... Types> static bool CallGlobalField(lua_State *L,std::string field,Types ... args){
            lua_getglobal(L, field.c_str());
            if(!lua_isfunction(L, -1) ){
                return false;
            }
            pexpander::expand(L,args...);
            if (lua_pcall(L, sizeof...(Types), 1, 0) != 0) {
                Console::GetInstance().AddTextInfo(utils::format("[Lua error]: %s",lua_tostring(L, -1)));
                lua_pop(L,1);
                return false;
            }
            bool ret = lua_toboolean(L,-1);
            lua_pop(L,1);
            return ret;
        }


        template <typename ... Types> static bool CallField(lua_State *L,uint64_t index,std::string field,Types ... args){
            lua_getglobal(L, "CallOnField");
            if(!lua_isfunction(L, -1) ){
                return false;
            }

            lua_pushinteger(L, index);
            lua_pushstring(L, field.c_str());
            pexpander::expand(L,args...);

            if (lua_pcall(L, 2 + (sizeof...(Types)), 1, 0) != 0) {
                Console::GetInstance().AddTextInfo(utils::format("Lua error: %s",lua_tostring(L, -1)));
                lua_pop(L,1);
                return false;
            }

            if (lua_isnil(L, -1)){
                return false;
            }
            bool ret = lua_toboolean(L,-1);
            lua_pop(L,1);
            return ret;
        }
        template <typename ... Types> static bool CallSelfField(lua_State *L,uint64_t index,std::string field,Types ... args){
            lua_getglobal(L, "CallFromField");
            if(!lua_isfunction(L, -1) ){
                return false;
            }
            lua_pushinteger(L, index);
            lua_pushstring(L, field.c_str());
            pexpander::expand(L,args...);

            if (lua_pcall(L, 2 + (sizeof...(Types)), 1, 0) != 0) {
                Console::GetInstance().AddTextInfo(utils::format("Lua error: %s",lua_tostring(L, -1)));
                lua_pop(L,1);
                return false;
            }
            if (lua_isnil(L, -1)){
                return false;
            }
            bool ret = lua_toboolean(L,-1);
            lua_pop(L,1);
            return ret;
        }
        static void DeleteField(lua_State *L,uint64_t index){
            lua_getglobal(L, "__REFS");
            lua_pushinteger(L, index);
            lua_pushnil(L);
            lua_settable(L, -3);
        };
};


template<typename T1,typename ... Types> void RandomRegister(lua_State *L,std::string str,T1 func(Types ... args) ){

    LuaCFunctionLambda f = [func,str](lua_State *L2) -> int {
        int argCount = sizeof...(Types);
        if (argCount > lua_gettop(L2)){
            Console::GetInstance().AddTextInfo(utils::format("[LUA][1]Too few arguments on function %s. Expected %d got %d",str,lua_gettop(L2),argCount));
        }
        if (argCount < lua_gettop(L2)){
            Console::GetInstance().AddTextInfo(utils::format("[LUA][1]Too much arguments on function %s. Expected %d got %d",str,lua_gettop(L2),argCount));
        }
        std::tuple<Types ...> ArgumentList;
        readLuaValues<sizeof...(Types)>::Read(ArgumentList,L2,-1);
        T1 rData = expander<sizeof...(Types),T1>::expand(ArgumentList,L2,func);
        GenericLuaReturner<T1>::Ret(rData,L2);
        return 1;
    };
    LuaCFunctionLambda** baseF = static_cast<LuaCFunctionLambda**>(lua_newuserdata(L, sizeof(LuaCFunctionLambda) ));
    (*baseF) = new LuaCFunctionLambda(f);
    lua_pushcclosure(L, LuaCaller::BaseEmpty<1>,1);
    lua_setglobal(L, str.c_str());
};

template<typename T1,typename ClassObj,typename ... Types> struct internal_register{
     static void LambdaRegisterStack(lua_State *L,std::string str,int stackPos,T1 (ClassObj::*func)(Types ... args) ){
        LuaCFunctionLambda f = [func,str](lua_State *L2) -> int {
            int argCount = sizeof...(Types);
            int argNecessary = lua_gettop(L2)-2;
            if (argCount < argNecessary){
                Console::GetInstance().AddTextInfo(utils::format("[LUA][3]Too few arguments on function %s. Expected %d got %d",str,argNecessary,argCount));
            }
            if (argCount > argNecessary){
                Console::GetInstance().AddTextInfo(utils::format("[LUA][3]Too much arguments on function %s. Expected %d got %d",str,argNecessary,argCount));
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

        lua_pushcclosure(L, LuaCaller::Base<1>,1);
        lua_setfield(L, stackPos,  str.c_str());

    };
};

template<typename ClassObj,typename ... Types> struct internal_register<void,ClassObj,Types...>{
     static void LambdaRegisterStack(lua_State *L,std::string str,int stackPos,void (ClassObj::*func)(Types ... args) ){
        LuaCFunctionLambda f = [func,str](lua_State *L2) -> int {
            int argCount = sizeof...(Types);
            int argNecessary = lua_gettop(L2)-2;
            if (argCount < argNecessary){
                Console::GetInstance().AddTextInfo(utils::format("[LUA][3]Too few arguments on function %s. Expected %d got %d",str,argNecessary,argCount));
            }
            if (argCount > argNecessary){
                Console::GetInstance().AddTextInfo(utils::format("[LUA][3]Too much arguments on function %s. Expected %d got %d",str,argNecessary,argCount));
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
        lua_pushcclosure(L, LuaCaller::Base<1>,1);
        lua_setfield(L, stackPos,  str.c_str());

    };
};



template<typename T1,typename ... Types> void LambdaClassRegister(lua_State *L,std::string str,int stackPos,std::function<T1(Types ... args)> func){
    LuaCFunctionLambda f = [func,str](lua_State *L2) -> int {
        int argCount = sizeof...(Types);
        if (argCount > lua_gettop(L2)){
            Console::GetInstance().AddTextInfo(utils::format("[LUA][4]Too few arguments on function %s. Expected %d got %d",str,lua_gettop(L2),argCount));
        }
        if (argCount < lua_gettop(L2)-2){
            Console::GetInstance().AddTextInfo(utils::format("[LUA][4]Too much arguments on function %s. Expected %d got %d",str,lua_gettop(L2)-2,argCount));
        }
        std::tuple<Types ...> ArgumentList;
        readLuaValues<sizeof...(Types)>::Read(ArgumentList,L2);
        T1 rData = expander<sizeof...(Types),T1>::expand(ArgumentList,L2,func);
        GenericLuaReturner<T1>::Ret(rData,L2);
        return 1;
    };
    LuaCFunctionLambda** baseF = static_cast<LuaCFunctionLambda**>(lua_newuserdata(L, sizeof(LuaCFunctionLambda) ));
    (*baseF) = new LuaCFunctionLambda(f);
    lua_pushcclosure(L, LuaCaller::BaseEmpty<1>,1);
    lua_setfield(L, stackPos,  str.c_str());
}

template<typename ... Types> void LambdaClassRegister(lua_State *L,std::string str,int stackPos,std::function<void(Types ... args)> func){
    LuaCFunctionLambda f = [func,str](lua_State *L2) -> int {
        int argCount = sizeof...(Types);
        if (argCount > lua_gettop(L2)){
            Console::GetInstance().AddTextInfo(utils::format("[LUA][4]Too few arguments on function %s. Expected %d got %d",str,lua_gettop(L2),argCount));
        }
        if (argCount < lua_gettop(L2)-2){
            Console::GetInstance().AddTextInfo(utils::format("[LUA][4]Too much arguments on function %s. Expected %d got %d",str,lua_gettop(L2)-2,argCount));
        }
        std::tuple<Types ...> ArgumentList;
        readLuaValues<sizeof...(Types)>::Read(ArgumentList,L2);
        expander<sizeof...(Types),void>::expand(ArgumentList,L2,func);
        //GenericLuaReturner<T1>::Ret(rData,L2);
        return 0;
    };
    LuaCFunctionLambda** baseF = static_cast<LuaCFunctionLambda**>(lua_newuserdata(L, sizeof(LuaCFunctionLambda) ));
    (*baseF) = new LuaCFunctionLambda(f);
    lua_pushcclosure(L, LuaCaller::BaseEmpty<1>,1);
    lua_setfield(L, stackPos,  str.c_str());
}

template<typename T1,typename ... Types> void LambdaRegister(lua_State *L,std::string str,std::function<T1(Types ... args)> func){
    LuaCFunctionLambda f = [func,str](lua_State *L2) -> int {
        int argCount = sizeof...(Types);
        if (argCount > lua_gettop(L2)){
            Console::GetInstance().AddTextInfo(utils::format("[LUA][5]Too few arguments on function %s. Expected %d got %d",str,lua_gettop(L2),argCount));
        }
        if (argCount < lua_gettop(L2)){
            Console::GetInstance().AddTextInfo(utils::format("[LUA][5]Too much arguments on function %s. Expected %d got %d",str,lua_gettop(L2),argCount));
        }
        std::tuple<Types ...> ArgumentList;
        readLuaValues<sizeof...(Types)>::Read(ArgumentList,L2);
        T1 rData = expander<sizeof...(Types),T1>::expand(ArgumentList,L2,func);
        GenericLuaReturner<T1>::Ret(rData,L2);
        return 1;
    };
    LuaCFunctionLambda** baseF = static_cast<LuaCFunctionLambda**>(lua_newuserdata(L, sizeof(LuaCFunctionLambda) ));
    (*baseF) = new LuaCFunctionLambda(f);
    lua_pushcclosure(L, LuaCaller::BaseEmpty<1>,1);
    lua_setglobal(L, str.c_str());
};

class MasterGC{
    public:
    template <typename T> static int Close(lua_State *L){
        lua_getfield(L, 1, "__self");
        T** part = (T**)lua_touserdata(L, -1);
        if (part){
            (*part) = nullptr;
        }
        return 0;
    }
    template <typename T> static int CloseAndClear(lua_State *L){
        std::cout << "Close and clear\n";
        lua_getfield(L, 1, "__self");

        T** part = (T**)lua_touserdata(L, -1);
        std::cout << "Part is: " <<part << "\n";
        if (part){
            LuaCaller::CallSelfField(LuaData::L,(uint64_t)(*part),"Close");
            LuaCaller::DeleteField(L,(uint64_t)(*part));
            (*part)->Kill();
            /*
                Erase reference
            */
            (*part) = nullptr;
        }
        return 0;
    }
    template <typename T> static int GC(lua_State *L){
        T** part = (T**)lua_touserdata(LuaData::L, -1);
        if (LuaData::IsDebug)
            Console::GetInstance().AddTextInfo(utils::format("[LUA][MasterGC]PTR: %d",part));
        if (part && *part){
            delete (*part);
            (*part) = nullptr;
            return 1;
        }
        return 0;

    }
};


template<typename T1,typename ObjT> struct TypeObserver{
    static int Newindex(lua_State *L){
        std::string field = lua_tostring(L,-2);
        ObjT         data = GenericLuaGetter<ObjT>::Call(L,-1);
        std::map<std::string,ObjT T1::*> &fieldData = TypeObserver<T1,ObjT>::getAddr();
        //std::map<std::string,std::string> &fieldName = TypeObserver<T1>::getAddrNames();
        T1 *self = LuaData::GetSelf<T1>();
        if (!self){
            Console::GetInstance().AddTextInfo(utils::format("[LUA]Setting field %s where its userdata is nullptr.",field));
            return 0;
        }
        if (fieldData[field]){
            //std::string typeName = fieldName[field];
            //std::cout << typeName << " APPLY2\n";
            ObjT T1::* fieldptr = fieldData[field];
            self->*fieldptr = data;
        }
        return 0;
    }

    static int Index(lua_State *L){
        std::string field = lua_tostring(L,-1);
        std::map<std::string,ObjT T1::*> &fieldData = TypeObserver<T1,ObjT>::getAddr();
        //std::map<std::string,std::string> &fieldName = TypeObserver<T1>::getAddrNames();
        T1 *self = LuaData::GetSelf<T1>();
        if (!self){
            Console::GetInstance().AddTextInfo(utils::format("[LUA]Requesting field %s where its userdata is nullptr.",field));
        }
        if (fieldData[field]){
            //std::string typeName = fieldName[field];
            //std::cout << typeName << " APPLY\n";
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
            return;
        }
        LambdaClassRegister(L,methodName,-2,func);
        lua_pop(L, 1);
    }
};

template<typename T1> struct ClassRegister{
     static void RegisterClassOutside(lua_State *L,std::string name,
		 std::function<T1*(lua_State*)> makerF = std::function<T1*(lua_State*)>(),
                                      LuaCFunctionLambda *gc_func = nullptr,
                                      std::function<void(lua_State*,T1*)> methods_f = std::function<void(lua_State*, T1*)>()

        ){

        lua_newtable(L);
        lua_pushvalue(L, -1);
        lua_setglobal(L, name.c_str());
        int methods = lua_gettop(L);
        lua_newtable(L);
        int methodsTable = lua_gettop(L);

        static LuaCFunctionLambda Flambb = [name,makerF,gc_func,methods_f](lua_State* L) -> int{
			T1 *obj;
			if (makerF) {
				obj = makerF(L);
			}else{
				obj = new T1();
			}

            luaL_checktype(L, 1, LUA_TTABLE);
            lua_newtable(L);
            //int methods = lua_gettop(L);

            lua_pushstring(L,"id");
            lua_pushnumber(L,(uint64_t)obj);
            lua_settable(L,-3);
            lua_pushstring(L,"type");
            lua_pushstring(L,typeid(T1).name());
            lua_settable(L,-3);
            lua_pushstring(L,"data");

            ClassRegister<T1>::MakeTypeObserver(L,obj,IndexerHelper<T1>::Index,IndexerHelper<T1>::Newindex);

            if (methods_f){
                methods_f(L,obj);
            }

            lua_pushvalue(L,1);
            lua_setmetatable(L, -2);
            lua_pushvalue(L,1);
            lua_setfield(L, 1, "__index");
            T1 **usr = static_cast<T1**>(lua_newuserdata(L, sizeof(T1)));
            *usr = obj;

            lua_getglobal(L, name.c_str());
            lua_setmetatable(L, -2);
            lua_setfield(L, -2, "__self");




            /*
                http://loadcode.blogspot.com.br/2007/02/wrapping-c-classes-in-lua.html
            */
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
        lua_getglobal(L, name.c_str());
        if (gc_func && *gc_func){
            LuaCFunctionLambda** baseF = static_cast<LuaCFunctionLambda**>(lua_newuserdata(L, sizeof(LuaCFunctionLambda) ));
            (*baseF) = &(*gc_func);
            lua_pushcclosure(L, LuaCaller::Base<1>,1);
        }else{
            lua_pushcfunction(L, MasterGC::GC<T1>);
        }
        lua_setfield(L, -2, "__gc");
        lua_pop(L, 1);
        //Destroy

        lua_getglobal(L, name.c_str());
        lua_pushcfunction(L, MasterGC::CloseAndClear<T1>);
        lua_setfield(L, -2,  "destroy");
        lua_pop(L, 1);
    };


    template<typename RetType,typename ... Types> static void RegisterClassLambdaMethod(lua_State *L,std::string name,std::string methodName,std::function<RetType(Types ... args)> func){
        lua_getglobal(L, name.c_str());
        LambdaClassRegister(L,methodName,-2,func);
        lua_pop(L, 1);
    }
    template<typename RetType,typename ClassObj,typename ... Types> static void RegisterClassMethod(lua_State *L,std::string name,std::string methodName,RetType (ClassObj::*func)(Types ... args)){
        lua_getglobal(L, name.c_str());
        int top = lua_gettop (L);
        internal_register<RetType,ClassObj,Types...>::LambdaRegisterStack(L,methodName,top,func);
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
        //lua_pop(L, );
    }
};



#endif // LUATOOLSBEH
#endif // DISABLE_LUAINTERFACE
