#include <stdio.h>
//Here you have to insert the header from your game!
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif



//Base header
#include "engine/gamebase.hpp"
#include "framework/gamefile.hpp"
#include "settings/configmanager.hpp"


#ifdef __EMSCRIPTEN__
void GameLoop(){
    Game::GetInstance()->Run();
    if (Game::GetInstance()->CanStop()){
        emscripten_cancel_main_loop();
    }
}
#endif

#ifdef __ANDROID__
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

extern "C"
{

    /*JNIEXPORT void JNICALL Java_com_tutorial_game_HelloSDL2Activity_load(JNIEnv *env, jclass clesse, jobject mgr){
        AAssetManager* mgr2 = AAssetManager_fromJava(env, mgr);
        if (mgr2){
            GameFile::m_assetManager = mgr2;
        }
    }*/


}


#endif // __ANDROID__

int main (int argc, char** argv) {

    ConfigManager::GetInstance().RegisterArgs(argc,argv);
    Game::GetInstance("Bear engine test");
    Game::GetInstance()->Begin();
    #ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(GameLoop, 60, 1);
    #else
        while (!Game::GetInstance()->CanStop()){
            Game::GetInstance()->Run();
        }
    #endif
    Game::GetInstance()->Close();
    exit(0);
    return 0;

}
