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
