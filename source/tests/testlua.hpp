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
            ScreenManager::GetInstance().SetScreenName("Test lua");
            LuaInterface::Instance().Startup();
            /*
                On normal cases, the engine would register the current
                state by itself. But as we're post starting, the register
                should be called right the way!
            */

            GlobalMethodRegister::RegisterGlobalTable(LuaManager::L,"g_test");
            GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_test","showInt",std::function<void(int)>([](int a){ bear::out << "Int is: " << a << "\n";}));
            GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_test","Sum",std::function<int(int,int)>([](int a,int b){return a+ b;}));

            GlobalMethodRegister::RegisterGlobalTableMethod(LuaManager::L,"g_test","testOptional",
            std::function<int(std::vector<int>,int,float,float )>([](std::vector<int>kek ,int a,float b,float c){
                for (auto &it : kek){
                    std::cout << "Table member: " << it << "\n";
                }
                std::cout << "Arguments:" << a << ", "<<b<<", "<<c<<"\n";
                return 1;
            }),13.37f,133.7f,13337);

            LuaCaller::StartupState(LuaManager::L,this,nullptr);
            bear::out << "Load file lua/test.lua\n";
            LuaCaller::LoadFile(LuaManager::L,"lua/test.lua");
            LuaCaller::Pcall(LuaManager::L);
            bear::out << "Call onLoad\n";



            LuaCaller::CallGlobalField(LuaManager::L,"onLoad");


        };

        void Update(float dt){
            duration -= dt;

            if( InputManager::GetInstance().IsAnyKeyPressed() != -1 || duration <= 0 ) {
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
