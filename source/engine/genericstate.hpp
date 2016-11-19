
#ifndef STATEH
#define STATEH
#include "object.hpp"
#include "../ui/base.hpp"
#include "../ui/window.hpp"
#include "../framework/basepool.hpp"
#include "../framework/poolmanager.hpp"
#include "assetmanager.hpp"

#include <cstdlib>
#include <iostream>
#include <memory>
#include <vector>
#include <list>

/**
 * @brief Basic game state
 *
 * The GenericState is the most basic game state you have
 *
 * What is a state?\n
 * An state is an class that will describe what the game will do
 * You can have an state for the title, another for the level 1, then one for
 * the boss fight. Its an simple way to divide your game in pieces, then you work
 * in small parts.
 *
 * The functions descibed here are IMPORTANT. you must have them in your state
 *
 * The states work on an stack. Every time you add an state, it will be stacking. Unless you set
 * to the current state to be deleted. When you delete the current state, the new current state will be the last one.
 * if the stack gets empty, the game close.
 *
 *
 * Simples game state that you can have:
 @code

Mystate::Mystate(){
    ParticlePool = new SPP<Particle>(1000);
    requestQuit = requestDelete = false;

    ResourceManager::GetInstance().Load("data/packedFiles.burr","assets");

    background = new Sprite("assets:bg.png");

}


void Mystate::Begin(){

}

Mystate::~Mystate(){
    delete background;
    delete ParticlePool;
    Pool.ErasePools();
    ResourceManager::GetInstance().Erase("assets");
}


void Mystate::Update(float dt){
    if (InputManager::GetInstance().ShouldQuit()){
        requestQuit = true;
    }
    if( InputManager::GetInstance().KeyPress(SDLK_ESCAPE) ) {
        requestQuit = true;
    }
    ParticlePool->UpdateInstances(dt);
    // GenericState::Map
    ParticlePool->PreRender(Map);
}

void Mystate::Render(){
    SDL_SetRenderDrawColor(BearEngine->GetRenderer(), 0,0,0, 0);
    SDL_RenderClear( BearEngine->GetRenderer() );
    background->Render(0,0);
    // GenericState::Map
    for (auto it=Map.begin(); it!=Map.end(); ++it){
        for (auto &k : *it->second){
            k->Render();
        }
        delete it->second;
    }

}

 @endcode

 */

class GenericState{
    public:
        /**
            *The constructor. You dont have to make am empty constructor. But you of course will need one
            *Please check GenericState::Begin
            *
            *<b>Things that you MUST HAVE in your constructor</b>
            @code
                requestDelete = false;
                requestQuit = false;
                ParticlePool = new SPP<Particle>(100);
            @endcode
        */
        GenericState();
        /**
            *Delete everything here
        */
        virtual ~GenericState(){};
        /**
            *Here is where you will make the code that describes your game.
            *Update instances\n
            *Check input\n
            *Check collision\n
            *And so on\n
        */
        virtual void Update(float dt) =0;
        /**
            *Now everything will be shown on screen here.
        */
        virtual void Render() =0;
        /**
            *Now this is tricky. At least on the first state, some things may not have been started yet
            *Like an object pool
            *Its always better start objects here!
        */
        virtual void Begin() =0;

        /**
            *Used by the engine. check if this state is dead;
            *When this flat is set to true, the engine will delete this state.
        */
        bool RequestedDeleted(){return requestDelete;};
        /**
            *Used by the engine. This is a flag to say to the engine "Hey, please, end the game".
        */
        bool RequestedQuit(){return requestQuit;};

        /**
            *Its an basic GameObject pool. SPP<T>
            *Its used by the class ParticleCreator
        */
        SPP<Particle> *ParticlePool;
        /**
            *Render map. It is used to make an "depth" inside the 2D world.
            *Its an map to order the rendering.
            *Lets say, you are in a topdown perspective, you can use the default
            *preRender function:
            *Pool->PreRender(Map);
            *This will fill the map with an ordened array on the Y axis
            *When the function of true Render will be called, the things on top screen
            *will be rendered first.
            *When rendering just use:
            @code
            for (auto it=Map.begin(); it!=Map.end(); ++it){
                for (auto &k : *it->second){
                    k->Render();
                }
                delete it->second;
            }
            @endcode
            *Also you can go to GameObject class, and change the function
            *GameObject::hasPerspective. The less the value, more on "bottom" it will be
        */
        std::map<int,std::vector<GameObject*>> Map;

        void RenderInstances();
        /**
            *Well, you dont need to use pools for the objects at all.
            *Here, there is an vector!
        */
        std::vector<std::unique_ptr<GameObject>> ObjectArray;

        /**
            *You dont want use the object array, and want use pools?\n
            *Dont know how to mess with our pool class SPP<T>?\n
            *Try using our PoolManager =]\n
        */
        PoolManager Pool;
        /**
            *Asset manager
        */
        AssetMannager Assets;
        int STATEID;
        bool gameStarted;
        bool requestDelete,requestQuit;

        void UpdateWindowses(float dt);
        void RenderWindowses();
        void AddWindow(UIBase *b);
    protected:
        friend class Game;
        virtual void Pause(){};
        virtual void Resume(){};

        /**
            *You may need show some windows. Here's a vector to Window components
        */

        std::vector<std::unique_ptr<UIBase>> GameWindows;
    private:
        UIBase *storedUi;

};
#endif // STATEH
