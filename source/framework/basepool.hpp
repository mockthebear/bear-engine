#ifndef BASEPOOLHH
#define BASEPOOLHH
#include <map>
#include <vector>
#include <iostream>
#include "typechecker.hpp"
#include "../engine/bear.hpp"
#include "../engine/object.hpp"
#include "../engine/camera.hpp"
#include "../engine/particlecreator.hpp"
/**
 * @brief Object pool
 *
 * SPP means Some Potato Pool
 * Just kidding. There is no meaning.
 *
 * Made to manage many objects of the same type. When created, it receives the amount
 * of objects to be alloc, then it can be managed.
 *
 * Why is better?
 * http://gameprogrammingpatterns.com/object-pool.html
 * And you rarely will have to deal with an deleted pointer or nullptr
 */
template <typename T> class SPP{
    public:
        friend class GameObject;
        /**
           @brief  Constructor
           *
           *When created, it will take the first object to check some things and know what object type is.
           *You may get an error when you dont register the object type in GameObject
        */
        SPP(int size){
            maxCount = 0;
            lastAdded = 0;
            counter = 0;

            max = size;
            pool = new T[size+1];
            if (!pool){
                bear::out << "[Basepool:create] Error. Could not alloc "<<size<<" objects.\n";
                return;
            }
            Type = Types::Get<T>();
            for (int in=0;in<=size;in++){
                pool[in].poolIndex = in;
                if (!pool[in].IsDead())
                    pool[in].NotifyInPool(this);
                if (pool[in].GetHash() != Type){
                    bear::out << "[Basepool:create] There is something wrong about the type "<< typeid(T).name() <<". You forgot to set OBJ_REGISTER() in creator "<<pool[in].GetHash()<< "x "<< Type <<"?\n";
                }
            }
        };
        /**
            The destructor will trully delete any of the objects.
        */
        ~SPP(){
            #ifdef DEBUG_ON_DELETE
            bear::out << "[Basepool:delete]\n";
            #endif // DEBUG_ON_DELETE

            delete []pool;
            #ifdef DEBUG_ON_DELETE
            bear::out << "[/Basepool:delete]\n";
            #endif // DEBUG_ON_DELETE

        };
        /**
            *This will pre render the objects at the Map vector.*
            *To understand better check on GenericState::Map
            *
            *It is here where the engine will make the selection, who will be rendered first
            *and who will be rendered last.
            @param Map an reference to the GenericState::Map std::map<int,std::vector<GameObject*>*>
        */
        void PreRender(std::map<int,std::vector<GameObject*>> &Map){
            for (int i=0;i<GetMaxInstances();i++){
                if (!pool[i].IsDead()){
                    if ( Camera::EffectArea.IsInside(pool[i].box) || pool[i].canForceRender()  ){
                        if (pool[i].hasPerspective() == 0){
                            int posy = pool[i].box.y;
                            Map[posy].emplace_back( &pool[i]);

                        }else{

                            int persp = pool[i].hasPerspective();
                            Map[persp].emplace_back( &pool[i]);
                        }
                    }
                }
            }
        }
        /**
            *Add an instance in the pool
            *
            *This function has an tricky thing. When you add the first object, it will set the size of the pool to 1.
            *Any iteration on it will search until the last one added.
            *If your'e trying to add an object, it will try add right next to the last added.
            *Unless you don't fill this pool, it will be preety much faster.
            *
            @param instance An reference to the object
            @param ignoreLiving will get the position of the last added object, then will place the new object +1 position
            it will not check if the next object is alive.
            @return if the pool is full, the return will be NULL, otherwise will be a pointer to the added object in the pool

        */
        T* AddInstance(T instance,bool ignoreLiving = false){
            bool added = false;
            int n=0;

            if (ignoreLiving && lastAdded < max){
                int e = lastAdded;
                pool[e] = instance;
                pool[e].poolIndex = e;
                n = e;
                lastAdded++;
                maxCount = std::max(e,maxCount-1);
                added = true;
            }else{
                for (int e=0;e<max;e++){
                    if (pool[e].IsDead()){
                        pool[e] = instance;
                        pool[e].poolIndex = e;
                        pool[e].NotifyInPool(this);
                        n = e;
                        if (e > lastAdded){
                            lastAdded = e;
                        }

                        maxCount = std::max(e,maxCount-1);
                        added = true;
                        break;
                    }
                }
            }
            if (added == false){
                bear::out << "[ERROR] Maximum pool reached - " << Types::GetInstance().getTypeName(TYPEOF(T)) <<" [" << lastAdded <<":"<<max<<"]\n";
                return NULL;
            }else{
                return &pool[n];
            }
        };

        /**
            *Will update all the alive, and all that are inside the Camera::UpdateArea
            *or the ones who have GameObject::canForceUpdate equals to true
            *
            *This function will run until the last one added.

            @param dt time between the last update
        */
        void UpdateInstances(float dt){

            for (int i=0;i<GetMaxInstances();i++){ //Maximo adcionado
                if (!pool[i].IsDead()){

                   if (pool[i].canForceUpdate() || Camera::UpdateArea.IsInside(pool[i].box)){
                        pool[i].Update(dt);
                   }
                }
            }

        };
        /**
            *Will update all the alive, and all that are inside the Camera::UpdateArea
            *or the ones who have GameObject::canForceUpdate equals to true
            *
            *This function will run until <b>the end of the pool</b>

            @param dt time between the last update

        */
        void UpdateInstancesForced(float dt){
            for (int i=0;i<max;i++){ //Maximo adcionado
                if (!pool[i].IsDead()){
                   if (Camera::UpdateArea.IsInside(pool[i].box) || pool[i].canForceUpdate() ){
                        pool[i].Update(dt);
                   }
                }
            }

        };
        /**
            *This may return 0 because the pool have the amount of objects that you created
            *But unless you add an object, this return will return 0. Its because an optimization.
            *
            *The pool its created with the size N, but çwhen started, it size is limited by the code to 0.
            *When you add an object with SPP<T>::AddInstance, an value "lastAdded" will be increased and the object will be added on this
            *position. Then the return will be lastAdded of this function.
        */
       int GetMaxInstances(){
           return std::min(lastAdded+1,max);
       };
       /**
            *Get the object in the pool with a given index.
        */
       GameObject *GetInstance(int i){
            if (i > max || i < 0){
                bear::out << "[ERROR] Required more than alloc. " << i << " / " << max << "\n";
                return NULL;
            }
            return (GameObject*)&pool[i];
       };
       /**
            *Get the object hash from that pool. Check GameObject::GetHash
        */
       int GetHash(){
           return Type;
       }
    private:
        int Type;
        T *pool;
        int maxCount,lastAdded,counter,max;
};


#endif
