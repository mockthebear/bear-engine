#include "typechecker.hpp"
#include "../engine/object.hpp"

int TypeChecker::typeCounter = 0;

TypeChecker::TypeChecker(){

}

TypeChecker::~TypeChecker(){

}

TypeChecker& TypeChecker::GetInstance(){
    static TypeChecker Singleton;
    return Singleton;
}

bool TypeChecker::checkType(GameObject *object1,GameObject *object2){
    return object1->GetHash() == object2->GetHash();
}

bool TypeChecker::checkType(GameObject *object1,int hash){
    return object1->GetHash() == hash;
}

