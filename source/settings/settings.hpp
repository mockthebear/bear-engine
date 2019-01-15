/**
    Bear engine config file.

        THIS FILE IS WHERE YOU SET STUFF
*/
/**
    General
*/
    #define APPNAME "Bear engine"
/**
    Screen and display
*/
    #define SCREEN_SIZE_W 800
    #define SCREEN_SIZE_H 600

    #define MAXFPS 900.0

    //#define RENDER_OPENGL
    //#define RENDER_OPENGL3

/**
    Features & services
*/
    //#define SILENT_GL

    #define POOL_DEFAULT_THREADS 2
    //#define DISABLE_UNIT_TESTS

    //#define DISABLE_THREADPOOL

    //#define DISABLE_LUAINTERFACE

    //#define DISABLE_SOCKET

/**
    Android things
*/
    #define JNI_BASE Java
    #define JNI_COM com
    #define JNI_MODULE tutorial
    #define JNI_FROM game
    #define JNI_NAME HelloSDL2Activity
