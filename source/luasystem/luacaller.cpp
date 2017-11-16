#include "luacaller.hpp"

#ifndef __EMSCRIPTEN__
std::string LuaManager::lastCalled = "?";


std::vector<LuaCFunctionLambda*> LuaManager::ptrs;
std::vector<std::function<void()>> LuaManager::eraseLambdas;
#endif // __EMSCRIPTEN__
