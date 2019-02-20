#include "luainterface.hpp"
#include "luatools.hpp"

#include "luagamestate.hpp"
#include "luaui.hpp"

#ifndef DISABLE_LUAINTERFACE
#include "../engine/gamebase.hpp"
#include "../engine/bear.hpp"




LuaInterface& LuaInterface::Instance()
{
    static LuaInterface S;

    return S;
}

LuaInterface::LuaInterface()
{
    parametersCount = 0;
    L = NULL;

}

LuaInterface::~LuaInterface()
{
    if (L != NULL)
    {
        Close();
    }
}





void LuaInterface::Close()
{
    if (!Game::Crashed){
        bear::out << "Called lua close.\n";
        //if (L != NULL){

        const char closeCode[] = "__REFS = nil;\n"
                                 "local bytes = collectgarbage('count');\n"
                                 "collectgarbage('collect');\n"
                                 "print( ( (bytes-collectgarbage('count'))  * 1024 ) .. ' bytes where released.');\n";

        luaL_loadstring(L, closeCode);
        lua_pcall(L, 0, LUA_MULTRET, 0);

        bear::out << "Closing function references\n";
        LuaManager::ClearRegisteredReferences();
        bear::out << "Closing saved object references\n";
        LuaManager::ClearReferences();
        bear::out << "Closing lua remains.\n";

        lua_close(L);

        //
    }
    L = nullptr;
    bear::out << "[Close] Lua is closed.\n";
}
void LuaInterface::Startup()
{
    L = luaL_newstate();
    if (L == NULL)
    {
        bear::out << "Cannot start lua\n";
    }
    else
    {
        Game::startFlags |= BEAR_FLAG_START_LUA;
        Console::GetInstance().AddTextInfo("Lua started.");
        LuaManager::L = L;
        luaL_openlibs(L);


        Console::GetInstance().AddTextInfo("Register classes...");
        RegisterClasses();


        if ( luaL_loadfile(L, DirManager::AdjustUserPath("lua/main.lua").c_str())==0 )
        {
            // execute Lua program
            if (lua_pcall(L, 0, LUA_MULTRET, 0) != 0)
            {

                bear::out << "Lua error: " << lua_tostring(L, -1) << "\n";
                lua_pop(L, 1); // remove error message
            }
        }
        else
        {
            bear::out << "Lua error: " << lua_tostring(L, -1) << "\n";
            lua_pop(L, 1); // remove error message
        }
        bear::out << "[lua] Lua started!" << "\n";
    }

}
void LuaInterface::InsertInt(int n)
{
    parametersCount++;
    lua_pushnumber(L, n);
}

void LuaInterface::InsertFloat(float n)
{
    parametersCount++;
    lua_pushnumber(L, n);
}
void LuaInterface::InsertPointer(void *p)
{
    parametersCount++;
    lua_pushlightuserdata(L, p);
}
bool LuaInterface::LoadPrepare(std::string name)
{

    if (!HScript[name] || name == "")
    {
        return false;
    }
    if (ScriptTable[HScript[name]] == "")
    {
        return false;
    }
    lua_getglobal(L, "SCRIPTS");
    lua_getfield(L, -1, ScriptTable[HScript[name]].c_str());
    lua_getfield(L, -1, "code");
    return true;
}

bool LuaInterface::LoadPrepareC(std::string name,std::string func)
{

    if (!HScript[name] || name == "")
    {
        return false;
    }
    if (ScriptTable[HScript[name]] == "")
    {
        return false;
    }
    lua_getglobal(L, "SCRIPTS");
    lua_getfield(L, -1, ScriptTable[HScript[name]].c_str());
    lua_getfield(L, -1, func.c_str());
    return true;
}

bool LuaInterface::Call()
{
    if (lua_pcall(L, parametersCount, 1, 0) != 0)
    {
        bear::out << "-- " << lua_tostring(L, -1) << "\n";
        lua_pop(L, 1); // remove error message
        parametersCount = 0;
        return false;
    }
    else
    {
        int ret = (int)lua_tonumber(L, -1);
        parametersCount = 0;
        lua_pop(L, 1);
        return ret > 0;
    }



}



void LuaInterface::Update(float dt)
{
REUP:
    ;
    for(int i = timers.size() - 1; i >= 0; --i)
    {
        timers[i].T -= dt;
        if (timers[i].T <= 0)
        {
            RunTimer(timers[i].name);
            timers.erase(timers.begin()+i);
            goto REUP;
        }
    }
}



bool LuaInterface::CallScript(std::string name,std::string fName)
{
    if ( luaL_loadfile(L, DirManager::AdjustUserPath(name).c_str())==0 )
    {
        if (lua_pcall(L, 0, LUA_MULTRET, 0) != 0)
        {
            bear::out << "Lua error: " << lua_tostring(L, -1) << "\n";
            lua_pop(L, 1);
            return false;
        }
        if (fName != "")
        {
            lua_getglobal(L, fName.c_str());
            if (lua_isnil(L,-1) || !lua_isfunction(L,-1))
            {
                bear::out << "Lua error: "<<fName<<" in file "<<name<<" is not a function\n";
                lua_pop(L, 1);
                return false;
            }
            if (lua_pcall(L, 0, LUA_MULTRET,0) != 0)
            {
                bear::out << "Lua error: " << lua_tostring(L, -1) << "\n";
                lua_pop(L, 1);
                return false;
            }
            return lua_tointeger(L, -1) != 0;
        }
    }
    else
    {
        bear::out << "Lua error: " << lua_tostring(L, -1)<< "\n";
        lua_pop(L, 1);
        return false;
    }
    return true;
}


bool LuaInterface::GetGlobal(std::string global)
{
    lua_getglobal(L,global.c_str());
    return !lua_isnil(L,-1);
}

bool LuaInterface::GetMember(std::string member)
{
    lua_pushstring(L, member.c_str());
    lua_gettable(L, -2);
    return !lua_isnil(L,-1);
}
bool LuaInterface::GetMember(int num)
{
    lua_pushnumber(L, num);
    lua_gettable(L, -2);
    return !lua_isnil(L,-1);
}


bool LuaInterface::RunTimer(std::string name)
{

    if (!HScript[name] || name == "")
    {
        return false;
    }
    if (ScriptTable[HScript[name]] == "")
    {
        return false;
    }
    lua_getglobal(L, "SCRIPTS");
    lua_getfield(L, -1, ScriptTable[HScript[name]].c_str());
    lua_getfield(L, -1, "timer");
    lua_pushnumber(L, 1);

    if (lua_pcall(L, 1, 0, 0) != 0)
    {
        bear::out << "[Lua error] " << lua_tostring(L, -1) << "\n";
        lua_pop(L, 1);
        return false;

    }
    //bear::out << "The return value of the function was " << lua_tostring(L, -1) << std::endl;
    return true;
}


bool LuaInterface::CallClear()
{
    bear::out << "Cleaning" << "\n";
    //LuaCaller::CallClear(L);
    bear::out << "Clear" << "\n";
    return true;
}

bool LuaInterface::RunScript(std::string name)
{

    if (!HScript[name] || name == "")
    {
        return false;
    }
    if (ScriptTable[HScript[name]] == "")
    {
        return false;
    }
    lua_getglobal(L, "SCRIPTS");
    lua_getfield(L, -1, ScriptTable[HScript[name]].c_str());
    lua_getfield(L, -1, "code");
    lua_pushnumber(L, 1);

    lua_pcall(L, 1, 0, 0);
    //bear::out << "The return value of the function was " << lua_tostring(L, -1)<< "\n";
    return true;
}



void LuaInterface::RegisterClasses()
{
    LuaCaller::Startup(L);

    RegisterTimerEvents();
    RegisterSound();
    RegisterFonts();
    RegisterObjects();
    RegisterCollision();
    RegisterCamera();
    RegisterAssets();
    RegisterSprite();
    RegisterInput();
    RegisterUI();
    RegisterScreenRender();
    RegisterSpriteEffectClass();
    #ifdef GENERATEDOCUMENTATION
    NameAtlas::GetInstance().GenerateDoc();
    #endif // GENERATEDOCUMENTATION
}

int LuaInterface::LoopEvent(lua_State *L){
    int32_t parameters = lua_gettop(L);

    if(lua_isfunction(L, -parameters) == 0)
    {
        bear::out << "First argument should be an function!\n";
        lua_pushboolean(L, false);
        return 1;
    }

    std::list<int> params;
    for(int32_t i = 0; i < parameters-2; ++i)
    {
        params.push_back(luaL_ref(L, LUA_REGISTRYINDEX));
    }


    int repeats = lua_tonumber(L, -1);
    lua_pop(L,1);

    float delay = std::max((float)0.00f,(float)lua_tonumber(L, -1));
    lua_pop(L,1);
    int luaF = luaL_ref(L, LUA_REGISTRYINDEX);

    auto F = [luaF,params,L]()
    {
        lua_rawgeti(L, LUA_REGISTRYINDEX, luaF);
        for (auto it = params.rbegin(); it != params.rend(); ++it)
        {
            lua_rawgeti(L, LUA_REGISTRYINDEX, *it);
        }
        LuaManager::Pcall(params.size());
        for(auto &it : params)
        {
            luaL_unref(L, LUA_REGISTRYINDEX, it);
        }
        //params.clear();
        luaL_unref(L, LUA_REGISTRYINDEX, luaF);
    };




    uint64_t evId = g_scheduler.AddEvent(delay,F,repeats);
    lua_pushnumber(L, evId);
    return 1;
}


int LuaInterface::AddEvent(lua_State *L)
{
    int32_t parameters = lua_gettop(L);
    if(lua_isfunction(L, -parameters) == 0)
    {
        bear::out << "First argument should be an function!\n";
        lua_pushboolean(L, false);
        return 1;
    }

    std::list<int> params;
    for(int32_t i = 0; i < parameters-2; ++i)
    {
        params.push_back(luaL_ref(L, LUA_REGISTRYINDEX));
    }


    float delay = std::max((float)0.00f,(float)lua_tonumber(L, -1));
    lua_pop(L,1);

    int luaF = luaL_ref(L, LUA_REGISTRYINDEX);

    auto F = [luaF,params,L]()
    {
        lua_rawgeti(L, LUA_REGISTRYINDEX, luaF);
        for (auto it = params.rbegin(); it != params.rend(); ++it)
        {
            lua_rawgeti(L, LUA_REGISTRYINDEX, *it);
        }
        LuaManager::Pcall(params.size());
        for(auto &it : params)
        {
            luaL_unref(L, LUA_REGISTRYINDEX, it);
        }
        //params.clear();
        luaL_unref(L, LUA_REGISTRYINDEX, luaF);
    };

    uint64_t evId = g_scheduler.AddEvent(delay,F);
    lua_pushnumber(L, evId);
    return 1;

}






#endif
