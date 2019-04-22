#include "luainterface.hpp"
#include "luatools.hpp"


#ifndef DISABLE_LUAINTERFACE
#include "../engine/timer.hpp"
#include "../framework/schedule.hpp"
#include "../engine/renderhelp.hpp"

void LuaInterface::RegisterTimerEvents(){
    ClassRegister<Stopwatch>::RegisterClassOutside(LuaManager::L, "Stopwatch", [](lua_State* La)
    {
        return LuaReferenceCounter<Stopwatch>::makeReference(Stopwatch());
    });

    ClassRegister<Stopwatch>::RegisterClassMethod(L,"Stopwatch","Get",&Stopwatch::Get);
    ClassRegister<Stopwatch>::RegisterClassMethod(L,"Stopwatch","Reset",&Stopwatch::Reset);
    ClassRegister<Stopwatch>::RegisterClassMethod(L,"Stopwatch","Set",&Stopwatch::Set);

    ClassRegister<Timer>::RegisterClassOutside(LuaManager::L, "Timer", [](lua_State* La)
    {
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

    /*
        Register addEvent
    */

    GlobalMethodRegister::RegisterGlobalTable(LuaManager::L,"g_schedule");

    lua_getglobal(L, "g_schedule");
    lua_pushcfunction(L, &LuaInterface::AddEvent);
    lua_setfield(L, -2,  "AddEvent");
    lua_pushcfunction(L, &LuaInterface::LoopEvent);
    lua_setfield(L, -2,  "LoopEvent");
    lua_pop(L, 1);


    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_schedule","Stop",std::function<void(long int)>([](long int evId){
        g_scheduler.Stop(evId);
    }));
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_schedule","SetExecutionTime",std::function<void(long int,ExecTimeDef wher)>([](long int evId,ExecTimeDef wher){
        EventRef & ev = g_scheduler.GetEvent(evId);
        ev.SetExecTime(wher);
    }));

}


void LuaInterface::RegisterCollision(){
    GlobalMethodRegister::RegisterGlobalTable(LuaManager::L,"g_collision");
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_collision","GetCollidingObjects",std::function< std::vector<GameObject*> (GameObject*,int,bool)>([](GameObject* thisObject,int poolId,bool onlySolid)
    {
        return Collision::GetCollidingObjects(thisObject,BearEngine->GetCurrentState().Pool,poolId,onlySolid);;
    }));
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_collision","IsColliding",std::function< bool(Rect,Rect)>([](Rect r1,Rect r2)
    {
        return Collision::IsColliding(r1,r2);
    }));


    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_collision","AdjustCollisionIndependent",
    std::function<Point3(Point ,float ,GameObject* ,std::vector<GameObject*>,float,bool )>(
                     [](Point speed,float dt,GameObject* dis,std::vector<GameObject*> vec,float msize,bool onlySolid)
    {
        Point3 ret(speed.x,speed.y,0);
        ret.z = Collision::AdjustCollisionIndependent(ret.x,ret.y,dt,dis,vec,msize,onlySolid);
        return ret;
    }),true,0.2f);

    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_collision","AdjustCollision",
    std::function<Point3(Point ,float ,GameObject* ,std::vector<GameObject*> )>(
                     [](Point speed,float dt,GameObject* dis,std::vector<GameObject*> vec)
    {
        Point3 ret(speed.x,speed.y,0);
        ret.z = Collision::AdjustCollision(ret.x,ret.y,dt,dis,vec);
        return ret;
    }));


    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_collision","GetNearObjects",std::function< std::vector<GameObject*>(GameObject* thisObject,int poolId,bool onlyS)>([](GameObject* thisObject,int poolId,bool onlyS)
    {
        return Collision::GetNearObjects(thisObject,BearEngine->GetCurrentState().Pool,poolId,4,onlyS);
    }),true);

}

void LuaInterface::RegisterCamera(){
    GlobalMethodRegister::RegisterGlobalTable(LuaManager::L,"g_camera");
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_camera","Initiate",std::function<void()>([]()
    {
        Camera::Initiate(Rect(0,0,ScreenManager::GetInstance().GetScreenSize().x,ScreenManager::GetInstance().GetScreenSize().y));
    }));
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_camera","GetBox",std::function<Rect()>([]()
    {
        return Camera::pos;
    }));
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_camera","GetPosition",std::function<PointInt()>([]()
    {
        return Camera::pos.GetPos();
    }));

    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_camera","SetSpeed",std::function<void(float)>([](float s)
    {
        Camera::speed = s;
    }));
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_camera","GetSpeed",std::function<float()>([]()
    {
        return Camera::speed;
    }));
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_camera","SetSmooth",std::function<void(bool)>([](bool t)
    {
        Camera::SetSmooth( t);
    }));
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_camera","Resize",std::function<void(Point)>([](Point newD)
    {
        Camera::Resize( newD);
    }));

    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_camera","SetOffsetFollow",std::function<void(Point)>([](Point newD)
    {
        Camera::SetOffsetFollow(newD.x,newD.y);
    }));
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_camera","UpdateByPos",std::function<void(Rect,int)>([](Rect r,float dt)
    {
        Camera::UpdateByPos(r,dt);
    }));
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_camera","UnlockLimits",std::function<void()>([]()
    {
        Camera::UnlockLimits();
    }));
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_camera","UpdateByPos",std::function<void(float,float,float,float)>([](float x,float y,float mx,float my)
    {
        Camera::LockLimits(x,y,mx,my);
    }));
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_camera","SetFollowRect",std::function<void(float,float,float,float)>([](float x,float y,float w,float h)
    {
        Camera::MyFollowPos = Rect(x,y,w,h);
    }));
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_camera","SetFollowRectPos",std::function<void(float,float)>([](float x,float y)
    {
        Camera::MyFollowPos.x = x;
        Camera::MyFollowPos.y = y;
    }));
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_camera","SetFollowOnRect",std::function<void(bool)>([](bool sm)
    {
        Camera::Follow(&Camera::MyFollowPos,sm);
    }));
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_camera","SetFollowOnObject",std::function<void(GameObject *,bool)>([](GameObject *obj,bool sm)
    {
        Camera::Follow(obj,sm);
    }));
}


void LuaInterface::RegisterScreenRender(){

    GlobalMethodRegister::RegisterGlobalTable(LuaManager::L,"g_console");
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_console","Print",std::function<void(std::string)>([](std::string str)
    {
        bear::out << str << "\n";
    }));


    GlobalMethodRegister::RegisterGlobalTable(LuaManager::L,"g_render");

    //g_render.DrawSquareColor(rect, red, green, blue, alpha,mode=false)
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_render","DrawSquareColor",&RenderHelp::DrawSquareColor_lua,false,static_cast<uint8_t>(255));
    //g_render.FormatARGB(red, green, blue, alpha)
    //GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_render","DrawLineColor",&RenderHelp::DrawLineColor);
    //g_render.FormatARGB(red, green, blue, alpha)
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_render","FormatARGB",&RenderHelp::FormatARGB);
    //g_render.FormatRGBA(red, green, blue, alpha)
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_render","FormatRGBA",&RenderHelp::FormatRGBA);
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_render","GetR",&RenderHelp::GetR);
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_render","GetG",&RenderHelp::GetG);
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_render","GetB",&RenderHelp::GetB);
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_render","GetA",&RenderHelp::GetA);


    GlobalMethodRegister::RegisterGlobalTable(LuaManager::L,"g_screen");
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_screen","ScreenShake",std::function<void(float,float,int,float)>([](float amountX,float amountY,int frame,float duration)
    {
        ScreenManager::GetInstance().ScreenShake(Point(amountX,amountY),frame,duration);
    }));
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_screen","GetFps",std::function<float()>([]()
    {
        return ScreenManager::GetInstance().GetFps();
    }));
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_screen","GetScreenSize",std::function<Point()>([]()
    {
        return Point(ScreenManager::GetInstance().GetScreenSize());
    }));
}
void LuaInterface::RegisterGL(){
    GlobalMethodRegister::RegisterGlobalTable(LuaManager::L,"gl");
    //glBegin();
    //GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"gl","glBegin",&glBegin);
}


void LuaInterface::RegisterInput(){
    GlobalMethodRegister::RegisterGlobalTable(LuaManager::L,"g_input");
    GlobalMethodRegister::RegisterGlobalTableField(LuaManager::L,"g_input","SDLK_LEFT",(int)SDLK_LEFT);
    GlobalMethodRegister::RegisterGlobalTableField(LuaManager::L,"g_input","SDLK_RIGHT",(int)SDLK_RIGHT);
    GlobalMethodRegister::RegisterGlobalTableField(LuaManager::L,"g_input","SDLK_UP",(int)SDLK_UP);
    GlobalMethodRegister::RegisterGlobalTableField(LuaManager::L,"g_input","SDLK_DOWN",(int)SDLK_DOWN);
    GlobalMethodRegister::RegisterGlobalTableField(LuaManager::L,"g_input","SDLK_BACKSLASH",(int)SDLK_BACKSLASH);
    GlobalMethodRegister::RegisterGlobalTableField(LuaManager::L,"g_input","SDLK_BACKSPACE",(int)SDLK_BACKSPACE);
    GlobalMethodRegister::RegisterGlobalTableField(LuaManager::L,"g_input","SDLK_F1",(int)SDLK_F1);
    GlobalMethodRegister::RegisterGlobalTableField(LuaManager::L,"g_input","SDLK_F2",(int)SDLK_F2);
    GlobalMethodRegister::RegisterGlobalTableField(LuaManager::L,"g_input","SDLK_F3",(int)SDLK_F3);
    GlobalMethodRegister::RegisterGlobalTableField(LuaManager::L,"g_input","SDLK_F4",(int)SDLK_F4);
    GlobalMethodRegister::RegisterGlobalTableField(LuaManager::L,"g_input","SDLK_F5",(int)SDLK_F5);
    GlobalMethodRegister::RegisterGlobalTableField(LuaManager::L,"g_input","SDLK_F6",(int)SDLK_F6);
    GlobalMethodRegister::RegisterGlobalTableField(LuaManager::L,"g_input","SDLK_F7",(int)SDLK_F7);
    GlobalMethodRegister::RegisterGlobalTableField(LuaManager::L,"g_input","SDLK_F8",(int)SDLK_F8);
    GlobalMethodRegister::RegisterGlobalTableField(LuaManager::L,"g_input","SDLK_F9",(int)SDLK_F9);
    GlobalMethodRegister::RegisterGlobalTableField(LuaManager::L,"g_input","SDLK_F10",(int)SDLK_F10);
    GlobalMethodRegister::RegisterGlobalTableField(LuaManager::L,"g_input","SDLK_F11",(int)SDLK_F11);
    GlobalMethodRegister::RegisterGlobalTableField(LuaManager::L,"g_input","SDLK_F12",(int)SDLK_F12);
    GlobalMethodRegister::RegisterGlobalTableField(LuaManager::L,"g_input","SDLK_VOLUMEDOWN",(int)SDLK_VOLUMEDOWN);
    GlobalMethodRegister::RegisterGlobalTableField(LuaManager::L,"g_input","SDLK_VOLUMEUP",(int)SDLK_VOLUMEUP);
    GlobalMethodRegister::RegisterGlobalTableField(LuaManager::L,"g_input","SDLK_TAB",(int)SDLK_TAB);
    GlobalMethodRegister::RegisterGlobalTableField(LuaManager::L,"g_input","SDLK_PRINTSCREEN",(int)SDLK_PRINTSCREEN);


    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_input","GetKeyByName",std::function<int(std::string)>([](std::string kname)
    {
        return (int)SDL_GetKeyFromName(kname.c_str());
    }));
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_input","KeyPress",std::function<bool(int)>([](int key)
    {
        return g_input.KeyPress(key);
    }));
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_input","IsKeyDown",std::function<bool(int)>([](int key)
    {
        return g_input.IsKeyDown(key);
    }));
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_input","KeyRelease",std::function<bool(int)>([](int key)
    {
        return g_input.KeyRelease(key);
    }));
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_input","IsKeyUp",std::function<bool(int)>([](int key)
    {
        return g_input.IsKeyUp(key);
    }));
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_input","IsAnyKeyPressed",std::function<bool()>([]()
    {
        return g_input.IsAnyKeyPressed() != -1;
    }));
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_input","GetPressedKey",std::function<int()>([]()
    {
        return g_input.IsAnyKeyPressed();
    }));
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_input","GetMouse",std::function<Point()>([]()
    {
        return g_input.GetMouse();
    }));
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_input","MousePress",std::function<bool(int)>([](int key)
    {
        return g_input.MousePress(key);
    }));
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_input","MouseRelease",std::function<bool(int)>([](int key)
    {
        return g_input.MouseRelease(key);
    }));
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_input","IsMousePressed",std::function<bool(int)>([](int key)
    {
        return g_input.IsMousePressed(key);
    }));
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_input","IsMouseReleased",std::function<bool(int)>([](int key)
    {
        return g_input.IsMouseReleased(key);
    }));
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_input","IsMouseInside",std::function<bool(Rect)>([](Rect key)
    {
        return g_input.IsMouseInside(key);
    }));
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_input","GetKeyState",std::function<int(int)>([](int key)
    {
        return (int)g_input.GetKeyState(key);
    }));
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_input","GetClipboard",std::function<std::string()>([](){
        return g_input.GetClipboard();
    }));
    GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_input","HaltInput",std::function<void()>([]()
    {
        g_input.HaltInput();
    }));
}


#endif // DISABLE_LUAINTERFACE
