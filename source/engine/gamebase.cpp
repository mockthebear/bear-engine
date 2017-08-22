// *** ADDED BY HEADER FIXUP ***
#include <ctime>
// *** END ***
//////////////////////////////////////////////////////////////
////////////////////MAIN FILE/////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
#include <time.h>
#include "gamebase.hpp"


#include "../luasystem/luainterface.hpp"
#include "../framework/threadpool.hpp"
#include "../framework/debughelper.hpp"
#include "../framework/resourcemanager.hpp"
#include "timer.hpp"

#include "parallelcollisionmanager.hpp"

#include "../settings/definitions.hpp"
#include "../performance/console.hpp"
#include "../sound/soundsources.hpp"
#include "../sound/soundloader.hpp"
#include __BEHAVIOR_FOLDER__



#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif
Game g_game;
Game* Game::instance = NULL;

Game::Game(){isClosing=SDLStarted=canDebug=GameBegin=hasBeenClosed=HasAudio=false;};

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

        startFlags = BEAR_FLAG_START_EVERYTHING;
        if (startFlags&BEAR_FLAG_START_CONSOLE){
            Console::GetInstance(true);
            Console::GetInstance().AddTextInfo("Starting...");
        }

        //GameBehavior::GetInstance().OnOpen();

        if (startFlags&BEAR_FLAG_START_SDL){
            if (SDL_Init(BEAR_SDL_CONST_INIT) != 0){
                Console::GetInstance().AddTextInfo( utils::format("SDL may nor work because [%s]",SDL_GetError()) );
            }
        }
        if (startFlags&BEAR_FLAG_START_TTF){
            if (TTF_Init()  == -1 ){
                Console::GetInstance().AddTextInfo("TTF not working");
            }
        }
        if (startFlags&BEAR_FLAG_START_SOUND){
            ConfigManager::GetInstance().SetSound(HasAudio);
        }
        GameBehavior::GetInstance();
        if (startFlags&BEAR_FLAG_START_SCREEN){
            window = ScreenManager::GetInstance().StartScreen(name);
            if (window == NULL){
                Console::GetInstance().AddTextInfo("Failed creating screen");
                exit(1);
            }

            renderer = ScreenManager::GetInstance().StartRenderer();

            if (!renderer){
                Console::GetInstance().AddTextInfo("Failed creating render");
                exit(1);
            }
        }
        if (startFlags&BEAR_FLAG_LOAD_BASEFILES){
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
            device = alcOpenDevice(NULL);
            if (device){
                SoundLoader::ShowError("on make");
                HasAudio = true;
                ctx = alcCreateContext(device,NULL);
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

        srand(time(NULL));
        if (startFlags&BEAR_FLAG_START_THREADS){
            ThreadPool::GetInstance(POOL_DEFAULT_THREADS);
        }
        if (startFlags&BEAR_FLAG_START_CONSOLE){
            Console::GetInstance().AddText("Opening console");
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
        delete stateStack.top();
        stateStack.pop();
    }
    isClosing = true;
    if (Started)
        GameBehavior::GetInstance().OnClose();
    Console::GetInstance().AddTextInfo("Closing engine assets");
    #ifndef DISABLE_THREADPOOL
    Console::GetInstance().AddTextInfo("Closing threads");
    if (startFlags&BEAR_FLAG_START_THREADS)
        if (Started)
            ThreadPool::GetInstance().KillThreads();
    #endif
    ResourceManager::GetInstance().Erase("engine");

    Console::GetInstance().AddTextInfo("Closing lua");
    #ifndef DISABLE_LUAINTERFACE
    if (startFlags&BEAR_FLAG_START_LUA)
        LuaInterface::Instance().Close();
    #endif
    Console::GetInstance().AddTextInfo("Resourcefiles");
    ResourceManager::GetInstance().ClearAll();

    Console::GetInstance().AddTextInfo("Closing screen");
    if (startFlags&BEAR_FLAG_START_SCREEN)
        ScreenManager::GetInstance().TerminateScreen();

    if (startFlags&BEAR_FLAG_START_SOUND)
        if (HasAudio){
            Console::GetInstance().AddTextInfo("Closing audio");

            SoundPool::GetInstance().Close();
            Console::GetInstance().AddTextInfo("Closing OpenAl");
            alcMakeContextCurrent(NULL);
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

    GameBehavior::GetInstance().OnUpdate(dt);

    if (startFlags&BEAR_FLAG_START_SCREEN)
        ScreenManager::GetInstance().PreRender();

    if (!CanStop()){
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
    if (!startFlags&BEAR_FLAG_START_SCREEN)
        return;
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
    if (storedState != NULL){
        stateStack.emplace(storedState);
        stateStack.top()->Begin();
        storedState =NULL;
    }
    GameBegin = true;
}

bool Game::CanStop(){
    if (isClosing || (SDL_QuitRequested() || stateStack.empty() || stateStack.top() == nullptr || stateStack.top()->RequestedQuit())){
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

        #ifdef CYCLYC_DEBUG
        bear::out << "[Update]";
        #endif
        Update();
        #ifdef CYCLYC_DEBUG
        bear::out << "[\\Update]\n";
        #endif
        //uint32_t tu = st.Get();
        //st.Reset();
        if (!CanStop()){
            if (startFlags&BEAR_FLAG_START_INPUT){
                if (InputManager::GetInstance().KeyPress(SDLK_F3)){
                    static bool full = false;
                    if (!full){
                        ScreenManager::GetInstance().MakeDefaultScreenAsTexture();
                        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
                    }else{
                        ScreenManager::GetInstance().ClearScreenTexture();
                        SDL_SetWindowFullscreen(window, SDL_FALSE);
                    }
                    full = !full;
                }
            }


            bool justDeleted = false;
            if (stateStack.top()->RequestedDeleted()){
                if (stateStack.empty() && storedState == NULL){
                    isClosing=true;
                    return;
                }else{
                    LuaCaller::CallClear(LuaManager::L,stateStack.top());
                    stateStack.top()->End();
                    delete stateStack.top();
                    stateStack.pop();
                    if (stateStack.empty()){
                        return;
                    }
                    justDeleted = true;
                    if (storedState == nullptr){
                        stateStack.top()->Resume(storedState);
                    }
                }
            }

            if (storedState != NULL){
                if (!justDeleted)
                    stateStack.top()->Pause(storedState);
                stateStack.emplace(storedState);
                stateStack.top()->Begin();
                storedState =NULL;
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

void Game::AddState(DefinedState *s,int forcedId){
    storedState = s;
    static int Ids = 0;
    if (forcedId == -1){
        storedState->STATEID = Ids++;
    }
    #ifndef DISABLE_LUAINTERFACE
    if (startFlags&BEAR_FLAG_START_LUA)
        LuaCaller::StartupState(LuaManager::L,s);
    #endif // DISABLE_LUAINTERFACE
    Ids++;
};
