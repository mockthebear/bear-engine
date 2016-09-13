#include "genericstate.hpp"

GenericState::GenericState(){
    requestDelete = requestQuit = false;
    storedUi = nullptr;
    ParticlePool = nullptr;
    STATEID = 0;
    gameStarted = false;
}


void GenericState::UpdateWindowses(float dt){
    if (storedUi != nullptr){
        GameWindows.emplace_back(storedUi);
        storedUi = nullptr;
    }
    for (unsigned i = 0; i < GameWindows.size(); ++i) {
        if (!GameWindows[i]->IsDead())
            GameWindows[i]->Update(dt);
    }

}
void GenericState::RenderWindowses(){
    for (unsigned i = 0; i < GameWindows.size(); ++i) {
        if (!GameWindows[i]->IsDead())
            GameWindows[i]->Render();
    }
}
void GenericState::RenderInstances(){
    for (auto it=Map.begin(); it!=Map.end(); ++it){
        for (auto &k : *it->second){
            k->Render();
        }
        delete it->second;
    }
}
void GenericState::AddWindow(UIBase *b){
    storedUi = b;
}
