#include "typechecker.hpp"
#include "../engine/object.hpp"

int Types::typeCounter = 0;

Types::Types(){

}

Types::~Types(){

}

Types& Types::GetInstance(){
    static Types Singleton;
    return Singleton;
}

bool Types::checkType(GameObject *object1,GameObject *object2){
    return object1->GetHash() == object2->GetHash();
}

bool Types::checkType(GameObject *object1,int hash){
    return object1->GetHash() == hash;
}

