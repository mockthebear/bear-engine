#include "luacaller.hpp"
#include <vector>
#include <functional>

#ifndef DISABLE_LUAINTERFACE

std::string LuaManager::lastCalled = "?";


std::vector<LuaCFunctionLambda*> LuaManager::ptrs;
std::vector<std::function<void()>> LuaManager::eraseLambdas;
#endif
