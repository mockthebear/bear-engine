#include "luatools.hpp"
#ifndef DISABLE_LUAINTERFACE

bool GenericLuaCaller<bool>::Empty = false;
std::string GenericLuaCaller<std::string>::Empty = "";
float GenericLuaCaller<float>::Empty = 0.0f;
int GenericLuaCaller<int>::Empty = 0;

#endif