#include "luainterface.hpp"
#include "luatools.hpp"
#include "../performance/console.hpp"




#ifndef DISABLE_LUAINTERFACE
#include "../engine/gamebase.hpp"
#include <iostream>
#include "../engine/sound.hpp"
#include "luaobject.hpp"


LuaInterface& LuaInterface::Instance(){
    static LuaInterface S;

    return S;
}
LuaInterface::LuaInterface(){
    parametersCount = 0;
    L = NULL;

}

LuaInterface::~LuaInterface(){
    if (L != NULL)
       lua_close(L);
    std::cout << "Closing lua\n" << std::endl;
}

int CallTimer(lua_State *L)
{
    float N = lua_tonumber(L,-1);
    lua_pop(L,1);
    std::string scriptname =  lua_tostring(L,-1);
    lua_pop(L,1);
    luaTimer Letimer;
    Letimer.T = N;
    Letimer.name = scriptname;
    LuaInterface::Instance().timers.emplace_back(Letimer);
  return 0; // number of return values
}






int setPointerX(lua_State *L)
{
    int cord = lua_tonumber(L,-1);
    lua_pop(L,1);
    GameObject *t= (GameObject *)lua_touserdata(L,-1);
    lua_pop(L,1);

    t->box.x = cord;
    t->ball.x = cord;
  return 1; // number of return values
}

int setPointerY(lua_State *L)
{
    int cord = lua_tonumber(L,-1);
    lua_pop(L,1);
    GameObject *t= (GameObject *)lua_touserdata(L,-1);
    lua_pop(L,1);
    t->box.y = cord;
    t->ball.y = cord;
  return 1; // number of return values
}



int getPointerX(lua_State *L)
{
    GameObject *t= (GameObject *)lua_touserdata(L,1);
    lua_pop(L,1);
    lua_pushnumber(L,t->box.x);
  return 1; // number of return values
}


int isPlayer(lua_State *L)
{
    GameObject *t= (GameObject *)lua_touserdata(L,1);
    lua_pop(L,1);
    lua_pushboolean(L,t->Is(OBJ_PLAYER));
  return 1; // number of return values
}

int isCreature(lua_State *L)
{
    GameObject *t= (GameObject *)lua_touserdata(L,1);
    lua_pop(L,1);
    lua_pushboolean(L,t->Is(OBJ_CREATURE));
  return 1; // number of return values
}

int thisIs(lua_State *L)
{
    GameObject *t= (GameObject *)lua_touserdata(L,1);
    lua_pop(L,1);
    int K = lua_tonumber(L,1);
    lua_pop(L,1);
    lua_pushboolean(L,t->Is(K));
  return 1; // number of return values
}

int IsDead(lua_State *L)
{
    GameObject *t= (GameObject *)lua_touserdata(L,1);
    if (!t){
        return true;
    }
    lua_pushboolean(L,t->IsDead());
  return 1; // number of return values
}

int getPointerY(lua_State *L)
{
    GameObject *t= (GameObject *)lua_touserdata(L,1);
    lua_pop(L,1);
    lua_pushnumber(L,t->box.y);
  return 1; // number of return values
}

int getPointerId(lua_State *L)
{
    GameObject *t= (GameObject *)lua_touserdata(L,1);
    lua_pop(L,1);
    lua_pushnumber(L,(int)t);
  return 1; // number of return values
}





int playSound(lua_State *L)
{
    //showAnimatedText(x,y,sx,sy,r,g,b,sz,str)
    std::string index = lua_tostring(L,-1);
    lua_pop(L,1);
    Sound::PlayOnce(index.c_str());
    return 1;
}


int RegisterScript(lua_State *L){
    static int TrueIndex = 1;
    std::string scriptname =  lua_tostring(LuaInterface::Instance().L,2);
    std::string index = lua_tostring(L,1);
    lua_pop(LuaInterface::  Instance().L,2);
    LuaInterface::Instance().HScript[scriptname] = TrueIndex;
    LuaInterface::Instance().ScriptTable[TrueIndex] = index;
    TrueIndex++;
    return 0;
}


void LuaInterface::Startup(){
    L = luaL_newstate();
    if (L == NULL){
        std::cout << "Cannot start lua" << std::endl;
    }else{
        Console::GetInstance().AddTextInfo("Lua started.");
        luaL_openlibs(L);

        Register("playSound",playSound);

        Register("CallTimer",CallTimer);
        Register("RegisterScript",RegisterScript);

        Register("getPointerX",getPointerX);
        Register("getPointerY",getPointerY);
        Register("setPointerX",setPointerX);
        Register("setPointerY",setPointerY);

        Register("getPointerId",getPointerId);
        Register("isPlayer",isPlayer);
        Register("isCreature",isCreature);
        Register("thisIs",thisIs);
        Register("IsDead",IsDead);

        //
        RegisterGameObjectClass();
        RegisterParticleClass();
        if ( luaL_loadfile(L, "lua/main.lua")==0 ) {
        // execute Lua program
            std::cout << "Calling main.lua" << std::endl;
           if (lua_pcall(L, 0, LUA_MULTRET, 0) != 0){

                std::cout << "Lua error: " << lua_tostring(L, -1) << std::endl;
                lua_pop(L, 1); // remove error message
            }
        }else{
             std::cout << "Lua error: " << lua_tostring(L, -1) << std::endl;
             lua_pop(L, 1); // remove error message
        }

             std::cout << "Lua started!" << std::endl;
    }
}
void LuaInterface::InsertInt(int n){
    parametersCount++;
    lua_pushnumber(L, n);
}

void LuaInterface::InsertFloat(float n){
    parametersCount++;
    lua_pushnumber(L, n);
}
void LuaInterface::InsertPointer(void *p){
    parametersCount++;
    lua_pushlightuserdata(L, p);
}
bool LuaInterface::LoadPrepare(std::string name){

    if (!HScript[name] or name == ""){
        return false;
    }
    if (ScriptTable[HScript[name]] == ""){
        return false;
    }
    lua_getglobal(L, "SCRIPTS");
    lua_getfield(L, -1, ScriptTable[HScript[name]].c_str());
    lua_getfield(L, -1, "code");
    return true;
}

bool LuaInterface::LoadPrepareC(std::string name,std::string func){

    if (!HScript[name] or name == ""){
        return false;
    }
    if (ScriptTable[HScript[name]] == ""){
        return false;
    }
    lua_getglobal(L, "SCRIPTS");
    lua_getfield(L, -1, ScriptTable[HScript[name]].c_str());
    lua_getfield(L, -1, func.c_str());
    return true;
}

bool LuaInterface::Call(){
    if (lua_pcall(L, parametersCount, 1, 0) != 0){
            std::cout << "-- " << lua_tostring(L, -1) << std::endl;
            lua_pop(L, 1); // remove error message
            parametersCount = 0;
            return false;
    }else{
        int ret = (int)lua_tostring(L, -1);
        parametersCount = 0;
        lua_pop(L, 1);
        return ret > 0;
    }



}



void LuaInterface::Update(float dt){
    REUP:;
    for(int i = timers.size() - 1; i >= 0; --i){
        timers[i].T -= dt;
        if (timers[i].T <= 0){
           RunTimer(timers[i].name);
           timers.erase(timers.begin()+i);
           goto REUP;
        }
    }
}
bool LuaInterface::CallScript(std::string name){


    if ( luaL_loadfile(L, name.c_str())==0 ) {
        // execute Lua program
            std::cout << "Calling main.lua" << std::endl;
           if (lua_pcall(L, 0, LUA_MULTRET, 0) != 0){

                std::cout << "Lua error: " << lua_tostring(L, -1) << std::endl;
                lua_pop(L, 1); // remove error message
            }
        }else{
             std::cout << "Lua error: " << lua_tostring(L, -1) << std::endl;
             lua_pop(L, 1); // remove error message
        }
    //std::cout << "The return value of the function was " << lua_tostring(L, -1) << std::endl;
    return true;
}
bool LuaInterface::RunTimer(std::string name){

    if (!HScript[name] or name == ""){
        return false;
    }
    if (ScriptTable[HScript[name]] == ""){
        return false;
    }
    lua_getglobal(L, "SCRIPTS");
    lua_getfield(L, -1, ScriptTable[HScript[name]].c_str());
    lua_getfield(L, -1, "timer");
    lua_pushnumber(L, 1);

    if (lua_pcall(L, 1, 0, 0) != 0){
        std::cout << "[Lua error] " << lua_tostring(L, -1) << std::endl;
        lua_pop(L, 1);
        return false;

    }
    //std::cout << "The return value of the function was " << lua_tostring(L, -1) << std::endl;
    return true;
}


bool LuaInterface::CallClear(){
    std::cout << "Cleaning" << std::endl;
    timers.clear();
    lua_getglobal(L, "clearCache");
   if (lua_pcall(L, 0, 1, 0) != 0){
    std::cout << "[Lua error] " << lua_tostring(L, -1) << std::endl;
    lua_pop(L, 1);
   }
   std::cout << "Clear" << std::endl;
    return true;
}

bool LuaInterface::RunScript(std::string name){

    if (!HScript[name] or name == ""){
        return false;
    }
    if (ScriptTable[HScript[name]] == ""){
        return false;
    }
    lua_getglobal(L, "SCRIPTS");
    lua_getfield(L, -1, ScriptTable[HScript[name]].c_str());
    lua_getfield(L, -1, "code");
    lua_pushnumber(L, 1);

    lua_pcall(L, 1, 0, 0);
    std::cout << "The return value of the function was " << lua_tostring(L, -1) << std::endl;
    return true;
}

int newFunction(lua_State *L)
{
    /*LuaObject obj;
    GameObject *created = BearEngine->GetCurrentState().Pool.AddInstance(obj);
    if (created != NULL)
        ((LuaObject*)created)->Start();
    else
        return 0;

    LuaObject **usr = static_cast<LuaObject**>(lua_newuserdata(L, sizeof(LuaObject)));
    *usr = &obj;
    luaL_getmetatable(L, "LuaObject");
	lua_setmetatable(L, -1 - 1);*/
    return 0; // number of return values
}


int DeActivate(lua_State *L)

{

    LuaObject* messagePtr = (LuaObject*)lua_touserdata(L, -1);
	if (messagePtr) {
        std::cout << "[LuaObject]Garbagge collector\n";
		messagePtr->Stop();
	}
  return 0; // number of return values
}

int DUIT(lua_State *L)
{
    std::cout << "DID IT\n";
    lua_pushnumber(L,123);
  return 1; // number of return values
}

void LuaInterface::RegisterGameObjectClass(){
    //LuaObject = {}
	lua_newtable(L);
	lua_pushvalue(L, -1);
	lua_setglobal(L, "LuaObject");
	int methods = lua_gettop(L);

	// methodsTable = {}
	lua_newtable(L);
	int methodsTable = lua_gettop(L);

    // className.__call = newFunction
    lua_pushcfunction(L, newFunction);
    lua_setfield(L, methodsTable, "__call");

	// setmetatable(className, methodsTable)
	lua_setmetatable(L, methods);

	// className.metatable = {}
	luaL_newmetatable(L, "LuaObject");
	int metatable = lua_gettop(L);

	// className.metatable.__metatable = className
	lua_pushvalue(L, methods);
	lua_setfield(L, metatable, "__metatable");

	// className.metatable.__index = className
	lua_pushvalue(L, methods);
	lua_setfield(L, metatable, "__index");

	lua_pop(L, 2);

	lua_getglobal(L, "LuaObject");
	lua_pushcfunction(L, DUIT);
	lua_setfield(L, -2, "DOIT");

	luaL_getmetatable(L, "LuaObject");
	lua_pushcfunction(L, DeActivate);
	lua_setfield(L, -2, "__gc");

	lua_pop(L, 1);
}
void LuaInterface::Register(std::string str,int (*F)(lua_State*)){
    lua_register(L, str.c_str(), F);
}


/**


*/

int newParticle(lua_State *L)
{
    std::cout << "criando\n";
    float y = lua_tonumber(L,-1);
    lua_pop(L, 1);
    int x = lua_tonumber(L,-1);
    lua_pop(L, 1);
    std::cout << x << " : " << y << "\n";
    Particle *p = BearEngine->GetCurrentState().ParticlePool->AddInstance(Particle(x,y));
    if (!p){
        return 0;
    }
    Particle **usr = static_cast<Particle**>(lua_newuserdata(L, sizeof(Particle)));
    *usr = p;
    luaL_getmetatable(L, "Particle");
	lua_setmetatable(L, -1 - 1);
    return 1; // number of return values
}


int ParticleSetSprite(lua_State *L)
{

    std::cout << lua_gettop(L) << "\n";
    Particle** part = (Particle**)lua_touserdata(L, -lua_gettop(L));
    float delay = lua_tonumber(L,-1);
    lua_pop(L, 1);
    int frames = lua_tonumber(L,-1);
    lua_pop(L, 1);
    std::string sprname = lua_tostring(L,-1);
    lua_pop(L,lua_gettop(L));

    if ((*part)){
        (*part)->SetSprite(sprname,frames,delay);
    }
    return 0; // number of return values
}


int LAlphasAsDuration(lua_State *L){
    Particle** part = (Particle**)lua_touserdata(L, -1);
    if ((*part)){
        (*part)->SetAlphaAsDuration();
    }
    return 0;
}
int LParticleMovementLine(lua_State *L)
{
    int mCount = lua_gettop(L);
    if (mCount > 5 or mCount < 2){
        Console::GetInstance().AddTextInfo("Error, function has too much methods");
        return 0;
    }
    float ay = 0,ax=0;
    if (mCount == 5){
        ay = lua_tonumber(L,-1);
        lua_pop(L, 1);
    }
    if (mCount >= 4){
        ax = lua_tonumber(L,-1);
        lua_pop(L, 1);
    }


    float sy = lua_tonumber(L,-1);
    lua_pop(L, 1);
    float sx = lua_tonumber(L,-1);
    lua_pop(L,1);
    Particle** part = (Particle**)lua_touserdata(L, -1);
    if ((*part)){
        (*part)->SetPatternMoveLine(Point(sx,sy),Point(ax,ay));
    }
    return 0; // number of return values
}


void LuaInterface::RegisterParticleClass(){
    //LuaObject = {}
	lua_newtable(L);
	lua_pushvalue(L, -1);
	lua_setglobal(L, "Particle");
	int methods = lua_gettop(L);

	// methodsTable = {}
	lua_newtable(L);
	int methodsTable = lua_gettop(L);

    // className.__call = newFunction
    lua_pushcfunction(L, newParticle);
    lua_setfield(L, methodsTable, "__call");

	// setmetatable(className, methodsTable)
	lua_setmetatable(L, methods);

	// className.metatable = {}
	luaL_newmetatable(L, "Particle");
	int metatable = lua_gettop(L);

	// className.metatable.__metatable = className
	lua_pushvalue(L, methods);
	lua_setfield(L, metatable, "__metatable");

	// className.metatable.__index = className
	lua_pushvalue(L, methods);
	lua_setfield(L, metatable, "__index");

	lua_pop(L, 2);

	lua_getglobal(L, "Particle");
	lua_pushcfunction(L, ParticleSetSprite);
	lua_setfield(L, -2, "setSprite");

	lua_getglobal(L, "Particle");
	lua_pushcfunction(L, LParticleMovementLine);
	lua_setfield(L, -2, "setPatternMovementLine");

    lua_getglobal(L, "Particle");
	lua_pushcfunction(L, LAlphasAsDuration);
	lua_setfield(L, -2, "setAlphaAsDuration");


	lua_setfield(L, -2, "__gc");


	lua_pop(L, 1);
}


#endif
