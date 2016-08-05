#include "../settings/definitions.hpp"
#include "luatools.hpp"
#include <string>
#ifndef DISABLE_LUAINTERFACE

#ifndef LUAINTH
#define LUAINTH

#include LUA_INCLUDE
#include <map>
#include <unordered_map>
#include <functional>
#include <vector>
typedef struct{
    float T;
    std::string name;
} luaTimer;

class LuaInterface{
    public:
        ~LuaInterface();
        LuaInterface();
        void Startup();
        template<typename Func>void RegisterLambda(std::string str,std::function<Func> f){
            LambdaRegister(L,str,f);
        }


        template<typename Func>void FancyRegister(std::string str,Func f){
            RandomRegister(L,str,f);
        };

        std::vector<LuaMember> ListTableFields(std::string tableGlobalName="");


        template<typename Type> Type RequestValue(int stack=-1){
            if(GenericLuaType<Type>::Is(L,stack)){
                return GenericLuaCaller<Type>::Call(L,false);
            }else{
                std::cout << "Lua error. requested as string, but type not match.\n";
                return GenericLuaCaller<Type>::Empty;
            }
        }
        template<typename Type> Type RequestGlobalMember(std::string global,std::string member){
            if (!GetGlobal(global)){
                return GenericLuaCaller<Type>::Empty;
            }
            lua_pushstring(L, member.c_str());
            lua_gettable(L, -2);

            if (lua_isnil(L,-1)){
                return GenericLuaCaller<Type>::Empty;
            }

            return RequestValue<Type>();

        }
        template<typename Type> Type RequestGlobalMember(std::string global,int member){
            if (!GetGlobal(global)){
                return GenericLuaCaller<Type>::Empty;
            }
            lua_pushnumber(L, member);
            lua_gettable(L, -2);
            if (lua_isnil(L,-1)){
                return GenericLuaCaller<Type>::Empty;
            }
            return RequestValue<Type>();
        }


        void Pop(int size = 1){
            lua_pop(L, size);
        };
        bool GetGlobal(std::string global);
        bool GetMember(std::string member);
        bool GetMember(int num);

        void Register(std::string str,int (*F)(lua_State*));
        bool RunScript(std::string name);
        bool CallScript(std::string name);
        bool RunTimer(std::string name);
        bool CallClear();
        bool LoadPrepare(std::string name);
        bool LoadPrepareC(std::string name,std::string func);
        bool Call();
        void Update(float dt);
        void InsertInt(int n);
        void InsertPointer(void *p);
        void InsertFloat(float n);
        static LuaInterface& Instance();
        std::unordered_map<std::string,uint32_t>    HScript;
        std::map<int,std::string>                   ScriptTable;
        lua_State *L;
        int Parameter_int[10];
        int parametersCount;
        std::vector<luaTimer> timers;
    private:
        void RegisterGameObjectClass();
        void RegisterParticleClass();

};

    #endif // LUAINTH


#endif
