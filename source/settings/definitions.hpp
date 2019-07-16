///Core Settings
#include "settings.hpp"
/*

*/

#define BearEngine Game::GetInstance()
#define __BEHAVIOR_FOLDER__ "../game/gamebehavior.hpp"
#define STATE_FILE "../game/state.hpp"
#define DefinedState State
#define LUA_INCLUDE "../luasystem/lua.hpp"
/*
    Web porting
*/
#ifdef __EMSCRIPTEN__
#define ASSETS_FOLDER ""
#define DISABLE_SOCKET
#define DISABLE_THREADPOOL
#else
#define ASSETS_FOLDER ""
#endif

#define DISABLE_SOCKET
//#define DISABLE_THREADPOOL
//#define DISABLE_LUAINTERFACE


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
#define BEAR_SDL_CONST_INIT SDL_INIT_VIDEO
#define BEAR_SDL_IMAGE_CONST_INIT IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF
#define SDL_LIB_HEADER <SDL.h>
#define SDL_TTF_LIB_HEADER <SDL_ttf.h>



#elif __EMSCRIPTEN__
#define BEAR_SDL_CONST_INIT SDL_INIT_EVERYTHING
#define BEAR_SDL_IMAGE_CONST_INIT IMG_INIT_JPG


#define SDL_LIB_HEADER <SDL2/SDL.h>
#define SDL_TTF_LIB_HEADER <SDL_ttf.h>
//#define DISABLE_LUAINTERFACE
#define DISABLE_THREADPOOL

#elif _WIN32
#define BEAR_SDL_CONST_INIT SDL_INIT_VIDEO
#define BEAR_SDL_IMAGE_CONST_INIT IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF

#define SDL_LIB_HEADER <SDL2/SDL.h>
#define SDL_TTF_LIB_HEADER "SDL2/SDL_ttf.h"


#else
#define BEAR_SDL_CONST_INIT SDL_INIT_VIDEO
#define BEAR_SDL_IMAGE_CONST_INIT IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF

#define SDL_LIB_HEADER "SDL2/SDL.h"
#define SDL_TTF_LIB_HEADER "SDL2/SDL_ttf.h"

#endif

#define GLEW_LIB_HEADER <GL/glew.h>



