#include "../settings/definitions.hpp"
#include "../engine/genericstate.hpp"


#ifndef DISABLE_LUAINTERFACE
#ifndef LUATESTSUITE
#define LUATESTSUITE


#include "../luasystem/luatools.hpp"
#include "../luasystem/luainterface.hpp"

class Test_Lua: public State{
    public:
        Test_Lua(){
            requestQuit = requestDelete = false;
            duration = 1.0f;
        };
        ~Test_Lua(){

        };
        void Begin(){
            bear::out << "Start lua\n";
            LuaInterface::Instance().Startup();
            /*
                On normal cases, the engine would register the current
                state by itself. But as we're post starting, the register
                should be called right the way!
            */
            LuaCaller::StartupState(LuaManager::L,this,nullptr);

            bear::out << "Load file lua/test.lua\n";
            LuaCaller::LoadFile(LuaManager::L,"lua/test.lua");
            LuaCaller::Pcall(LuaManager::L);
            bear::out << "Call onLoad\n";
            LuaCaller::CallGlobalField(LuaManager::L,"onLoad");


        };

        void Update(float dt){
            duration -= dt;

            if( InputManager::GetInstance().IsAnyKeyPressed() || duration <= 0 ) {
                bear::out << "Call new state\n";
                LuaCaller::CallGlobalField(LuaManager::L,"MakeLuaState");

                requestDelete = true;
            }

        };
        void Render(){

        };
        void Input();
        void Resume(){};
        void End(){};
    private:
        float duration;
};

#endif // LUATESTSUITE
#endif // DISABLE_LUAINTERFACE
