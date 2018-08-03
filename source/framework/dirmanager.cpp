#include "dirmanager.hpp"
#include <sstream>
#include "../settings/definitions.hpp"
#include SDL_LIB_HEADER


std::string DirManager::AdjustAssetsPath(std::string path){
    std::string true_path;
    #ifdef __EMSCRIPTEN__
    true_path = ASSETS_FOLDER + path;
    #else
    true_path = SDL_GetBasePath() + path;
    #endif
    return true_path;
}


std::string DirManager::AdjustUserPath(std::string path){
    std::string true_path;

    #ifdef __ANDROID__
    std::stringstream S;
    S << SDL_AndroidGetExternalStoragePath() <<"/"<< path;
    true_path = S.str();
    #else
    true_path = std::string(ASSETS_FOLDER) + std::string(SDL_GetBasePath()) +  path;
    #endif
    return true_path;
}
