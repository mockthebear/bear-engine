///Core Settings
#define BearEngine Game::GetInstance()

#define __BEHAVIOR_FOLDER__ "../game/gamebehavior.hpp"

#define STATE_FILE "../game/state.hpp"
#define DefinedState State

#define BeginStateFile "../game/title.hpp"
#define BeginStateClass Title
/*
    Web porting
*/
#ifdef __EMSCRIPTEN__
#define ASSETS_FOLDER "game/"
#else
#define ASSETS_FOLDER ""
#endif

/*
    Screen
*/

#define SCREEN_SIZE_W 800
#define SCREEN_SIZE_H 600

#define MAXFPS 300.0




#define POOL_DEFAULT_THREADS 2
//#define DISABLE_THREADPOOL
/*
    Lua
*/
#define DISABLE_LUAINTERFACE





#ifdef __ANDROID__
#define BEAR_SDL_CONST_INIT SDL_INIT_EVERYTHING
#define BEAR_SDL_IMAGE_CONST_INIT IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF
#define SDL_LIB_HEADER <SDL.h>
#define SDL_IMAGE_LIB_HEADER <SDL_image.h>
#define SDL_TTF_LIB_HEADER <SDL_ttf.h>
#define SDL_MIXER_LIB_HEADER <SDL_mixer.h>
#define LUA_INCLUDE <lua.hpp>


#elif __EMSCRIPTEN__
#define BEAR_SDL_CONST_INIT SDL_INIT_EVERYTHING
#define BEAR_SDL_IMAGE_CONST_INIT IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF


#define SDL_LIB_HEADER <SDL2/SDL.h>
#define SDL_IMAGE_LIB_HEADER <SDL2/SDL_image.h>
#define SDL_TTF_LIB_HEADER <SDL_ttf.h>
#define SDL_MIXER_LIB_HEADER <SDL_mixer.h>
#define LUA_INCLUDE <stdio.h>
#define DISABLE_LUAINTERFACE

#else
#define BEAR_SDL_CONST_INIT SDL_INIT_EVERYTHING
#define BEAR_SDL_IMAGE_CONST_INIT IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF

#define SDL_LIB_HEADER "SDL2/SDL.h"
#define SDL_IMAGE_LIB_HEADER "SDL2/SDL_image.h"
#define SDL_TTF_LIB_HEADER "SDL2/SDL_ttf.h"
#define SDL_MIXER_LIB_HEADER "SDL2/SDL_mixer.h"
#define LUA_INCLUDE <lua.hpp>
#endif



