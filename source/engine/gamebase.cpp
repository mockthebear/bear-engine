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

Game::Game(){};

void Game::init(const char *name){
    if (instance == NULL){
        SDLStarted = false;
        isClosing = false;
        hasBeenClosed = false;
        instance = this;
        HasAudio = true;
        GameBegin = false;
        dt = frameStart = 0;
        canDebug = false;

        Console::GetInstance(true); //Start console
        Console::GetInstance().AddTextInfo("Starting...");

        if (SDL_Init(BEAR_SDL_CONST_INIT) != 0){
            Console::GetInstance().AddTextInfo( utils::format("SDL may nor work because [%s]",SDL_GetError()) );
        }
        /*if (not Mix_Init(MIX_INIT_FLAC | MIX_INIT_MP3 | MIX_INIT_OGG)){
            Console::GetInstance().AddTextInfo("Audio not found");
            HasAudio=false;
        }else{
            HasAudio = true;
        }*/
        if (TTF_Init()  == -1 ){
            Console::GetInstance().AddTextInfo("TTF not working");
        }
        /*if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT,MIX_DEFAULT_CHANNELS,1024) == -1){
            HasAudio=false;
            Console::GetInstance().AddTextInfo("Audio not found");
        }*/


        ConfigManager::GetInstance().SetSound(HasAudio);
        GameBehavior::GetInstance();
        window = ScreenManager::GetInstance().StartScreen(name);
        if (window == NULL){
            Console::GetInstance().AddTextInfo("Failed creating screen");
            return;
        }

        renderer = ScreenManager::GetInstance().StartRenderer();

        if (!renderer){
            Console::GetInstance().AddTextInfo("Failed creating render");
            return;
        }

        if (!ResourceManager::GetInstance().Load("engine/enginedata.burr","engine")){
            Console::GetInstance().AddTextInfo("engine/enginedata.burr missing!!!");
            Console::GetInstance().CloseOutput();
            exit(1);
            return;
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

        /*link_version=Mix_Linked_Version();
        SDL_MIXER_VERSION(&compiled);

        Console::GetInstance().AddText(utils::format("SDL_Mixer compiled with %d.%d.%d",compiled.major, compiled.minor, compiled.patch));
        Console::GetInstance().AddText(utils::format("SDL_Mixer Linked with %d.%d.%d.", link_version->major, link_version->minor, link_version->patch));*/

        g_input.init();
        Console::GetInstance().Begin();
        ConfigManager::GetInstance().DisplayArgs();
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
        Console::GetInstance().AddText("Finished audio!");
        SoundPool::GetInstance(true);

        skipRender = 1;
        nextUpdate =  1000;

        srand(time(NULL));


        ThreadPool::GetInstance(POOL_DEFAULT_THREADS);


        #ifndef DISABLE_LUAINTERFACE
        LuaInterface::Instance().Startup();
        #endif

        /*if (HasAudio){
            Mix_Volume(-1,   128.0/2.0);
            Mix_VolumeMusic( 128.0/2.0);
        }*/

        CalculateDeltaTime();
        Console::GetInstance().AddTextInfo(utils::format("Bear started in %f seconds",GetDeltaTime()/10.0f));
        dt = frameStart = 0;

        //ScreenManager::GetInstance().Render();
        //getchar();


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
    stateStack.pop();
    //delete title;
    isClosing = true;
    GameBehavior::GetInstance().OnClose();
    Console::GetInstance().AddTextInfo("Closing engine assets");
    #ifndef DISABLE_THREADPOOL
    Console::GetInstance().AddTextInfo("Closing threads");
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

    float dt = std::min(GetDeltaTime(),1.2f );
    InputManager::GetInstance().Update(dt);
    GameBehavior::GetInstance().OnUpdate(dt);
    ScreenManager::GetInstance().PreRender();
    if (!CanStop()){
        #ifndef DISABLE_LUAINTERFACE
        LuaInterface::Instance().Update(dt);
        #endif
        stateStack.top()->Update(std::min(dt,ConfigManager::MinimumDT) );
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
        return;
    }
    if (storedState != NULL){
        stateStack.emplace(storedState);
        stateStack.top()->Begin();
        storedState =NULL;
    }
    GameBegin = true;
}

bool Game::CanStop(){
    if (isClosing || (SDL_QuitRequested() || stateStack.top()->RequestedQuit())){
        isClosing = true;
        return true;
    }else{
        return false;
    }
}

void Game::Run(){

        frameStart = dt;
        CalculateDeltaTime();

        #ifdef CYCLYC_DEBUG
        std::cout << "[Update]";
        #endif
        Update();
        #ifdef CYCLYC_DEBUG
        std::cout << "[\\Update]\n";
        #endif
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
                    delete stateStack.top();
                    stateStack.pop();
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
            std::cout << "[Render]";
            #endif

            Render();

            #ifdef CYCLYC_DEBUG
            std::cout << "[\\Render]\n";
            #endif
            float delay = SDL_GetTicks()-dt;

            if ((1000.0f/ConfigManager::MaxFps) - delay > 0){
                SDL_Delay( (1000.0f/ConfigManager::MaxFps) - delay );
            }

        }
};
DefinedState &Game::GetCurrentState(){
    Game *g = GetInstance();
    return (*g->stateStack.top());
}
