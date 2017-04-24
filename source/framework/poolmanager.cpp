#include "poolmanager.hpp"
#include "../luasystem/luaobject.hpp"
#include "../engine/bear.hpp"
#include "../engine/timer.hpp"

PoolManager::PoolManager(bool insertUnregistered){

    LuaPool = -1;
    contentList = new GameObject*[10];
    localMaximum = 0;
    GroupsCount = 0;
    nextPoolGroup = 255;
    this->insertUnregistered = true;
    indexCounter = insertUnregistered ? 1 : 0;
}

PoolManager::~PoolManager(){
    delete []contentList;
    bear::out << "Erasing pools.\n";
    for(unsigned int i = 0; i< Pools.size(); ++i){
        if (Pools[i].Drop)
            Pools[i].Delete();
    }
    bear::out << "Pools deleted.\n";
}

PoolId PoolManager::RegisterPool(std::function<int(void)> maxFuncion,
                              std::function<GameObject*(int)> getFunction,
                              std::function<void(void)> deleteFunction,
                              std::function<void(std::map<int,std::vector<GameObject*>>&)> preRenderFunction,
                              std::function<void(float)> updateFunction,
                              std::function<GameObject*(GameObject*)> addFunction,
                              std::function<int(void)> hashFuncion,bool DropPool)
{

    Holder newHolder;
    newHolder.Max       = maxFuncion;
    newHolder.Get       = getFunction;
    newHolder.Delete    = deleteFunction;
    newHolder.PRender   = preRenderFunction;
    newHolder.Update    = updateFunction;
    newHolder.Add       = addFunction;
    newHolder.Hash      = hashFuncion;
    newHolder.Index     = indexCounter;
    newHolder.Drop      = DropPool;
    Pools.emplace_back(newHolder);
    indexCounter++;
    GenerateInternalPool();
    return indexCounter-1;
}

void PoolManager::ErasePools(){
    for(unsigned int i = 0; i< Pools.size(); ++i){
        Pools[i].Delete();
    }
    Pools.clear();
}
GameObject *PoolManager::InternalAddInstance(GameObject *obj,bool deleteptr){
    for(unsigned int i = 0; i< Pools.size(); ++i){
        if (obj->IsHash(Pools[i].Hash())){
            GameObject *add = Pools[i].Add(obj);
            if (deleteptr)
                delete obj;
            GenerateInternalPool();
            return add;
        }
    }
    if (insertUnregistered){
        Unregistered.emplace_back(obj);
        GenerateInternalPool();
        return obj;
    }else{
        bear::out << "Cannot add object with PoolManager::insertUnregistered as false.\n";
        int type = Types::GetInstance().getObjectType(obj);
        bear::out << "Object typeid is "<< type << " named as " << Types::GetInstance().getTypeName(type) << "\n";
        return nullptr;
    }
}

void PoolManager::GenerateInternalPool(){
    int localMaximum_aux = GetMaxInstancesOptimized()+Unregistered.size();
    if (localMaximum_aux > localMaximum){
        delete []contentList;
        contentList = new GameObject*[localMaximum_aux+1];
    }
    localMaximum = localMaximum_aux;
    int count = 0;
    for(unsigned int i = 0; i< Pools.size(); ++i){
        for (int e=0;e<Pools[i].Max();e++){
            GameObject *obj = Pools[i].Get(e);
            if (obj != NULL && !obj->IsDead()){
                contentList[count] = obj;
                count++;
            }
        }
    }
    for(unsigned int i = 0; i< Unregistered.size(); ++i){
        if (Unregistered[i].get() != NULL && !Unregistered[i]->IsDead()){
            contentList[count] = Unregistered[i].get();
            count++;
        }
    }

    RemakeGroups();
}


PoolGroupId PoolManager::CreatePoolGroup(std::vector<PoolId> poolsGroup){
    PoolGroup localGroup;
    int maxObjects=0;
    std::vector<PoolId> ListPools;
    for(unsigned int i = 0; i< poolsGroup.size(); ++i){
        if (Pools[poolsGroup[i]].Index == poolsGroup[i]){
            localGroup.Pools.emplace_back(poolsGroup[i]);
            maxObjects += Pools[poolsGroup[i]].Max();
        }
    }
    localGroup.Max = maxObjects;
    localGroup.Objects = new GameObject*[localGroup.Max+1];
    localGroup.Id = GroupsCount;
    Groups.emplace_back(localGroup);
    RemakeGroups();
    GroupsCount++;
    return GroupsCount-1;
}
int PoolManager::GetMaxInstancesGroup(PoolGroupId group){
    return Groups[group].Max;
}

GameObject *PoolManager::GetInstanceGroup(int index,PoolGroupId group){
    return Groups[group].Objects[index];
}

void PoolManager::RemakeGroups(){
    for(unsigned int i = 0; i< Groups.size(); ++i){
        int newMax = 0;
        for(unsigned int e = 0; e< Groups[i].Pools.size(); ++e){
            int PoolIndex = Groups[i].Pools[e];
            for (int a=0;a<Pools[PoolIndex].Max();a++){
                GameObject *obj = Pools[PoolIndex].Get(a);
                if (obj != NULL && !obj->IsDead()){
                    newMax++;
                }
            }

        }
        if (newMax > Groups[i].Max){
            delete []Groups[i].Objects;
            Groups[i].Objects = new GameObject*[newMax+1];
        }
        Groups[i].Max = newMax;
        int theIndex=0;
        /**
            Insert objects
        */
        for(unsigned int e = 0; e< Groups[i].Pools.size(); ++e){
            int PoolIndex = Groups[i].Pools[e];
            for (int poolid = 0;poolid<Pools[PoolIndex].Max();poolid++){
                GameObject *obj = Pools[PoolIndex].Get(poolid);
                if (obj != NULL && !obj->IsDead()){
                    Groups[i].Objects[theIndex] = obj;
                    theIndex++;
                }
            }
        }
    }
}

int PoolManager::GetMaxInstancesOptimized(){
    int count = 0;
    for(unsigned int i = 0; i< Pools.size(); ++i){
        for (int e=0;e<Pools[i].Max();e++){
            GameObject *obj = Pools[i].Get(e);
            if (obj != NULL && !obj->IsDead()){
                count++;
            }
        }
    }
    return count;
}

int PoolManager::GetMaxInstances(){
    return localMaximum;
}
GameObject * PoolManager::GetInstance(int index){
    return contentList[index];
}

int PoolManager::GetMaxInstancesGlobal(PoolId pool){
    int count = 0;
    if (pool > -1){
        return Pools[pool].Max();
    }
    for(unsigned int i = 0; i< Pools.size(); ++i){
        count += Pools[i].Max();
    }
    return count;
}

GameObject * PoolManager::GetInstanceGlobal(int index,PoolId pool){
    GameObject *found = NULL;
    if (pool <= -1){
        for(unsigned int i = 0; i< Pools.size(); ++i){
            if (index <= Pools[i].Max()){
                found = Pools[i].Get(index);
                break;
            }else{
                index = index-Pools[i].Max();
            }
        }
    }else{
        found = Pools[pool].Get(index);
    }
    return found;
}

void PoolManager::Update(float dt){
    unsigned int t = 0;
    unsigned int id = 0;
    for(unsigned int i = 0; i< Pools.size(); ++i){
        Stopwatch sw;
        Pools[i].Update(dt);
        if (t < sw.Get()){
            t = sw.Get();
            id = i;
        }
    }
    Stopwatch sw;
    for (auto &it : Unregistered){

        it->Update(dt);

    }
    if (t < sw.Get()){
            t = sw.Get();
            id = 1337;
        }
    if (t > 8)
        bear::out << "big: " << t << " and " << id << " = "<<Types::GetInstance().getTypeName(Pools[id].Hash())<<"\n";
}


void PoolManager::PreRender(std::map<int,std::vector<GameObject*>> &Map){
    for(unsigned int i = 0; i< Pools.size(); ++i){
        Pools[i].PRender(Map);
    }
    for (auto &it : Unregistered){
        if (!it->IsDead() && (Camera::EffectArea.IsInside(it->box) || it->canForceRender())  ){
            if (it->hasPerspective() == 0){
                int posy = it->box.y;

                Map[posy].emplace_back( it.get());

            }else{
                int persp = it->hasPerspective();
                Map[persp].emplace_back( it.get());
            }
        }

    }
}
