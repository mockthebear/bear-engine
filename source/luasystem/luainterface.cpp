#include "luainterface.hpp"
#include "luatools.hpp"
#include "../performance/console.hpp"
#include "luagamestate.hpp"
#include "luaui.hpp"






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
    if (L != NULL){
        Close();
    }
}





void LuaInterface::Close(){
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
        L = nullptr;
    //
    bear::out << "[Close] Lua is closed.\n";
}
void LuaInterface::Startup(){
    L = luaL_newstate();
    if (L == NULL){
        bear::out << "Cannot start lua\n";
    }else{
        Console::GetInstance().AddTextInfo("Lua started.");
        LuaManager::L = L;
        luaL_openlibs(L);


		Console::GetInstance().AddTextInfo("Register classes...");
        RegisterClasses();


        if ( luaL_loadfile(L, "lua/main.lua")==0 ) {
        // execute Lua program
           if (lua_pcall(L, 0, LUA_MULTRET, 0) != 0){

                bear::out << "Lua error: " << lua_tostring(L, -1) << "\n";
                lua_pop(L, 1); // remove error message
            }
        }else{
             bear::out << "Lua error: " << lua_tostring(L, -1) << "\n";
             lua_pop(L, 1); // remove error message
        }
        bear::out << "[lua] Lua started!" << "\n";
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
    //LuaCaller::CallClear(L);
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
	LuaCaller::Startup(L);

	ClassRegister<Stopwatch>::RegisterClassOutside(LuaManager::L, "Stopwatch", [](lua_State* L) {
		return LuaReferenceCounter<Stopwatch>::makeReference(Stopwatch());
	});

    ClassRegister<Stopwatch>::RegisterClassMethod(L,"Stopwatch","Get",&Stopwatch::Get);
    ClassRegister<Stopwatch>::RegisterClassMethod(L,"Stopwatch","Reset",&Stopwatch::Reset);
    ClassRegister<Stopwatch>::RegisterClassMethod(L,"Stopwatch","Set",&Stopwatch::Set);

    ClassRegister<Timer>::RegisterClassOutside(LuaManager::L, "Timer", [](lua_State* L) {
		return LuaReferenceCounter<Timer>::makeReference(Timer());
	});

    ClassRegister<Timer>::RegisterClassMethod(L,"Timer","SetDuration",&Timer::SetDuration);
    ClassRegister<Timer>::RegisterClassMethod(L,"Timer","Set",&Timer::SetDuration);
    ClassRegister<Timer>::RegisterClassMethod(L,"Timer","Restart",&Timer::Restart);
    ClassRegister<Timer>::RegisterClassMethod(L,"Timer","Reset",&Timer::Restart);
    ClassRegister<Timer>::RegisterClassMethod(L,"Timer","Start",&Timer::Restart);
    ClassRegister<Timer>::RegisterClassMethod(L,"Timer","Get",&Timer::Get);
    ClassRegister<Timer>::RegisterClassMethod(L,"Timer","Disable",&Timer::Disable);
    ClassRegister<Timer>::RegisterClassMethod(L,"Timer","Enable",&Timer::Enable);
    ClassRegister<Timer>::RegisterClassMethod(L,"Timer","IsEnabled",&Timer::IsEnabled);
    ClassRegister<Timer>::RegisterClassMethod(L,"Timer","HasFinished",&Timer::HasFinished);
    ClassRegister<Timer>::RegisterClassMethod(L,"Timer","Update",&Timer::Update);

    ClassRegister<Timer>::RegisterClassMethod(L,"Timer","GetDuration",&Timer::GetDuration);

    TypeObserver<Timer,float>::RegisterMethod(LuaManager::L,"m_time",&Timer::m_time);



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




    ClassRegister<ColorReplacer>::RegisterClassOutside(LuaManager::L,"ColorReplacer",[](lua_State* L){
        return LuaReferenceCounter<ColorReplacer>::makeReference(ColorReplacer());
    });
    ClassRegister<ColorReplacer>::RegisterClassMethod(L,"ColorReplacer","clear",&ColorReplacer::clear);
    ClassRegister<ColorReplacer>::RegisterClassMethod(L,"ColorReplacer","AddReplacer",&ColorReplacer::AddReplacer);
    ClassRegister<ColorReplacer>::RegisterClassMethod(L,"ColorReplacer","Get",&ColorReplacer::Get);

    ClassRegister<Text>::RegisterClassOutside(LuaManager::L,"Text",[](lua_State* L){
        std::string name;
        int size = 12;
        SDL_Color color = {100,100,100,255};
        if (lua_gettop(L) >= 4){
            color = GenericLuaGetter<SDL_Color>::Call(L);
        }
        if (lua_gettop(L) >= 3){
            size = GenericLuaGetter<int>::Call(L);
        }

        name = GenericLuaGetter<std::string>::Call(L);

        Text *t = LuaReferenceCounter<Text>::makeReference(Text(name,size,color));
        t->RemakeTexture();
        return t;
    });
    ClassRegister<Text>::RegisterClassMethod(L,"Text","Close",&Text::Close);
    ClassRegister<Text>::RegisterClassMethod(L,"Text","SetText",&Text::SetText);
    ClassRegister<Text>::RegisterClassMethod(L,"Text","SetColor",&Text::SetColor);
    ClassRegister<Text>::RegisterClassMethod(L,"Text","SetStyle",&Text::SetStyle);
    ClassRegister<Text>::RegisterClassMethod(L,"Text","SetAlpha",&Text::SetAlpha);
    ClassRegister<Text>::RegisterClassMethod(L,"Text","GetText",&Text::GetText);
    ClassRegister<Text>::RegisterClassMethod(L,"Text","GetHeight",&Text::GetHeight);
    ClassRegister<Text>::RegisterClassMethod(L,"Text","GetWidth",&Text::GetWidth);
    ClassRegister<Text>::RegisterClassMethod(L,"Text","SetScaleX",&Text::SetScaleX);
    ClassRegister<Text>::RegisterClassMethod(L,"Text","SetScaleY",&Text::SetScaleY);
    ClassRegister<Text>::RegisterClassMethod(L,"Text","RemakeTexture",&Text::RemakeTexture);
    ClassRegister<Text>::RegisterClassMethod(L,"Text","IsWorking",&Text::IsWorking);
    ClassRegister<Text>::RegisterClassMethod(L,"Text","SetRotation",&Text::SetRotation);
    ClassRegister<Text>::RegisterClassMethod(L,"Text","Render",&Text::RenderLua);
    ClassRegister<Text>::RegisterClassMethod(L,"Text","SetAliasign",&Text::SetAliasign);
    ClassRegister<Text>::RegisterClassMethod(L,"Text","SetFont",&Text::SetFont);



    ClassRegister<Sprite>::RegisterClassOutside(LuaManager::L,"Sprite",[](lua_State* L){
        std::string name;
        if (lua_gettop(L) == 3){
            ColorReplacer r = GenericLuaGetter<ColorReplacer>::Call(L);
            name = GenericLuaGetter<std::string>::Call(L);
            Sprite *t = LuaReferenceCounter<Sprite>::makeReference(Game::GetCurrentState().Assets.make<Sprite>(name,r));
            return t;
        }else{
            name = GenericLuaGetter<std::string>::Call(L);
        }
        Sprite *t = LuaReferenceCounter<Sprite>::makeReference(Game::GetCurrentState().Assets.make<Sprite>(name));
        return t;
    });

    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","Open",&Sprite::Openf);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","SetClip",&Sprite::SetClip);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","Render",&Sprite::Renderpoint);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","RawRender",&Sprite::RawRender);
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
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","SetCenter",&Sprite::SetCenter);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","GetCenter",&Sprite::GetCenter);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","SetRepeatTimes",&Sprite::SetRepeatTimes);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","GetRepeatTimes",&Sprite::GetRepeatTimes);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","GetCurrentFrame",&Sprite::GetCurrentFrame);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","IsAnimationOver",&Sprite::IsAnimationOver);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","SetScaleX",&Sprite::SetScaleX);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","SetScaleY",&Sprite::SetScaleY);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","SetScale",&Sprite::SetScale);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","GetScale",&Sprite::GetScale);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","ReBlend",&Sprite::ReBlend);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","SetAlpha",&Sprite::SetAlpha);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","GetAlpha",&Sprite::GetAlpha);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","SetGrid",&Sprite::SetGrid);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","GetGrid",&Sprite::GetGrid);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","SetFlip",&Sprite::SetFlip);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","GetFlip",&Sprite::GetFlip);
    ClassRegister<Sprite>::RegisterClassMethod(L,"Sprite","GetMe",&Sprite::GetMe);

    ClassRegister<CustomFont>::RegisterClassOutside(LuaManager::L,"CustomFont",[](lua_State* L){
        std::string name = GenericLuaGetter<std::string>::Call(L);
        CustomFont *t = LuaReferenceCounter<CustomFont>::makeReference(new CustomFont(name));
        return t;
    });

    ClassRegister<CustomFont>::RegisterClassMethod(L,"CustomFont","Render",&CustomFont::Renderl);
    ClassRegister<CustomFont>::RegisterClassMethod(L,"CustomFont","GetSizes",&CustomFont::GetSizes);
    ClassRegister<CustomFont>::RegisterClassMethod(L,"CustomFont","SetSprite",&CustomFont::SetSprite);

    TypeObserver<CustomFont,bool>::RegisterMethod(LuaManager::L,"loaded",&CustomFont::loaded);





    static LuaCFunctionLambda GarbageCollectorFunction = [](lua_State* L){
        std::cout << "Calling gc\n";
        return 1;
    };
    ClassRegister<LuaObject>::RegisterClassOutside(LuaManager::L,"LuaObject",[](lua_State* L){
        int y = GenericLuaGetter<int>::Call(L);
        int x = GenericLuaGetter<int>::Call(L);
        LuaObject *t = (LuaObject*)BearEngine->GetCurrentState().Pool.AddInstance(LuaObject(x ,y));
        return t;
    });

    ClassRegister<LuaObject>::RegisterClassMethod(LuaManager::L,"LuaObject","Destroy",&LuaObject::Destroy);
    ClassRegister<LuaObject>::RegisterClassMethod(LuaManager::L,"LuaObject","SetX",&LuaObject::SetX);
    ClassRegister<LuaObject>::RegisterClassMethod(LuaManager::L,"LuaObject","SetY",&LuaObject::SetY);
    ClassRegister<LuaObject>::RegisterClassMethod(LuaManager::L,"LuaObject","GetY",&LuaObject::GetY);
    ClassRegister<LuaObject>::RegisterClassMethod(LuaManager::L,"LuaObject","GetX",&LuaObject::GetX);
    ClassRegister<LuaObject>::RegisterClassMethod(LuaManager::L,"LuaObject","SetWidth",&LuaObject::SetWidth);
    ClassRegister<LuaObject>::RegisterClassMethod(LuaManager::L,"LuaObject","GetWidth",&LuaObject::GetWidth);
    ClassRegister<LuaObject>::RegisterClassMethod(LuaManager::L,"LuaObject","SetHeight",&LuaObject::SetHeight);
    ClassRegister<LuaObject>::RegisterClassMethod(LuaManager::L,"LuaObject","GetHeight",&LuaObject::GetHeight);
    ClassRegister<LuaObject>::RegisterClassMethod(LuaManager::L,"LuaObject","SetBox",&LuaObject::SetBox);
    ClassRegister<LuaObject>::RegisterClassMethod(LuaManager::L,"LuaObject","GetBox",&LuaObject::GetBox);
    ClassRegister<LuaObject>::RegisterClassMethod(LuaManager::L,"LuaObject","GetPoolIndex",&LuaObject::GetPoolIndex);
    ClassRegister<LuaObject>::RegisterClassMethod(LuaManager::L,"LuaObject","GetMyRef",&LuaObject::GetMyRef);
    ClassRegister<LuaObject>::RegisterClassMethod(LuaManager::L,"LuaObject","GetMyObj",&LuaObject::GetMyObj);

    ClassRegister<GameObject>::RegisterClassMethod(LuaManager::L,"LuaObject","hasPerspective",&GameObject::hasPerspective);

    TypeObserver<LuaObject,bool>::RegisterMethod(LuaManager::L,"forceUpdate",&LuaObject::forceUpdate);
    TypeObserver<LuaObject,bool>::RegisterMethod(LuaManager::L,"forceRender",&LuaObject::forceRender);
    TypeObserver<LuaObject,int>::RegisterMethod(LuaManager::L,"perspective",&LuaObject::perspective);

    ClassRegister<LuaGameState>::RegisterClassOutside(LuaManager::L,"LuaGameState",[](lua_State* L){
        LuaGameState *t = new LuaGameState();
        return t;
    });

    ClassRegister<LuaGameState>::RegisterClassMethod(LuaManager::L,"LuaGameState","Setup",&LuaGameState::Setup);

    TypeObserver<LuaGameState,bool>::RegisterMethod(LuaManager::L,"canClose",&LuaGameState::canClose);


    ClassRegister<GameObject>::RegisterClassVirtual(LuaManager::L,"GameObject",[](lua_State* L){
        uint64_t y = GenericLuaGetter<uint64_t>::Call(L);
        GameObject *t = (GameObject*)y;
        return t;
    },&GarbageCollectorFunction);


    ClassRegister<GameObject>::RegisterClassMethod(LuaManager::L,"GameObject","Is",&GameObject::Is);
    ClassRegister<GameObject>::RegisterClassMethod(LuaManager::L,"GameObject","GetMyRef",&GameObject::GetMyRef);
    ClassRegister<GameObject>::RegisterClassMethod(LuaManager::L,"GameObject","SetX",&GameObject::SetX);
    ClassRegister<GameObject>::RegisterClassMethod(LuaManager::L,"GameObject","SetY",&GameObject::SetY);
    ClassRegister<GameObject>::RegisterClassMethod(LuaManager::L,"GameObject","GetY",&GameObject::GetY);
    ClassRegister<GameObject>::RegisterClassMethod(LuaManager::L,"GameObject","GetX",&GameObject::GetX);
    ClassRegister<GameObject>::RegisterClassMethod(LuaManager::L,"GameObject","SetWidth",&GameObject::SetWidth);
    ClassRegister<GameObject>::RegisterClassMethod(LuaManager::L,"GameObject","GetWidth",&GameObject::GetWidth);
    ClassRegister<GameObject>::RegisterClassMethod(LuaManager::L,"GameObject","SetHeight",&GameObject::SetHeight);
    ClassRegister<GameObject>::RegisterClassMethod(LuaManager::L,"GameObject","GetHeight",&GameObject::GetHeight);
    ClassRegister<GameObject>::RegisterClassMethod(LuaManager::L,"GameObject","SetBox",&GameObject::SetBox);
    ClassRegister<GameObject>::RegisterClassMethod(LuaManager::L,"GameObject","GetBox",&GameObject::GetBox);
    ClassRegister<GameObject>::RegisterClassMethod(LuaManager::L,"GameObject","GetBox",&GameObject::GetBox);
    ClassRegister<GameObject>::RegisterClassMethod(LuaManager::L,"GameObject","GetSolid",&GameObject::GetSolid);
    ClassRegister<GameObject>::RegisterClassMethod(LuaManager::L,"GameObject","SetSolid",&GameObject::SetSolid);

    TypeObserver<GameObject,bool>::RegisterMethod(LuaManager::L,"solid",&GameObject::solid);




    GlobalMethodRegister::RegisterGlobalTable(LuaManager::L,"g_console");
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_console","Print",std::function<void(std::string)>([](std::string str){ bear::out << "[Lua]: "<< str << "\n"; }));


    GlobalMethodRegister::RegisterGlobalTable(LuaManager::L,"g_collision");
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_collision","GetCollidingObjects",std::function< std::vector<GameObject*> (GameObject*,int,bool)>([](GameObject* thisObject,int poolId,bool onlySolid){ return Collision::GetCollidingObjects(thisObject,BearEngine->GetCurrentState().Pool,poolId,onlySolid);; }));
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_collision","IsColliding",std::function< bool(Rect,Rect)>([](Rect r1,Rect r2){ return Collision::IsColliding(r1,r2); }));



    GlobalMethodRegister::RegisterGlobalTable(LuaManager::L,"g_camera");
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_camera","GetBox",std::function<Rect()>([](){return Camera::pos;}));
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_camera","GetPosition",std::function<PointInt()>([](){return Camera::pos.GetPos(); }));

    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_camera","SetSpeed",std::function<void(float)>([](float s){ Camera::speed = s;}));
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_camera","GetSpeed",std::function<float()>([](){ return Camera::speed; }));
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_camera","SetSmooth",std::function<void(bool)>([](bool t){ Camera::SetSmooth( t); }));
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_camera","Resize",std::function<void(Point)>([](Point newD){ Camera::Resize( newD); }));
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_camera","UpdateByPos",std::function<void(Rect,int)>([](Rect r,float dt){ Camera::UpdateByPos(r,dt); }));
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_camera","UnlockLimits",std::function<void()>([](){ Camera::UnlockLimits(); }));
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_camera","UpdateByPos",std::function<void(float,float,float,float)>([](float x,float y,float mx,float my){ Camera::LockLimits(x,y,mx,my); }));
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_camera","SetFollowRect",std::function<void(float,float,float,float)>([](float x,float y,float w,float h){ Camera::MyFollowPos = Rect(x,y,w,h); }));
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_camera","SetFollowRectPos",std::function<void(float,float)>([](float x,float y){ Camera::MyFollowPos.x = x; Camera::MyFollowPos.y = y; }));
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_camera","SetFollowOnRect",std::function<void(bool)>([](bool sm){ Camera::Follow(&Camera::MyFollowPos,sm); }));
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_camera","SetFollowOnObject",std::function<void(GameObject *,bool)>([](GameObject *obj,bool sm){ Camera::Follow(obj,sm); }));

    GlobalMethodRegister::RegisterGlobalTable(LuaManager::L,"g_input");
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_input","KeyPress",std::function<bool(int)>([](int key){ return g_input.KeyPress(key); }));
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_input","IsKeyDown",std::function<bool(int)>([](int key){ return g_input.IsKeyDown(key); }));
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_input","KeyRelease",std::function<bool(int)>([](int key){ return g_input.KeyRelease(key); }));
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_input","IsKeyUp",std::function<bool(int)>([](int key){ return g_input.IsKeyUp(key); }));
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_input","IsAnyKeyPressed",std::function<bool()>([](){ return g_input.IsAnyKeyPressed(); }));
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_input","GetMouse",std::function<Point()>([](){ return g_input.GetMouse(); }));
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_input","MousePress",std::function<bool(int)>([](int key){ return g_input.MousePress(key); }));
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_input","MouseRelease",std::function<bool(int)>([](int key){ return g_input.MouseRelease(key); }));
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_input","IsMousePressed",std::function<bool(int)>([](int key){ return g_input.IsMousePressed(key); }));
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_input","IsMouseReleased",std::function<bool(int)>([](int key){ return g_input.IsMouseReleased(key); }));
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_input","IsMouseInside",std::function<bool(Rect)>([](Rect key){ return g_input.IsMouseInside(key); }));
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_input","GetKeyState",std::function<int(int)>([](int key){ return (int)g_input.GetKeyState(key); }));
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_input","HaltInput",std::function<void()>([](){ g_input.HaltInput(); }));


    GlobalMethodRegister::RegisterGlobalTable(LuaManager::L,"g_render");
    //g_render.DrawFillSquare(rect, red, green, blue, alpha)
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_render","DrawFillSquare",std::function<void(Rect,uint8_t,uint8_t,uint8_t,uint8_t)>([](Rect rct,uint8_t r,uint8_t g,uint8_t b,uint8_t a){ RenderHelp::DrawSquareColor(rct.x,rct.y,rct.w,rct.h,r,g,b,a,false); }));
    //g_render.DrawOutlineSquare(rect, red, green, blue, alpha)
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_render","DrawOutlineSquare",std::function<void(Rect,uint8_t,uint8_t,uint8_t,uint8_t)>([](Rect rct,uint8_t r,uint8_t g,uint8_t b,uint8_t a){ RenderHelp::DrawSquareColor(rct.x,rct.y,rct.w,rct.h,r,g,b,a,true); }));
    //g_render.DrawOutlineSquare(point1, point2, red, green, blue, alpha)
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_render","DrawLineColor",std::function<void(Point,Point,uint8_t,uint8_t,uint8_t,uint8_t)>([](Point p1,Point p2,uint8_t r,uint8_t g,uint8_t b,uint8_t a){
                                                                                                                                                              RenderHelp::DrawLineColorA(p1.x,p1.y,p2.x,p2.y,r,g,b,a); }));
    //g_render.FormatARGB(red, green, blue, alpha)
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_render","FormatARGB",std::function<uint32_t(uint8_t,uint8_t,uint8_t,uint8_t)>([](uint8_t r,uint8_t g,uint8_t b,uint8_t a){ return RenderHelp::FormatARGB(a,r,g,b); }));
    //g_render.FormatRGBA(red, green, blue, alpha)
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_render","FormatRGBA",std::function<uint32_t(uint8_t,uint8_t,uint8_t,uint8_t)>([](uint8_t r,uint8_t g,uint8_t b,uint8_t a){ return RenderHelp::FormatRGBA(r,g,b,a); }));
    //g_render.GetR(color)
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_render","GetR",std::function<uint8_t(uint32_t)>([](uint32_t c){ return RenderHelp::GetR(c); }));
    //g_render.GetG(color)
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_render","GetG",std::function<uint8_t(uint32_t)>([](uint32_t c){ return RenderHelp::GetG(c); }));
    //g_render.GetB(color)
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_render","GetB",std::function<uint8_t(uint32_t)>([](uint32_t c){ return RenderHelp::GetB(c); }));
    //g_render.GetA(color)
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_render","GetA",std::function<uint8_t(uint32_t)>([](uint32_t c){ return RenderHelp::GetA(c); }));



    GlobalMethodRegister::RegisterGlobalTable(LuaManager::L,"g_screen");
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_screen","ScreenShake",std::function<void(float,float,int,float)>([](float amountX,float amountY,int frame,float duration){  ScreenManager::GetInstance().ScreenShake(Point(amountX,amountY),frame,duration); }));
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_screen","GetFps",std::function<float()>([](){  return ScreenManager::GetInstance().GetFps(); }));
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_screen","GetScreenSize",std::function<Point()>([](){  return Point(ScreenManager::GetInstance().GetScreenSize()); }));



    static LuaCFunctionLambda DeleteGCF = [](lua_State* L){
        std::cout << "Calling gc to delete\n";
        return 1;
    };
    ClassRegister<LuaUi>::RegisterClassOutside(LuaManager::L,"LuaUi",[](lua_State* L){
        LuaUi *t = new LuaUi();
        //BearEngine->GetCurrentState().AddWindow(t);
        return t;
    },nullptr);

    GlobalMethodRegister::RegisterGlobalTable(LuaManager::L,"g_ui");
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_ui","AddWindow",std::function<void(LuaUi *)>([](LuaUi *ui){ BearEngine->GetCurrentState().AddWindow(ui); }));


    ClassRegister<LuaUi>::RegisterClassMethod(LuaManager::L,"LuaUi","SetFocused",&LuaUi::SetFocused);
    ClassRegister<LuaUi>::RegisterClassMethod(LuaManager::L,"LuaUi","SetAlpha",&LuaUi::SetAlpha);
    ClassRegister<LuaUi>::RegisterClassMethod(LuaManager::L,"LuaUi","Close",&LuaUi::Close);
    ClassRegister<LuaUi>::RegisterClassMethod(LuaManager::L,"LuaUi","SetPosition",&LuaUi::SetPosition);
    ClassRegister<LuaUi>::RegisterClassMethod(LuaManager::L,"LuaUi","SetText",&LuaUi::SetTextStr);
    ClassRegister<LuaUi>::RegisterClassMethod(LuaManager::L,"LuaUi","SetTextObj",&LuaUi::SetTextObj);
    ClassRegister<LuaUi>::RegisterClassMethod(LuaManager::L,"LuaUi","GetText",&LuaUi::GetText);
    ClassRegister<LuaUi>::RegisterClassMethod(LuaManager::L,"LuaUi","IsDead",&LuaUi::IsDead);
    ClassRegister<LuaUi>::RegisterClassMethod(LuaManager::L,"LuaUi","Is",&LuaUi::Is);
    ClassRegister<LuaUi>::RegisterClassMethod(LuaManager::L,"LuaUi","IsInside",&LuaUi::IsInside);
    ClassRegister<LuaUi>::RegisterClassMethod(LuaManager::L,"LuaUi","Hide",&LuaUi::Hide);
    ClassRegister<LuaUi>::RegisterClassMethod(LuaManager::L,"LuaUi","Show",&LuaUi::Show);
    ClassRegister<LuaUi>::RegisterClassMethod(LuaManager::L,"LuaUi","Refresh",&LuaUi::Refresh);
    ClassRegister<LuaUi>::RegisterClassMethod(LuaManager::L,"LuaUi","SetFocused",&LuaUi::SetFocused);
    ClassRegister<LuaUi>::RegisterClassMethod(LuaManager::L,"LuaUi","AddComponent",&LuaUi::AddComponent2);

    ClassRegister<LuaUi>::RegisterClassMethod(LuaManager::L,"LuaUi","GetX",&LuaUi::GetX);
    ClassRegister<LuaUi>::RegisterClassMethod(LuaManager::L,"LuaUi","GetY",&LuaUi::GetY);

    ClassRegister<LuaUi>::RegisterClassMethod(LuaManager::L,"LuaUi","GetTrueX",&LuaUi::GetTrueX);
    ClassRegister<LuaUi>::RegisterClassMethod(LuaManager::L,"LuaUi","GetTrueY",&LuaUi::GetTrueY);


    ClassRegister<LuaUi>::RegisterClassMethod(LuaManager::L,"LuaUi","SetX",&LuaUi::SetX);
    ClassRegister<LuaUi>::RegisterClassMethod(LuaManager::L,"LuaUi","SetY",&LuaUi::SetY);

    ClassRegister<LuaUi>::RegisterClassMethod(LuaManager::L,"LuaUi","GetScreenY",&LuaUi::GetScreenY);
    ClassRegister<LuaUi>::RegisterClassMethod(LuaManager::L,"LuaUi","GetScreenX",&LuaUi::GetScreenX);

    ClassRegister<LuaUi>::RegisterClassMethod(LuaManager::L,"LuaUi","SetWidth",&LuaUi::SetWidth);
    ClassRegister<LuaUi>::RegisterClassMethod(LuaManager::L,"LuaUi","GetWidth",&LuaUi::GetWidth);
    ClassRegister<LuaUi>::RegisterClassMethod(LuaManager::L,"LuaUi","SetHeight",&LuaUi::SetHeight);
    ClassRegister<LuaUi>::RegisterClassMethod(LuaManager::L,"LuaUi","GetHeight",&LuaUi::GetHeight);

    ClassRegister<LuaUi>::RegisterClassMethod(LuaManager::L,"LuaUi","SetId",&LuaUi::SetId);
    ClassRegister<LuaUi>::RegisterClassMethod(LuaManager::L,"LuaUi","GetId",&LuaUi::GetId);

    ClassRegister<LuaUi>::RegisterClassMethod(LuaManager::L,"LuaUi","GetLastComponent",&LuaUi::GetLastComponent_Lua);
    ClassRegister<LuaUi>::RegisterClassMethod(LuaManager::L,"LuaUi","GetChildById",&LuaUi::GetChildById_Lua);
    ClassRegister<LuaUi>::RegisterClassMethod(LuaManager::L,"LuaUi","GetMother",&LuaUi::GetMother);
    ClassRegister<LuaUi>::RegisterClassMethod(LuaManager::L,"LuaUi","GetChilds",&LuaUi::GetChilds);
    ClassRegister<LuaUi>::RegisterClassMethod(LuaManager::L,"LuaUi","SetAsMain",&LuaUi::SetAsMain);


}


#endif
