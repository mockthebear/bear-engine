///Core Settings
#define BearEngine Game::GetInstance()



#define APPNAME "Bear engine"

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

#define SCREEN_SIZE_W 640
#define SCREEN_SIZE_H 480

#define MAXFPS 900.0




#define POOL_DEFAULT_THREADS 2
//#define DISABLE_THREADPOOL
/*
    Lua
*/
#define DISABLE_LUAINTERFACE

/*
    Android things
*/
#define JNI_BASE Java
#define JNI_COM com
#define JNI_MODULE tutorial
#define JNI_FROM game
#define JNI_NAME HelloSDL2Activity

/**
*********************************************************
**********************ENGINE STUFF***********************
*********************************************************

*/


#define CAT(X,Y) X ##_## Y
#define CAT2(X,Y) CAT(X,Y)
#define CAT3(X,Y,Z) CAT2(X,CAT2(Y,Z))
#define CAT4(X,Y,Z,W) CAT2(X,CAT3(Y,Z,W))
#define CAT5(X,Y,Z,W,R) CAT2(X,CAT4(Y,Z,W,R))

#define FNAME CAT5(JNI_BASE,JNI_COM,JNI_MODULE,JNI_FROM,JNI_NAME)
#define JniCallFunction(Fname) JNICALL CAT2(FNAME,Fname)




#ifdef __ANDROID__
#define BEAR_SDL_CONST_INIT SDL_INIT_EVERYTHING
#define BEAR_SDL_IMAGE_CONST_INIT IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF
#define SDL_LIB_HEADER <SDL.h>
#define SDL_TTF_LIB_HEADER <SDL_ttf.h>
#define LUA_INCLUDE <lua.hpp>


#elif __EMSCRIPTEN__
#define BEAR_SDL_CONST_INIT SDL_INIT_EVERYTHING
#define BEAR_SDL_IMAGE_CONST_INIT IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF


#define SDL_LIB_HEADER <SDL2/SDL.h>
#define SDL_TTF_LIB_HEADER <SDL_ttf.h>
#define LUA_INCLUDE <stdio.h>
#define DISABLE_LUAINTERFACE
#define DISABLE_THREADPOOL

#elif _WIN32
#define BEAR_SDL_CONST_INIT SDL_INIT_EVERYTHING
#define BEAR_SDL_IMAGE_CONST_INIT IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF

#define SDL_LIB_HEADER "SDL2/SDL.h"
#define SDL_TTF_LIB_HEADER "SDL2/SDL_ttf.h"
#define LUA_INCLUDE <lua.hpp>
#else
#define BEAR_SDL_CONST_INIT SDL_INIT_EVERYTHING
#define BEAR_SDL_IMAGE_CONST_INIT IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF

#define SDL_LIB_HEADER "SDL2/SDL.h"
#define SDL_TTF_LIB_HEADER "SDL2/SDL_ttf.h"
#define LUA_INCLUDE <lua5.1/lua.hpp>

#endif



