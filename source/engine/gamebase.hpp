// *** ADDED BY HEADER FIXUP ***
#include <string>
// *** END ***
#ifndef GAMEBASEH
#define GAMEBASEH
#include <cstdlib>
#include <iostream>
#include <memory>
#include <vector>
#include <list>
#include <stack>
#include <string.h>
#include <stdio.h>
#include <functional>


#include "../settings/definitions.hpp"

#include SDL_LIB_HEADER
#include SDL_TTF_LIB_HEADER
#include STATE_FILE


#include <AL/al.h>
#include <AL/alc.h>


#include "../input/inputmanager.hpp"
#include "../settings/configmanager.hpp"
#include "screenmanager.hpp"

#ifdef __ANDROID__
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

#endif

/**
    @brief Game controller
    *
    * Here, the game loop will be managed.
    * These function must be called in the main.cpp:\n
    * Game::Begin
    * Game::Close
    * Game::CanStop
    * Game::Run
    *
    * You dont need to do it. The main came with everything.
*/
class Game{
    public:
        /**
            * Begin the game
        */
        void Begin();
        /**
            * Close the game
        */
        void Close();
        /**
            * Game loop
        */
        void Run();
        /**
            * Check if the game can stop
        */
        bool CanStop();
        /**
            * Get the SDL_Renderer global pointer.
            @return Renderer
        */
        SDL_Renderer* GetRenderer() {return renderer;};
        /**
            * Get the Window pointer
            @return Window
        */
        SDL_Window* GetWindow() {return window;};
        /**
            *Get the current state
            @return GenericState
        */
        static DefinedState &GetCurrentState();
        /**
            * Used on constructor
        */
        static Game* GetInstance (const char *name);
        /**
            *Singleton.
            *This one is pointer because make difference in performance!
        */
        static inline Game* GetInstance(){return instance;};
        /**
            *Delta time. Get the time between the frames
        */
        inline float GetDeltaTime(){return (dt-frameStart)/100.0;};
        /**
            *Add an state to the state stack
        */
        void AddState(DefinedState *s,int forcedId = -1){
            storedState = s;
            static int Ids = 0;
            if (forcedId == -1){
                storedState->STATEID = Ids++;
            }
            Ids++;
        };
        /**
            Notify the engine about the game closing
        */
        bool isClosing;
        /**
            Check if the game has audio
        */
        bool AudioWorking(){return HasAudio;};
        /**
            Check if the game has begin
        */
        bool IsGameBegin(){return GameBegin;};
        /**
            Check if the SDL has started
        */
        bool IsSDLStarted(){return SDLStarted;};
        /**
            Flag to check if debug is enabled
            only run time
        */
        bool canDebug;
        Game();
        ~Game();
        void init(const char *name);
    private:
        inline void CalculateDeltaTime(){dt = SDL_GetTicks();};
        void Update();
        void Render();
        static Game* instance;

        std::stack<DefinedState*> stateStack;
        bool hasBeenClosed;
        int skipRender;
        bool Started;
        bool HasAudio;
        bool GameBegin;
        DefinedState *storedState;
        void UpdateTitleAsFps(float);

        unsigned int nextUpdate;
        int frameStart;
        float dt;

        bool SDLStarted;

        SDL_Renderer* renderer;
        SDL_Window* window;


        ALCdevice *device;
        ALCcontext *ctx;



};

extern Game g_game;
#endif
