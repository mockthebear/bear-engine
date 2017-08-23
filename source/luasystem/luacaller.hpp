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
        static bool Pcall(int arg = 0,int returns = 0,int ext = 0){
            if (lua_pcall(L, arg, returns,  ext) != 0) {
                Console::GetInstance().AddTextInfo(utils::format("Lua error: :c %s -> [%s]",lua_tostring(L, -1),lastCalled.c_str()));
                lua_pop(L,1);
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
