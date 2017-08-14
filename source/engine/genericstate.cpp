#include "genericstate.hpp"

GenericState::GenericState(){
    requestDelete = requestQuit = false;
    storedUi = nullptr;
    STATEID = -1;
    ParticlePool = nullptr;
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
    for (auto & w : GameWindows) {
        if (!w->IsDead())
            w->Render();
    }
}

void GenericState::RenderInstances(){
    for (auto &it : Map){
        for (auto &k : it.second){
            k->Render();
        }
    }
}
void GenericState::AddWindow(UIBase *b){
    storedUi = b;
}
