#include "luatools.hpp"
#include "../framework/userfile.hpp"
#ifndef DISABLE_LUAINTERFACE

bool        GenericLuaGetter<bool>::Empty = false;
std::string GenericLuaGetter<std::string>::Empty = "";


float       GenericLuaGetter<float>::Empty = 0.0f;
double       GenericLuaGetter<double>::Empty = 0.0;
int         GenericLuaGetter<int>::Empty = 0;
lua_State* LuaManager::L = nullptr;
bool LuaManager::IsDebug = false;

char GenericLuaGetter<char*>::transfer[512];

#ifdef GENERATEDOCUMENTATION

NameAtlas& NameAtlas::GetInstance(){
    static NameAtlas ref;
    return ref;
}

void NameAtlas::GenerateDoc(){
    UserFile f;
    f.Open("doc.txt");
    f.Write(g_s.str());
    f.Close();
}

#endif // GENERATEDOCUMENTATION

//GenericLuaGetter<const char*>::Empty[0] = '\0';
#endif
