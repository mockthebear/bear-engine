#include "../settings/definitions.hpp"
#ifndef DISABLE_LUAINTERFACE
#pragma once

#include "../engine/gamebase.hpp"
#include "../performance/console.hpp"
#include <iostream>
#include <functional>
#include <string.h>
#include LUA_INCLUDE

/*
    Function used to store/get self and lua state
*/
typedef std::function<int(lua_State*)> LuaCFunctionLambda;

class LuaManager{
    public:
        void static ClearRegisteredReferences(){
            for (auto &it : ptrs){
                delete it;
            }
            ptrs.clear();
        };
        static void AddReference(LuaCFunctionLambda *v){
            ptrs.emplace_back(v);
        }
        template <typename T> static T* GetSelf(){
            lua_getfield(L, 1, "__self");
            T** data = (T**)lua_touserdata(LuaManager::L, -1);
            if (!data){
                return nullptr;
            }
            return (*data);
        }
        template <typename T> static T** GetSelfReference(){
            lua_getfield(L, 1, "__self");
            T** data = (T**)lua_touserdata(LuaManager::L, -1);
            if (!data){
                return nullptr;
            }
            return (data);
        }
        static int ErrorHandler(lua_State* L){
            std::string err;
            size_t len;
            const char *c_str = lua_tolstring(L, -1, &len);
            if(c_str && len > 0)
                err.assign(c_str, len);


            lua_pop(L,1);



            lua_getglobal(L,"debug");
            lua_getfield(L, -1, "traceback");
            lua_remove(L,-2);

            lua_pushlstring(L, err.c_str(), err.length());
            lua_pushinteger(L, 1);
            lua_call(L, 2, 1);

            std::string error = lua_tostring(L, -1);
            lua_pop(L,1);


            lua_pushlstring(L, error.c_str(), error.length());
            return 1;
        }

        static bool Rawcall(int arg = 0,int returns = 0,int ext = 0){
            int ret = lua_pcall(L, arg, returns, ext );
            if (ret != 0){

                Console::GetInstance().AddTextInfo(utils::format("Lua error:  %s -> [%s]",lua_tostring(L, -1),lastCalled.c_str()));


                return false;
            }
            return true;
        }
        static bool Pcall(int arg = 0,int returns = 0,int ext = 0){
            int previousStackSize = lua_gettop(L);
            int errorFuncIndex = previousStackSize - arg;
            lua_pushcclosure(L, &LuaManager::ErrorHandler, 0);
            lua_insert(L, errorFuncIndex);


            int ret = lua_pcall(L, arg, returns,  errorFuncIndex);

            lua_remove(L, errorFuncIndex);

            if (ret != 0){

                Console::GetInstance().AddTextInfo(utils::format("Lua error:  %s -> [%s]",lua_tostring(L, -1),lastCalled.c_str()));


                return false;
            }
            return true;
        }
    static std::vector<LuaCFunctionLambda*> ptrs;
    static std::string lastCalled;
    static bool IsDebug;
    static lua_State *L;
    static std::vector<std::function<void()>> eraseLambdas;

    static void ClearReferences(){
        for (auto &it : eraseLambdas){
            it();
        }
    }
};



#endif // DISABLE_LUAINTERFACE
