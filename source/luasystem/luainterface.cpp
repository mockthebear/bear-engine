#include "luainterface.hpp"
#include "luatools.hpp"
#include "../performance/console.hpp"






#ifndef DISABLE_LUAINTERFACE
#include "../engine/renderhelp.hpp"
#include "../engine/gamebase.hpp"
#include "../engine/bear.hpp"
#include <iostream>

#include "luaobject.hpp"
//Bindings
#include "../sound/sound.hpp"
#include "../engine/timer.hpp"
#include "../engine/sprite.hpp"

LuaInterface& LuaInterface::Instance(){
    static LuaInterface S;

    return S;
}

LuaInterface::LuaInterface(){
    parametersCount = 0;
    L = NULL;

}

LuaInterface::~LuaInterface(){
    Close();
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
    //uint32_t t= (uint32_t)lua_touserdata(L,1);
    //lua_pop(L,1);
    //lua_pushnumber(L,(int)t);
  return 0; // number of return values
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


void LuaInterface::Close(){

    if (L != NULL){
        bear::out << "Closing lua...\n";
        lua_close(L);
        L = nullptr;
    }
    bear::out << "Lua is closed.\n";
}
void LuaInterface::Startup(){
    L = luaL_newstate();
    if (L == NULL){
        bear::out << "Cannot start lua\n";
    }else{
        Console::GetInstance().AddTextInfo("Lua started.");
        LuaData::L = L;
        luaL_openlibs(L);

        /*Register("playSound",playSound);

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
        Register("IsDead",IsDead);*/

        //
        //RegisterGameObjectClass();
        //RegisterParticleClass();
		Console::GetInstance().AddTextInfo("Register classes...");
        RegisterClasses();


        if ( luaL_loadfile(L, "lua/main.lua")==0 ) {
        // execute Lua program
            bear::out << "Calling main.lua\n";
           if (lua_pcall(L, 0, LUA_MULTRET, 0) != 0){

                bear::out << "Lua error: " << lua_tostring(L, -1) << "\n";
                lua_pop(L, 1); // remove error message
            }
        }else{
             bear::out << "Lua error: " << lua_tostring(L, -1) << "\n";
             lua_pop(L, 1); // remove error message
        }

             bear::out << "Lua started!" << "\n";
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

    if (!HScript[name] || name == ""){
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

    if (!HScript[name] || name == ""){
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
            bear::out << "-- " << lua_tostring(L, -1) << "\n";
            lua_pop(L, 1); // remove error message
            parametersCount = 0;
            return false;
    }else{
        int ret = (int)lua_tonumber(L, -1);
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



bool LuaInterface::CallScript(std::string name,std::string fName){
    if ( luaL_loadfile(L, name.c_str())==0 ) {
        if (lua_pcall(L, 0, LUA_MULTRET, 0) != 0){
            bear::out << "Lua error: " << lua_tostring(L, -1) << "\n";
            lua_pop(L, 1);
            return false;
        }
        if (fName != ""){
            lua_getglobal(L, fName.c_str());
            if (lua_isnil(L,-1) || !lua_isfunction(L,-1)){
                bear::out << "Lua error: "<<fName<<" in file "<<name<<" is not a function\n";
                lua_pop(L, 1);
                return false;
            }
            if (lua_pcall(L, 0, LUA_MULTRET,0) != 0){
                bear::out << "Lua error: " << lua_tostring(L, -1) << "\n";
                lua_pop(L, 1);
                return false;
            }
            return lua_tointeger(L, -1) != 0;
        }
    }else{
        bear::out << "Lua error: " << lua_tostring(L, -1)<< "\n";
        lua_pop(L, 1);
        return false;
    }
    return true;
}


bool LuaInterface::GetGlobal(std::string global){
    lua_getglobal(L,global.c_str());
    return !lua_isnil(L,-1);
}

bool LuaInterface::GetMember(std::string member){
    lua_pushstring(L, member.c_str());
    lua_gettable(L, -2);
    return !lua_isnil(L,-1);
}
bool LuaInterface::GetMember(int num){
    lua_pushnumber(L, num);
    lua_gettable(L, -2);
    return !lua_isnil(L,-1);
}


bool LuaInterface::RunTimer(std::string name){

    if (!HScript[name] || name == ""){
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
        bear::out << "[Lua error] " << lua_tostring(L, -1) << "\n";
        lua_pop(L, 1);
        return false;

    }
    //bear::out << "The return value of the function was " << lua_tostring(L, -1) << std::endl;
    return true;
}


bool LuaInterface::CallClear(){
    bear::out << "Cleaning" << "\n";
    timers.clear();
    lua_getglobal(L, "clearCache");
   if (lua_pcall(L, 0, 1, 0) != 0){
    bear::out << "[Lua error] " << lua_tostring(L, -1) << "\n";
    lua_pop(L, 1);
   }
   bear::out << "Clear" << "\n";
    return true;
}

bool LuaInterface::RunScript(std::string name){

    if (!HScript[name] || name == ""){
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
    //bear::out << "The return value of the function was " << lua_tostring(L, -1)<< "\n";
    return true;
}



void LuaInterface::RegisterClasses(){
	Console::GetInstance().AddTextInfo("Start ref holder.");
	LuaCaller::Startup(L);
	Console::GetInstance().AddTextInfo("Reg stopwatch.");

    ClassRegister<Stopwatch>::RegisterClassOutside(L,"Stopwatch");

	ClassRegister<Stopwatch>::RegisterClassOutside(LuaData::L, "Stopwatch", [](lua_State* L) {
		return new Stopwatch();
	});

    ClassRegister<Stopwatch>::RegisterClassMethod(L,"Stopwatch","Get",&Stopwatch::Get);
    ClassRegister<Stopwatch>::RegisterClassMethod(L,"Stopwatch","Reset",&Stopwatch::Reset);
    ClassRegister<Stopwatch>::RegisterClassMethod(L,"Stopwatch","Set",&Stopwatch::Set);
    //RegisterClassMetaMethod<Stopwatch>(L,"Stopwatch","__gc");


    ClassRegister<Sound>::RegisterClassOutside(L,"Sound");
    ClassRegister<Sound>::RegisterClassMethod(L,"Sound","Open",&Sound::Open);
    ClassRegister<Sound>::RegisterClassMethod(L,"Sound","SetVolume",&Sound::SetVolume);
    ClassRegister<Sound>::RegisterClassMethod(L,"Sound","Play",&Sound::Play);
    ClassRegister<Sound>::RegisterClassMethod(L,"Sound","SetRepeat",&Sound::SetRepeat);
    ClassRegister<Sound>::RegisterClassMethod(L,"Sound","Stop",&Sound::Stop);
    ClassRegister<Sound>::RegisterClassMethod(L,"Sound","Pause",&Sound::Pause);
    ClassRegister<Sound>::RegisterClassMethod(L,"Sound","Resume",&Sound::Resume);
    ClassRegister<Sound>::RegisterClassMethod(L,"Sound","Toggle",&Sound::Toggle);
    ClassRegister<Sound>::RegisterClassMethod(L,"Sound","GetPosition",&Sound::GetPosition);
    ClassRegister<Sound>::RegisterClassMethod(L,"Sound","GetDuration",&Sound::GetDuration);
    ClassRegister<Sound>::RegisterClassMethod(L,"Sound","IsOpen",&Sound::IsOpen);
    ClassRegister<Sound>::RegisterClassMethod(L,"Sound","IsPlaying",&Sound::IsPlaying);
    ClassRegister<Sound>::RegisterClassMethod(L,"Sound","IsPaused",&Sound::IsPaused);
    ClassRegister<Sound>::RegisterClassMethod(L,"Sound","GetFileName",&Sound::GetFileName);
    ClassRegister<Sound>::RegisterClassMethod(L,"Sound","SetClassType",&Sound::SetClassType);
    ClassRegister<Sound>::RegisterClassMethod(L,"Sound","GetClassType",&Sound::GetClassType);
    ClassRegister<Sound>::RegisterClassMethod(L,"Sound","SetPitch",&Sound::SetPitch);

    static LuaCFunctionLambda sounKill = [](lua_State *L)->int{
        Sound *snd = LuaData::GetSelf<Sound>();
        if (snd){
            snd->Stop();
            delete snd;
        }
        return 1;
    };
    //RegisterClassMetaMethod<Sound>(L,"Sound","__gc",&sounKill);



    ClassRegister<Sprite>::RegisterClassOutside(LuaData::L,"Sprite",[](lua_State* L){
        std::string name = GenericLuaGetter<std::string>::Call(L);
        std::cout << "Loading: "<<name<<"\n";
        Sprite *t = new Sprite(Game::GetCurrentState().Assets.make<Sprite>(name));
        return t;
    });

    //ClassRegister<Sprite>::RegisterClassOutside(L,"Sprite");
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","Open",&Sprite::Openf);
    //RegisterClassMethod<Sprite>(L,"Sprite","Openrw",&Sprite::Openrw);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","SetClip",&Sprite::SetClip);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","Render",&Sprite::Renderxy);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","RawRender",&Sprite::RawRender);
    //RegisterClasprite>(L,"Sprite","Raw&Sprite::RawRender);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","Update",&Sprite::Update);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","ResetAnimation",&Sprite::ResetAnimation);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","SetFrame",&Sprite::SetFrame);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","GetWidth",&Sprite::GetWidth);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","GetHeight",&Sprite::GetHeight);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","GetFrameWidth",&Sprite::GetFrameWidth);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","GetFrameHeight",&Sprite::GetFrameHeight);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","SetFrameCount",&Sprite::SetFrameCount);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","GetFrameCount",&Sprite::GetFrameCount);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","SetFrameTime",&Sprite::SetFrameTime);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","GetFrameTime",&Sprite::GetFrameTime);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","IsLoaded",&Sprite::IsLoaded);
    //RegisterClasprite>(L,"Sprite","Getre",&Sprite::GetSDLTexture);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","SetCenter",&Sprite::SetCenter);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","SetRepeatTimes",&Sprite::SetRepeatTimes);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","IsAnimationOver",&Sprite::IsAnimationOver);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","SetScaleX",&Sprite::SetScaleX);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","SetScaleY",&Sprite::SetScaleY);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","SetScale",&Sprite::SetScale);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","ReBlend",&Sprite::ReBlend);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","SetAlpha",&Sprite::SetAlpha);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","SetGrid",&Sprite::SetGrid);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","SetFlip",&Sprite::SetFlip);
    //RegisterClassMethod<Sprite>(L,"Sprite","Query",&Sprite::Query);


    LambdaRegister(LuaData::L,"GetR",std::function<uint8_t(uint32_t)>([](uint32_t col){ return RenderHelp::GetR(col);}));
    LambdaRegister(LuaData::L,"GetG",std::function<uint8_t(uint32_t)>([](uint32_t col){ return RenderHelp::GetG(col);}));
    LambdaRegister(LuaData::L,"GetB",std::function<uint8_t(uint32_t)>([](uint32_t col){ return RenderHelp::GetB(col);}));
    LambdaRegister(LuaData::L,"GetA",std::function<uint8_t(uint32_t)>([](uint32_t col){ return RenderHelp::GetA(col);}));
    LambdaRegister(LuaData::L,"DrawSquareColor",std::function<int(int x,int y,int w,int h,uint8_t r,uint8_t g,uint8_t b,uint8_t a)>([](int x,int y,int w,int h,uint8_t r,uint8_t g,uint8_t b,uint8_t a){ RenderHelp::DrawSquareColorA(x-Camera::pos.x,y-Camera::pos.y,w,h,r,g,b,a,false); return 1;}));


    static LuaCFunctionLambda GarbageCollectorFunction = [](lua_State* L){
        LuaObject* part = LuaData::GetSelf<LuaObject>();
        if (part){
            part->Destroy();
            return 1;
        }
        return 1;
    };
    ClassRegister<LuaObject>::RegisterClassOutside(LuaData::L,"LuaObject",[](lua_State* L){
        int y = GenericLuaGetter<int>::Call(L);
        int x = GenericLuaGetter<int>::Call(L);
        LuaObject *t = (LuaObject*)BearEngine->GetCurrentState().Pool.AddInstance(LuaObject(x ,y));
        return t;
    },&GarbageCollectorFunction);

    ClassRegister<LuaObject>::RegisterClassMethod(LuaData::L,"LuaObject","Destroy",&LuaObject::Destroy);
    ClassRegister<LuaObject>::RegisterClassMethod(LuaData::L,"LuaObject","SetX",&LuaObject::SetX);
    ClassRegister<LuaObject>::RegisterClassMethod(LuaData::L,"LuaObject","SetY",&LuaObject::SetY);
    ClassRegister<LuaObject>::RegisterClassMethod(LuaData::L,"LuaObject","GetY",&LuaObject::GetY);
    ClassRegister<LuaObject>::RegisterClassMethod(LuaData::L,"LuaObject","GetX",&LuaObject::GetX);
    ClassRegister<LuaObject>::RegisterClassMethod(LuaData::L,"LuaObject","SetWidth",&LuaObject::SetWidth);
    ClassRegister<LuaObject>::RegisterClassMethod(LuaData::L,"LuaObject","GetWidth",&LuaObject::GetWidth);
    ClassRegister<LuaObject>::RegisterClassMethod(LuaData::L,"LuaObject","SetHeight",&LuaObject::SetHeight);
    ClassRegister<LuaObject>::RegisterClassMethod(LuaData::L,"LuaObject","GetHeight",&LuaObject::GetHeight);
    ClassRegister<LuaObject>::RegisterClassMethod(LuaData::L,"LuaObject","GetPoolIndex",&LuaObject::GetPoolIndex);
    ClassRegister<LuaObject>::RegisterClassMethod(LuaData::L,"LuaObject","GetMyRef",&LuaObject::GetMyRef);


    TypeObserver<LuaObject,bool>::RegisterMethod(LuaData::L,"forceUpdate",&LuaObject::forceUpdate);
    TypeObserver<LuaObject,bool>::RegisterMethod(LuaData::L,"forceRender",&LuaObject::forceRender);
    TypeObserver<LuaObject,int>::RegisterMethod(LuaData::L,"perspective",&LuaObject::perspective);


    GlobalMethodRegister::RegisterGlobalTable(LuaData::L,"g_camera");
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaData::L,"g_camera","GetBox",std::function<Rect()>([](){return Camera::pos;}));
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaData::L,"g_camera","GetPosition",std::function<PointInt()>([](){return Camera::pos.GetPos(); }));

    GlobalMethodRegister::RegisterGlobalTableMethod(LuaData::L,"g_camera","SetSpeed",std::function<void(float)>([](float s){ Camera::speed = s;}));
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaData::L,"g_camera","GetSpeed",std::function<float()>([](){ return Camera::speed; }));
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaData::L,"g_camera","SetSmooth",std::function<void(bool)>([](bool t){ Camera::SetSmooth( t); }));
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaData::L,"g_camera","Resize",std::function<void(Point)>([](Point newD){ Camera::Resize( newD); }));
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaData::L,"g_camera","UpdateByPos",std::function<void(Rect,int)>([](Rect r,float dt){ Camera::UpdateByPos(r,dt); }));
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaData::L,"g_camera","UnlockLimits",std::function<void()>([](){ Camera::UnlockLimits(); }));
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaData::L,"g_camera","UpdateByPos",std::function<void(float,float,float,float)>([](float x,float y,float mx,float my){ Camera::LockLimits(x,y,mx,my); }));
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaData::L,"g_camera","SetFollowRect",std::function<void(float,float,float,float)>([](float x,float y,float w,float h){ Camera::MyFollowPos = Rect(x,y,w,h); }));
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaData::L,"g_camera","SetFollowRectPos",std::function<void(float,float)>([](float x,float y){ Camera::MyFollowPos.x = x; Camera::MyFollowPos.y = y; }));
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaData::L,"g_camera","SetFollowOnRect",std::function<void(bool)>([](bool sm){ Camera::Follow(&Camera::MyFollowPos,sm); }));
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaData::L,"g_camera","SetFollowOnObject",std::function<void(GameObject *,bool)>([](GameObject *obj,bool sm){ Camera::Follow(obj,sm); }));

    GlobalMethodRegister::RegisterGlobalTable(LuaData::L,"g_input");
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaData::L,"g_input","KeyPress",std::function<bool(int)>([](int key){ return g_input.KeyPress(key); }));
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaData::L,"g_input","IsKeyDown",std::function<bool(int)>([](int key){ return g_input.IsKeyDown(key); }));
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaData::L,"g_input","KeyRelease",std::function<bool(int)>([](int key){ return g_input.KeyRelease(key); }));
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaData::L,"g_input","IsKeyUp",std::function<bool(int)>([](int key){ return g_input.IsKeyUp(key); }));
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaData::L,"g_input","IsAnyKeyPressed",std::function<bool()>([](){ return g_input.IsAnyKeyPressed(); }));
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaData::L,"g_input","GetMouse",std::function<Point()>([](){ return g_input.GetMouse(); }));
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaData::L,"g_input","MousePress",std::function<bool(int)>([](int key){ return g_input.MousePress(key); }));
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaData::L,"g_input","MouseRelease",std::function<bool(int)>([](int key){ return g_input.MouseRelease(key); }));
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaData::L,"g_input","IsMousePressed",std::function<bool(int)>([](int key){ return g_input.IsMousePressed(key); }));
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaData::L,"g_input","IsMouseReleased",std::function<bool(int)>([](int key){ return g_input.IsMouseReleased(key); }));
    //GlobalMethodRegister::RegisterGlobalTableMethod(LuaData::L,"g_input","IsMouseInside",std::function<bool(Rect)>([](Rect key){ return g_input.IsMouseInside(key); }));
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaData::L,"g_input","IsMouseInside",std::function<int(int)>([](int key){ return (int)g_input.GetKeyState(key); }));


    GlobalMethodRegister::RegisterGlobalTable(LuaData::L,"g_render");
    //g_render.DrawFillSquare(rect, red, green, blue, alpha)
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaData::L,"g_render","DrawFillSquare",std::function<void(Rect,uint8_t,uint8_t,uint8_t,uint8_t)>([](Rect rct,uint8_t r,uint8_t g,uint8_t b,uint8_t a){ RenderHelp::DrawSquareColor(rct.x,rct.y,rct.w,rct.h,r,g,b,a,false); }));
    //g_render.DrawOutlineSquare(rect, red, green, blue, alpha)
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaData::L,"g_render","DrawOutlineSquare",std::function<void(Rect,uint8_t,uint8_t,uint8_t,uint8_t)>([](Rect rct,uint8_t r,uint8_t g,uint8_t b,uint8_t a){ RenderHelp::DrawSquareColor(rct.x,rct.y,rct.w,rct.h,r,g,b,a,true); }));
    //g_render.DrawOutlineSquare(point1, point2, red, green, blue, alpha)
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaData::L,"g_render","DrawLineColor",std::function<void(Point,Point,uint8_t,uint8_t,uint8_t,uint8_t)>([](Point p1,Point p2,uint8_t r,uint8_t g,uint8_t b,uint8_t a){ RenderHelp::DrawLineColorA(p1.x,p1.y,p1.x,p2.y,r,g,b,a); }));
    //g_render.FormatARGB(red, green, blue, alpha)
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaData::L,"g_render","FormatARGB",std::function<uint32_t(uint8_t,uint8_t,uint8_t,uint8_t)>([](uint8_t r,uint8_t g,uint8_t b,uint8_t a){ return RenderHelp::FormatARGB(a,r,g,b); }));
    //g_render.FormatRGBA(red, green, blue, alpha)
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaData::L,"g_render","FormatRGBA",std::function<uint32_t(uint8_t,uint8_t,uint8_t,uint8_t)>([](uint8_t r,uint8_t g,uint8_t b,uint8_t a){ return RenderHelp::FormatRGBA(r,g,b,a); }));
    //g_render.GetR(color)
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaData::L,"g_render","GetR",std::function<uint8_t(uint32_t)>([](uint32_t c){ return RenderHelp::GetR(c); }));
    //g_render.GetG(color)
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaData::L,"g_render","GetG",std::function<uint8_t(uint32_t)>([](uint32_t c){ return RenderHelp::GetG(c); }));
    //g_render.GetB(color)
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaData::L,"g_render","GetB",std::function<uint8_t(uint32_t)>([](uint32_t c){ return RenderHelp::GetB(c); }));
    //g_render.GetA(color)
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaData::L,"g_render","GetA",std::function<uint8_t(uint32_t)>([](uint32_t c){ return RenderHelp::GetA(c); }));



    GlobalMethodRegister::RegisterGlobalTable(LuaData::L,"g_screen");
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaData::L,"g_screen","ScreenShake",std::function<void(float,float,int,float)>([](float amountX,float amountY,int frame,float duration){  ScreenManager::GetInstance().ScreenShake(Point(amountX,amountY),frame,duration); }));


	Console::GetInstance().AddTextInfo("Finished");

}


#endif
