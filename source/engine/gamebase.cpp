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

#include __BEHAVIOR_FOLDER__



#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif



Game* Game::instance = NULL;

Game::Game(const char *name){
    if (instance == NULL){
        SDLStarted = false;
        isClosing = false;
        hasBeenClosed = false;
        instance = this;
        HasAudio = true;
        Halt = false;
        GameBegin = false;
        dt = frameStart = 0;
        canDebug = false;

        Console::GetInstance(true); //Start console
        Console::GetInstance().AddTextInfo("Starting...");

        if (SDL_Init(BEAR_SDL_CONST_INIT) != 0){
            Console::GetInstance().AddTextInfo( utils::format("SDL may nor work because [%s]",SDL_GetError()) );
        }
        if (not IMG_Init(BEAR_SDL_IMAGE_CONST_INIT)){
            Console::GetInstance().AddTextInfo( utils::format("SDL img not working [%s]",IMG_GetError()));
        }
        if (not Mix_Init(MIX_INIT_FLAC | MIX_INIT_MP3 | MIX_INIT_OGG)){
            Console::GetInstance().AddTextInfo("Audio not found");
            HasAudio=false;
        }else{
            HasAudio = true;
        }
        if (TTF_Init()  == -1 ){
            Console::GetInstance().AddTextInfo("TTF not working");
        }
        if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT,MIX_DEFAULT_CHANNELS,1024) == -1){
            HasAudio=false;
            Console::GetInstance().AddTextInfo("Audio not found");
        }


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
            return;
        }

        if (!ResourceManager::GetInstance().Load("engine/ui.burr","ui")){
            Console::GetInstance().AddTextInfo("engine/ui.burr missing!!!");
            return;
        }

        SDLStarted = true;

        Console::GetInstance().Begin();


        skipRender = 1;
        nextUpdate =  1000;

        srand(time(NULL));


        ThreadPool::GetInstance(POOL_DEFAULT_THREADS);


        #ifndef DISABLE_LUAINTERFACE
        LuaInterface::Instance().Startup();
        #endif



        if (HasAudio){
            Mix_Volume(-1,   128.0/2.0);
            Mix_VolumeMusic( 128.0/2.0);
        }

        CalculateDeltaTime();
        Console::GetInstance().AddTextInfo(utils::format("Bear started in %f seconds",GetDeltaTime()/10.0f));
        dt = frameStart = 0;


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

    ResourceManager::GetInstance().Erase("engine");
    Console::GetInstance().AddTextInfo("Closing screen");
    ScreenManager::GetInstance().TerminateScreen();
    if (HasAudio)
        Console::GetInstance().AddTextInfo("Closing audio");
    if (HasAudio)
        Mix_CloseAudio();
    Console::GetInstance().AddTextInfo("Closing text");
    TTF_Quit();
    Console::GetInstance().AddTextInfo("Closing mix");
    Mix_Quit();
    Console::GetInstance().AddTextInfo("Closing img");
    IMG_Quit();
    Console::GetInstance().AddTextInfo("Closing sdl");

    Console::GetInstance().AddTextInfo("Quit game");
    Console::GetInstance().CloseOutput();

    SDL_Quit();
}

Game *Game::GetInstance(const char *name){
    if (instance == NULL){
        instance = new Game(name);
    }
    return instance;
}
void Game::Update(){

    float dt = std::min(GetDeltaTime(),1.2f );
    InputManager::GetInstance().Update(dt);
    if (!CanStop()){
        #ifndef DISABLE_LUAINTERFACE
        LuaInterface::Instance().Update(dt);
        #endif
        if (!Halt)
            stateStack.top()->Update(dt);
        ScreenManager::GetInstance().Update(dt);
    }

}
void Game::Render(){
    if (!Halt)
        stateStack.top()->Render();
    InputManager::GetInstance().Render();
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
    if (isClosing or (SDL_QuitRequested() or stateStack.top()->RequestedQuit())){
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
            if (InputManager::GetInstance().KeyPress(SDLK_F11)){
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
                }
            }

            if (storedState != NULL){
                stateStack.emplace(storedState);

                stateStack.top()->Begin();
                storedState =NULL;
                return;
            }

            #ifdef CYCLYC_DEBUG
            std::cout << "[Render]";
            #endif

            Render();
            SDL_RenderPresent(GetRenderer() );
            #ifdef CYCLYC_DEBUG
            std::cout << "[\\Render]\n";
            #endif


        }
};
DefinedState &Game::GetCurrentState(){
    return (*stateStack.top());
}
