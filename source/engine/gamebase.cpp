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

        Console::GetInstance(true);
        Console::GetInstance().AddTextInfo("Starting...");

        if (SDL_Init(BEAR_SDL_CONST_INIT) != 0){
            Console::GetInstance().AddTextInfo( utils::format("SDL may nor work because [%s]",SDL_GetError()) );
        }

        if (TTF_Init()  == -1 ){
            Console::GetInstance().AddTextInfo("TTF not working");
        }

        ConfigManager::GetInstance().SetSound(HasAudio);
        GameBehavior::GetInstance();
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

        if (!ResourceManager::GetInstance().Load("engine/enginedata.burr","engine")){
            Console::GetInstance().AddTextInfo("engine/enginedata.burr missing!!!");
            Console::GetInstance().CloseOutput();
            exit(1);
        }

        if (!ResourceManager::GetInstance().Load("engine/ui.burr","ui")){
            Console::GetInstance().AddTextInfo("engine/ui.burr missing!!!");
        }

        SDLStarted = true;

        SDL_version compiled;
        SDL_version linked;
        SDL_VERSION(&compiled);
        SDL_GetVersion(&linked);
        Console::GetInstance().AddText(utils::format("Compiled with SDL version %d.%d.%d",compiled.major, compiled.minor, compiled.patch));
        Console::GetInstance().AddText(utils::format("Linked with SDL version %d.%d.%d.", linked.major, linked.minor, linked.patch));

        const SDL_version *link_version =TTF_Linked_Version();
        SDL_TTF_VERSION(&compiled);

        Console::GetInstance().AddText(utils::format("SDL_TTF compiled with %d.%d.%d",compiled.major, compiled.minor, compiled.patch));
        Console::GetInstance().AddText(utils::format("SDL_TTF Linked with %d.%d.%d.", link_version->major, link_version->minor, link_version->patch));
        Console::GetInstance().AddText("Opening input");
        g_input.init();
        ConfigManager::GetInstance().DisplayArgs();
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

        skipRender = 1;
        nextUpdate =  1000;

        srand(time(NULL));

        ThreadPool::GetInstance(POOL_DEFAULT_THREADS);

        Console::GetInstance().AddText("Opening console");
        Console::GetInstance().Begin();

        #ifndef DISABLE_LUAINTERFACE

        LuaInterface::Instance().Startup();

        #endif

        CalculateDeltaTime();
        Console::GetInstance().AddTextInfo(utils::format("Bear started in %f seconds",GetDeltaTime()/10.0f));
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
    #ifndef DISABLE_LUAINTERFACE
    LuaInterface::Instance().Close();
    #endif // DISABLE_LUAINTERFACE
    while (!stateStack.empty()){
        stateStack.pop();
    }
    isClosing = true;
    if (Started)
        GameBehavior::GetInstance().OnClose();
    Console::GetInstance().AddTextInfo("Closing engine assets");
    #ifndef DISABLE_THREADPOOL
    Console::GetInstance().AddTextInfo("Closing threads");
    if (Started)
        ThreadPool::GetInstance().KillThreads();
    #endif
    ResourceManager::GetInstance().Erase("engine");
    Console::GetInstance().AddTextInfo("Closing screen");
    ScreenManager::GetInstance().TerminateScreen();


    if (HasAudio){
        Console::GetInstance().AddTextInfo("Closing audio");
        alcDestroyContext(ctx);
        alcCloseDevice(device);

    }
    //    Mix_CloseAudio();
    Console::GetInstance().AddTextInfo("Closing text");
    TTF_Quit();
    Console::GetInstance().AddTextInfo("Closing mix");
    //Mix_Quit();
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
    //
        //

    //}
    float dt = std::min(GetDeltaTime(),1.2f );
    InputManager::GetInstance().Update(dt);
    GameBehavior::GetInstance().OnUpdate(dt);
    ScreenManager::GetInstance().PreRender();
    if (!CanStop()){
        #ifndef DISABLE_LUAINTERFACE
        LuaInterface::Instance().Update(dt);
        #endif
        #ifndef DISABLE_THREADPOOL
        if (wasLocked){
            ThreadPool::GetInstance().Lock();
            wasLocked = false;
        }
        #endif // DISABLE_THREADPOOL
        stateStack.top()->Update(std::min(dt,ConfigManager::MinimumDT) );
        #ifndef DISABLE_THREADPOOL
        g_parallelCollision.Reset();
        wasLocked = g_parallelCollision.MakeJobs();
        if (wasLocked){
            ThreadPool::GetInstance().Unlock();
        }
        #endif // DISABLE_THREADPOOL
        ScreenManager::GetInstance().Update(dt);
    }

}
void Game::Render(){
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
    ThreadPool::GetInstance().Unlock();
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
            if (InputManager::GetInstance().KeyPress(SDLK_F3)){
                static bool full = false;
                if (!full)
                    SDL_SetWindowFullscreen(window, SDL_TRUE);
                else
                    SDL_SetWindowFullscreen(window, SDL_FALSE);
                full = !full;
            }



            if (stateStack.top()->RequestedDeleted()){
                if (stateStack.empty() && storedState == NULL){
                    isClosing=true;
                    return;
                }else{
                    stateStack.top()->End();
                    delete stateStack.top();
                    stateStack.pop();
                    if (stateStack.empty()){
                        return;
                    }
                    stateStack.top()->Resume(storedState);
                }
            }

            if (storedState != NULL){
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
            std::cout << tu << ":"<<tr<<"\n";
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
