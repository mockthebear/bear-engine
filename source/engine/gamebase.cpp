#include "gamebase.hpp"
#include <ctime>
#include <time.h>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif
#include <csignal>
#include <signal.h>
#include "../luasystem/luainterface.hpp"
#include "../framework/threadpool.hpp"
#include "../framework/debughelper.hpp"
#include "../framework/resourcemanager.hpp"
#include "../framework/schedule.hpp"
#include "parallelcollisionmanager.hpp"
#include "../settings/definitions.hpp"
#include "../performance/console.hpp"
#include "../sound/soundsources.hpp"
#include "../sound/soundloader.hpp"
#ifndef DISABLE_SOCKET
#include "../socket/socketdef.hpp"
#endif // DISABLE_SOCKET
#include "timer.hpp"
#include "../crashhandler/crashhandler.hpp"
#include __BEHAVIOR_FOLDER__

#define BEAR_VERSION "4.3"

#ifndef __EMSCRIPTEN__
void exitHandler(int sig)
{
    static bool signaled = false;
    switch(sig) {
        case SIGTERM:
        case SIGINT:
            if(!signaled ) {
                bear::out << "Received signal to close\n";
                signaled = true;
                Game::GetInstance()->isClosing = true;
            }
            break;
        #ifndef __linux__
        case SIGBREAK:
            bear::out << "Received signal to break\n";
            Game::Crashed = true;
            Game::GetInstance()->isClosing = true;
            break;
        #endif // __linux__
    }
}
#endif // __EMSCRIPTEN__


Game g_game;
Game* Game::instance = NULL;

Game::Game(){isClosing=SDLStarted=canDebug=GameBegin=hasBeenClosed=HasAudio=false;};

uint32_t Game::startFlags = BEAR_FLAG_START_EVERYTHING;

bool Game::Crashed = false;


void Game::init(const char *name){
    if (instance == NULL){


        SDLStarted = false;
        Started = false;
        isClosing = false;
        hasBeenClosed = false;
        instance = this;
        HasAudio = true;
        wasLocked = false;
        GameBegin = false;
        dt = frameStart = 0;
        canDebug = false;



        if (!GameBehavior::GetInstance().Begin()){
            return;
        }
        #ifndef __EMSCRIPTEN__
        signal(SIGINT, exitHandler);
        signal(SIGTERM, exitHandler);
        #ifndef __linux__
        signal(SIGBREAK, exitHandler);
        #endif // __linux__
        #ifdef CRASH_HANDLER
        installCrashHandler();
        #endif
        #endif // __EMSCRIPTEN__

        if (startFlags&BEAR_FLAG_START_CONSOLE){
            Console::GetInstance(true);
            Console::GetInstance().AddTextInfo("Starting...");
        }

        DisplayBearInfo();

        if (startFlags&BEAR_FLAG_START_SDL){
            if (SDL_Init(BEAR_SDL_CONST_INIT) != 0){
                Console::GetInstance().AddTextInfo( utils::format("SDL may nor work because [%s]",SDL_GetError()) );
            }else{
                Console::GetInstance().AddTextInfo("SDL is on!");
            }
        }

        if (startFlags&BEAR_FLAG_START_TTF){
            if (TTF_Init()  == -1 ){
                Console::GetInstance().AddTextInfo("TTF not working");
            }else{
                Console::GetInstance().AddTextInfo("TTF is on!");
            }
        }
        #ifndef DISABLE_SOCKET
        if (startFlags&BEAR_FLAG_START_SOCKET){
            if (!BaseSocket::StartSocket()){
                Console::GetInstance().AddTextInfo("Failed to create socket context");
            }
        }
        #endif // DISABLE_SOCKET

        if (startFlags&BEAR_FLAG_START_SOUND){
            ConfigManager::GetInstance().SetSound(HasAudio);
        }
        GameBehavior::GetInstance();
        if (startFlags&BEAR_FLAG_START_SCREEN){
            window = ScreenManager::GetInstance().StartScreen(name);
            if (window == NULL){
                Console::GetInstance().AddTextInfo("Failed creating screen");
                return;
            }else{
                Console::GetInstance().AddTextInfo("Screen is on!");
            }
            #ifndef RENDER_OPENGL

            renderer = ScreenManager::GetInstance().StartRenderer();

            if (!renderer){
                Console::GetInstance().AddTextInfo("Failed creating render");
                exit(1);
            }else{
                Console::GetInstance().AddTextInfo("Renderer is on!");
            }
            #else
            Console::GetInstance().AddTextInfo("Starting openGL");
            ScreenManager::GetInstance().SetupOpenGL();
            #endif // RENDER_OPENGL

        }
        if (startFlags&BEAR_FLAG_LOAD_BASEFILES){
            bear::out << "Loading basefiles\n";
            if (!ResourceManager::GetInstance().Load("engine/enginedata.burr","engine")){
                Console::GetInstance().AddTextInfo("engine/enginedata.burr missing!!!");
                Console::GetInstance().CloseOutput();
                exit(1);
            }

            if (!ResourceManager::GetInstance().Load("engine/ui.burr","ui")){
                Console::GetInstance().AddTextInfo("engine/ui.burr missing!!!");
            }
        }
        if (startFlags&BEAR_FLAG_START_SDL){
            SDLStarted = true;


            SDL_version compiled;
            SDL_version linked;
            SDL_VERSION(&compiled);
            SDL_GetVersion(&linked);
            Console::GetInstance().AddText(utils::format("Compiled with SDL version %d.%d.%d",compiled.major, compiled.minor, compiled.patch));
            Console::GetInstance().AddText(utils::format("Linked with SDL version %d.%d.%d.", linked.major, linked.minor, linked.patch));
        }
        if (startFlags&BEAR_FLAG_START_TTF){
            const SDL_version *link_version =TTF_Linked_Version();
            SDL_version compiled;
            SDL_TTF_VERSION(&compiled);

            Console::GetInstance().AddText(utils::format("SDL_TTF compiled with %d.%d.%d",compiled.major, compiled.minor, compiled.patch));
            Console::GetInstance().AddText(utils::format("SDL_TTF Linked with %d.%d.%d.", link_version->major, link_version->minor, link_version->patch));
        }
        if (startFlags&BEAR_FLAG_START_INPUT){
            Console::GetInstance().AddText("Opening input");
            g_input.init();
        }
        ConfigManager::GetInstance().DisplayArgs();
        if (startFlags&BEAR_FLAG_START_SOUND){
            Console::GetInstance().AddText("Opening audio");
            device = alcOpenDevice(nullptr);
            if (device){
                SoundLoader::ShowError("on make");
                HasAudio = true;
                ctx = alcCreateContext(device,nullptr);
                if (!alcMakeContextCurrent(ctx)){
                   SoundLoader::ShowError("on device");
                   Console::GetInstance().AddText("Some error on audio!");
                }
                SoundLoader::ShowError("on make");
                ALfloat listenerOri[] = { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f };
                alListener3f(AL_POSITION, 0, 0, 1.0f);
                SoundLoader::ShowError("on pos");
                alListener3f(AL_VELOCITY, 0, 0, 0);
                SoundLoader::ShowError("on vel");
                alListenerfv(AL_ORIENTATION, listenerOri);
                SoundLoader::ShowError("on ori");
            }else{
                HasAudio = true;
                Console::GetInstance().AddText("Cannot start audio");
            }
            SoundPool::GetInstance(true);
        }

        skipRender = 1;
        nextUpdate =  1000;

        srand(time(nullptr));
        if (startFlags&BEAR_FLAG_START_THREADS){
            Console::GetInstance().AddText("Starting threads");
            ThreadPool::GetInstance();
            ThreadPool::GetInstance().Begin(POOL_DEFAULT_THREADS);
        }
        if (startFlags&BEAR_FLAG_START_CONSOLEGRAPHICAL){
            Console::GetInstance().AddText("Opening console graphical");
            Console::GetInstance().Begin();
        }

        if (startFlags&BEAR_FLAG_START_LUA){

            #ifndef DISABLE_LUAINTERFACE

            LuaInterface::Instance().Startup();

            #endif
        }

        CalculateDeltaTime();
        if (startFlags&BEAR_FLAG_START_CONSOLE){
            Console::GetInstance().AddTextInfo(utils::format("Bear started in %f seconds",GetDeltaTime()/10.0f));
        }
        dt = frameStart = 0;
        Started = true;
    }else{
        Console::GetInstance().AddTextInfo("It seems you created a new Game instance. Well. Closing");
        Close();
        exit(1);
    }

}
Game::~Game(){
    if (!hasBeenClosed)
        Close();
}
void Game::Close(){
    hasBeenClosed = true;
    while (!stateStack.empty()){
        if (stateStack.top() != nullptr){
            stateStack.top()->End();
            delete stateStack.top();
            stateStack.pop();
        }
    }
    isClosing = true;
    if (Started)
        GameBehavior::GetInstance().OnClose();

    #ifndef DISABLE_THREADPOOL

    if (startFlags&BEAR_FLAG_START_THREADS){
        Console::GetInstance().AddTextInfo("Closing threads...");
        if (Started)
            ThreadPool::GetInstance().KillThreads();
        Console::GetInstance().AddTextInfo("Threads closed...");
    }
    #endif
    if (startFlags&BEAR_FLAG_LOAD_BASEFILES){
        Console::GetInstance().AddTextInfo("Closing engine assets");
        ResourceManager::GetInstance().Erase("engine");
    }


    #ifndef DISABLE_LUAINTERFACE
    if (startFlags&BEAR_FLAG_START_LUA){
        Console::GetInstance().AddTextInfo("Closing lua");
        LuaInterface::Instance().Close();
    }
    #endif
    Console::GetInstance().AddTextInfo("Resourcefiles");
    ResourceManager::GetInstance().ClearAll();

    Console::GetInstance().AddTextInfo("Closing screen");
    if (startFlags&BEAR_FLAG_START_SCREEN)
        ScreenManager::GetInstance().TerminateScreen();

    if (startFlags&BEAR_FLAG_START_SOUND)
        if (HasAudio){

            SoundWorker::Clear();
            Console::GetInstance().AddTextInfo("Closing audio");

            SoundPool::GetInstance().Close();
            Console::GetInstance().AddTextInfo("Closing OpenAl");
            alcMakeContextCurrent(nullptr);
            alcDestroyContext(ctx);
            alcCloseDevice(device);

        }

    Console::GetInstance().AddTextInfo("Closing text");
    if (startFlags&BEAR_FLAG_START_TTF)
        TTF_Quit();
    Console::GetInstance().AddTextInfo("Closing sdl");

    Console::GetInstance().AddTextInfo("Quit game");
    Console::GetInstance().CloseOutput();



    #ifdef __ANDROID__
    exit(0);
    #endif
    SDL_Quit();
}

Game *Game::GetInstance(const char *name){
    return &g_game;
}
void Game::Update(){
    float dt = std::min(GetDeltaTime(),1.2f );
    if (startFlags&BEAR_FLAG_START_INPUT)
        InputManager::GetInstance().Update(dt);

    if (startFlags&BEAR_FLAG_START_SOUND)
        SoundWorker::Update(dt);

    GameBehavior::GetInstance().OnUpdate(dt);


    if (startFlags&BEAR_FLAG_START_SCREEN)
        ScreenManager::GetInstance().PreRender();

    if (!CanStop()){
        g_scheduler.Update(dt);
        #ifndef DISABLE_LUAINTERFACE
        if (startFlags&BEAR_FLAG_START_LUA)
            LuaInterface::Instance().Update(dt);
        #endif

        stateStack.top()->Update(std::min(dt,ConfigManager::MinimumDT) );



        if (startFlags&BEAR_FLAG_START_SCREEN)
            ScreenManager::GetInstance().Update(dt);
    }

}
void Game::Render(){
    if ( (startFlags&BEAR_FLAG_START_SCREEN) == 0)
        return;
    GameBehavior::GetInstance().OnPreRender();
    stateStack.top()->Render();
    ScreenManager::GetInstance().Render();
    InputManager::GetInstance().Render();
    ScreenManager::GetInstance().RenderPresent();
}


void Game::Begin(){
    if (!GameBehavior::GetInstance().OnLoad()){
        Console::GetInstance().AddText("[Game::Begin]<OnLoad> Returned false. May not continue!");
        Close();
        exit(1);
    }
    if (!preStored.empty()){
        stateStack.emplace(preStored.front());
        preStored.pop();
        stateStack.top()->p_StateTicks = 0;
    }
    GameBegin = true;
}

bool Game::CanStop(){
    if (isClosing || ((SDL_QuitRequested() || stateStack.empty() || stateStack.top() == nullptr || stateStack.top()->RequestedQuit()) && preStored.empty())) {
        isClosing = true;
        return true;
    }else{
        return false;
    }
}

void Game::Run(){
        //Stopwatch st;
        frameStart = dt;
        CalculateDeltaTime();

        //uint32_t tu = st.Get();
        //st.Reset();
        if (!CanStop()){
            if (startFlags&BEAR_FLAG_START_INPUT){
                if (InputManager::GetInstance().KeyPress(SDLK_F3)){
                    static bool full = false;
                    if (!full){
                        //ScreenManager::GetInstance().MakeDefaultScreenAsTexture();
                        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
                    }else{
                        //ScreenManager::GetInstance().ClearScreenTexture();
                        SDL_SetWindowFullscreen(window, SDL_FALSE);
                    }
                    full = !full;
                }
            }


            bool justDeleted = false;
            if (!stateStack.empty()){

                if (stateStack.top()->RequestedDeleted()){
                    stateStack.top()->End();
                    #ifndef DISABLE_LUAINTERFACE
                    LuaCaller::CallClear(LuaManager::L,stateStack.top());
                    #endif // DISABLE_LUAINTERFACE

                    justDeleted = true;


                    DefinedState *oldState = stateStack.top();
                    stateStack.pop();
                    if (!stateStack.empty()){
                        stateStack.top()->Resume(oldState);
                    }
                    delete oldState;
                    return;
                }
            }

            if (!preStored.empty()){
                if (!justDeleted && !stateStack.empty())
                    stateStack.top()->Pause(preStored.front());
                stateStack.emplace(preStored.front());
                preStored.pop();
                stateStack.top()->p_StateTicks = 0;
                return;
            }
            if (stateStack.empty() ){
                if (preStored.empty()){
                    isClosing = true;
                }
                return;
            }
            stateStack.top()->p_StateTicks++;
            if (stateStack.top()->p_StateTicks == 1){
                 stateStack.top()->Begin();
                 return;
            }



            #ifdef CYCLYC_DEBUG
            bear::out << "[Update]";
            #endif
            Update();
            #ifdef CYCLYC_DEBUG
            bear::out << "[\\Update]\n";
            #endif
            if (stateStack.top()->RequestedDeleted()){
                return;
            }

            #ifdef CYCLYC_DEBUG
            bear::out << "[Render]";
            #endif

            Render();

            #ifdef CYCLYC_DEBUG
            bear::out << "[\\Render]\n";
            #endif
            //unsigneduint32_t tr = st.Get();

            float delay = SDL_GetTicks()-dt;
            if ((1000.0f/ConfigManager::MaxFps) - delay > 0){
                SDL_Delay( std::max( (1000.0f/ConfigManager::MaxFps) - delay,0.0f) );
            }

        }
};
DefinedState &Game::GetCurrentState(){
    Game *g = GetInstance();
    return (*g->stateStack.top());
}

void Game::DisplayBearInfo(){
    bear::out << "Bear engine V " << BEAR_VERSION <<"\n";
    bear::out << "Start flags: "<< startFlags <<"\n";
    bear::out << "Built at: "<<__DATE__ << " "<< __TIME__<<"\n";
    #ifndef DISABLE_LUAINTERFACE
    bear::out << "Has lua support enabled\n";
    #endif // DISABLE_LUAINTERFACE
}


void Game::AddState(DefinedState *s,int forcedId){
    static int Ids = 0;
    if (forcedId == -1){
        s->STATEID = Ids++;
    }
    preStored.emplace(s);


    #ifndef DISABLE_LUAINTERFACE
    DefinedState *MainState = nullptr;
    if (startFlags&BEAR_FLAG_START_LUA){
        if (stateStack.size() > 0){
            MainState = stateStack.top();
        }
        LuaCaller::StartupState(LuaManager::L,s,MainState);
    }
    #endif // DISABLE_LUAINTERFACE
    Ids++;
};

bool Game::IsClosed(){
    if (instance){
        return instance->hasBeenClosed;
    }
    return false;
}
