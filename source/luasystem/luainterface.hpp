#include "../settings/definitions.hpp"
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

};

    #endif // LUAINTH
#else
//Dummy in case of not supporting lua
class lua_State{
	public:
		lua_State(){};
		int x;
};
class LuaInterface{
    public:
        ~LuaInterface(){};;
        LuaInterface(){};
        void Startup(){};
        void Register(std::string str,int (*F)(lua_State*)){};
        bool RunScript(std::string name){return false;};
        bool CallScript(std::string name){return false;};
        bool RunTimer(std::string name){return false;};
        bool CallClear(){return false;};
        bool LoadPrepare(std::string name){return false;};
        bool LoadPrepareC(std::string name,std::string func){return false;};
        bool Call(){return false;};
        void Update(float dt){};
        void InsertInt(int n){};
        void InsertPointer(void *p){};
        void InsertFloat(float n){};
        static LuaInterface& Instance();
};

#endif
