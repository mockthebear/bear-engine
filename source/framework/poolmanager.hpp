#include "basepool.hpp"

#ifndef POOLMANAGERHPP
#define POOLMANAGERHPP

#include <vector>
#include <functional>
#include <map>

#define POOL_CAST_MAX(Var)          [=](){return Var->GetMaxInstances();}
#define POOL_CAST_GET(Var)          [=](int index){return Var->GetInstance(index);}
#define POOL_CAST_PRENDER(Var)      [=](std::map<int,std::vector<GameObject*>*> &Map){Var->PreRender(Map);}
#define POOL_CAST_UPDATE(Var)       [=](float dt){Var->UpdateInstances(dt);}
#define POOL_CAST_REMOVE(Var)       [=](){delete Var;}
#define POOL_CAST_ADD(Var,Type)     [=](GameObject *obj){Type *localObj =(Type*)obj; return Var->AddInstance(*localObj);}
#define POOL_CAST_HASH(Var)         [=](){return Var->GetHash();}
#define POOL_CAST(Var,Type)         POOL_CAST_MAX(Var),POOL_CAST_GET(Var),POOL_CAST_REMOVE(Var),POOL_CAST_PRENDER(Var),POOL_CAST_UPDATE(Var),POOL_CAST_ADD(Var,Type),POOL_CAST_HASH(Var)

/**
    @brief Internal class used by PoolManager
    *
    *Only holds lambda functions
    *
*/
typedef struct Holder_s{
    std::function<int(void)> Max;
    std::function<GameObject*(int)> Get;
    std::function<GameObject*(GameObject*)> Add;
    std::function<void(void)> Delete;
    std::function<int(void)> Hash;
    std::function<void(std::map<int,std::vector<GameObject*>*>&)> PRender;
    std::function<void(float)> Update;
    bool Drop;
    uint8_t Index;
}Holder;

typedef int16_t PoolId;
typedef int PoolGroupId;
/**
    @brief Internal class used by PoolManager
    *
    *Only information about pool groups
    *
*/
typedef struct PoolGroup_{
    GameObject **Objects;
    std::vector<PoolId> Pools;
    int Max,Id;
}PoolGroup;



/**
 * @brief Class to manage and unity object pools to make you life easier
 *
 * This class is a bit complex, about what it does.\n
 * Basically you insert static object pools, then it manages all for you, than you can work on all
 * Pools like it was only one. Consider checking GameObject and TypeChecker because this class
 * Heavly uses type cheking things.
 *
 * Every class that inherit from GenericState has on PoolManager
 *
 *When you create your state, you have to register some object pools.
 *Lets say you have the object MyObj and Player objects. Its an pool, so you have to define
 *right the way the maximum amount of instances that will be in that state.
 *
 *We only have one player, but 1000 instances. Should be registered like this:
 @code
 SPP<MyObj>* LocalPool = new SPP<MyObj>(100);
 Pool.RegisterPool(POOL_CAST(LocalPool,MyObj ));
 SPP<Player>* PlayerPool = new SPP<Player>(1);
 Pool.RegisterPool(POOL_CAST(PlayerPool,Player ));
 @endcode
 *The first parameter get the pointer to the Pool.\n
 *The second parameter get the Type.\n
 *<b>Iterating</b>
 @code
    for (int i=0;i<Pool.GetMaxInstances();i++){
        GameObject *obj = Pool.GetInstance(i);
        if (!obj->IsDead()){
            //Do something
        }
    }
 @endcode
 *You can specify One pool to be iterated!
 @code
    for (int i=0;i<Pool.GetInstanceGlobal(poolId);i++){
        GameObject *obj = Pool.GetInstanceGlobal(i,poolId);
        if (!obj->IsDead()){
            //Do something
        }
    }
 @endcode
 *
 *
 *When you add an object, that object will be TypeChecked, then added to the
 *Right pool. Every time you add an object. The manager will run trought the pools
 *Looking only for live objects. Then will make an vector of GameObjects. That vector
 *will be used on PoolManager::GetInstance to get faster an object.
 *This process is automatic, but you can call it by PoolManager::GenerateInternalPool
 *Note that this process cost a bit.
 *
 *That vector of objects is an local vector of the type GameObject**, and it will contains
 *at its generation, only alive objects. When a object dies it may continue in the vector
 *
 *This is made to faster the process of game iteration. Took a toe on the object Add, but fasten the
 *iteration.
 *
 *Lets say, you don't want get all the objects in the poolManager. You only want the player
 *and the monsters. There are two ways, create another poolManager, or pool groups!
 *
 *<b>PoolGroups</b>
 *When called PoolManager::RegisterPool you will get an PoolId. You should save this ID to create an group
 @code
    SPP<MyObj>* LocalPool = new SPP<MyObj>(100);
    PoolId p1 = Pool.RegisterPool(POOL_CAST(LocalPool,MyObj ));
    SPP<Thing>* ThingPool = new SPP<Thing>(100);
    PoolId p2 = Pool.RegisterPool(POOL_CAST(ThingPool,Thing ));
 @endcode
 * We got p1 and p2 ids, so we create a group:
 @code
 std::vector<PoolId> group;
 group.emplace_back(p1);
 group.emplace_back(p2);
 PoolGroupId GID = Pool.CreatePoolGroup(group);
 @endcode
 *Now you can call PoolManager::Update to update all instances in the manager and use the following code to iterate only in the types you want:
 @code
    for (int i=0;i<Pool.GetMaxInstancesGroup(GID);i++){
        GameObject *obj = Pool.GetInstance(i,GID);
        if (!obj->IsDead()){
            //Do something
        }
    }
 @endcode

 */

template<typename T> class GenericIterator{
    public:
        GenericIterator(T **vector_,int size){
            vector = vector_;
            sz = size;
        }
        T** begin(){
            return &vector[0];
        }
        /**
            Used to be the iterator on c++11
        */
        T** end(){
            return &vector[sz];
        }
    private:
        T **vector;
        int sz;
};


class PoolManager{
    public:
        PoolManager(bool insertUnregistered=false);
        /**
            @brief This wont delete the pools!
        */
        ~PoolManager();
        /**
            @brief Register an pool. Check on description for examples
            *
            *When you register a pool on this manager and NEED the pointer to the pool, you should use:
            @code
            SPP<MyObj>* LocalPool = new SPP<MyObj>(100);
            Pool.RegisterPool(POOL_CAST(LocalPool,MyObj ));
            @endcode
            *Forget about all the arguments, use  POOL_CAST instead.\n
            *Its an macro to make everything simpler
            @return When registered a pool, you will get a number, PoolId type. You may save this to use later
        */
        PoolId RegisterPool(std::function<int(void)> maxFuncion,
                              std::function<GameObject*(int)> getFunction,
                              std::function<void(void)> deleteFunction,
                              std::function<void(std::map<int,std::vector<GameObject*>*>&)> preRenderFunction,
                              std::function<void(float)> updateFunction,
                              std::function<GameObject*(GameObject*)> addFunction,
                              std::function<int(void)> hashFuncion,bool DropPool=true);

        /**
            @brief Fast pool register
            Unlike RegisterPool(lots of args...), this one you simply register and get the group id.
            No need to worry about this pool anywhere else, the manager will manage it for you.
            @code
            Pool.Register<Player>(1);
            Pool.Register<MyEnemy>(100);
            Pool.Register<Particles>(800);
            @endcode

        */
        template<typename T> int Register(int size){
            SPP<T>* r_pool = new SPP<T>(size);
            return RegisterPool(POOL_CAST(r_pool,T));
        }

        /**
            *Get the maximum number of instances in the current generated local pool
        */
        int GetMaxInstances();

        /**
            * This will get an object inside the internal optimized vector
        */
        GameObject *GetInstance(int index);

        /**
            *This function will receive an object (not the reference, not pointer).
            *Then will check it type, and search for the correct pool.
            *Once added will be called PoolManager::GenerateInternalPool\n
            *<b>This function may return dead objects</b>
            @param object Any object that have been its pool registered
            @return an pointer to this object <b>inside</b> the pool.
        */


        template<typename T> GameObject* AddInstance(T object){
            GameObject *added = NULL;
            for(int i = 0; i< Pools.size(); ++i){
                if (object.IsHash(Pools[i].Hash())){
                    added = Pools[i].Add(&object);

                    GenerateInternalPool();
                    return added;
                }
            }
            return added;
        }

        int GetLuaPool(){return LuaPool;};
        /**
            This will delete all the pools.
        */
        void ErasePools();
        /**
            Update all the instances in all pools
        */
        void Update(float dt);
        /**
            *Pre Render.
            *Check GenericState::Map
        */
        void PreRender(std::map<int,std::vector<GameObject*>*> &Map);
        /**
            *When this function is called, it will run trough all registered pools looking for alive objects.\n
            *This dont mean that when you have a pool with 10000 object it will run trough all of it.
            *
            *The pools SPP<T> have its own optimization to this don't happens always.
            *
            *When run, it will create an array of GameObjects* containing all ALIVE instances in all pools
            *,Then can be used to fast accessing trough PoolManager::GetInstance
        */
        void GenerateInternalPool();

        /**
            * This function will receive an vector of pool ids. Then will create a internal optimized vector
            * with only the objects of the pools you set
            *
            * May be useful to compare objects like scenario, monster and itens only with player
        */
        PoolGroupId CreatePoolGroup(std::vector<PoolId> pools);
        /**
            Get instances from a group
        */
        GameObject *GetInstanceGroup(int index,PoolGroupId group);
        /**
            Get the maximum instances from that group
        */
        int GetMaxInstancesGroup(PoolGroupId group);
        /**
            *Get an object from a given pool.\n
            *The index may not be the correct id of the object in the pool
            *<b>There is almost no one optimization here, use with care, may be sighly slower than other ways</b>
        */
        GameObject *GetInstanceGlobal(int index,PoolId pool);
        /**
            *Get the maximum instances.\n
            *Note that function may always return 0 if there is no object alive.
            *Once you create an object, it will be set that pool will have 1 alive object
            *So when you have a pool of 100000 instances, you will not have to itarate trought all of it.
            *
            *Basically it calls SPP<T>::GetMaxInstances() for every pool
            *<b>There is almost no one optimization here, use with care, may be sighly slower than other ways</b>
            @param This function get the pool id, will return the amount of objects in a given pool
        */
        int GetMaxInstancesGlobal(PoolId pool);
        /**
            * Dont use this often. Because it will search for all the pools for alive objects and count
            * Instead use PoolManager::GetMaxInstances()
        */
        int GetMaxInstancesOptimized();

        /**
            *Define and poolID to the next iterator
            *Removed automatically
        */
        void SetIteratorPoolId(PoolId pool){nextPoolGroup = pool;};
        /**
            Used to be the iterator on c++11
        */
        GameObject **begin(){
            return &contentList[0];
        }
        /**
            Used to be the iterator on c++11
        */
        GameObject **end(){
            return &contentList[localMaximum];
        }

        GenericIterator<GameObject> ForGroup(PoolGroupId group){
            if (group == -1){
                GenericIterator<GameObject> IteratorF(contentList,localMaximum);
                return IteratorF;
            }
            GenericIterator<GameObject> Iterator(Groups[group].Objects,Groups[group].Max);
            return Iterator;
        }
    private:
        bool insertUnregistered;
        PoolId nextPoolGroup;
        void RemakeGroups();
        int LuaPool;
        GameObject **contentList;
        int localMaximum;
        bool InternalAddInstance(GameObject *);
        uint8_t indexCounter;
        int GroupsCount;
        std::vector<Holder> Pools;
        std::vector<PoolGroup> Groups;
        std::vector<GameObject*> Unregistered;
};


#endif // POOLMANAGERHPP
