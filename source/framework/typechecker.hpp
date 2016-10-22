#ifndef TYPECHECKERH
#define TYPECHECKERH

#include <map>
#include <iostream>
#include <string>

#define TYPEOF(var) (int)typeid(var).hash_code()



class GameObject;

/**
 * @brief Used to check types of objects
 *
 * Use this class to handle run time type checking
 */

class TypeChecker{
    public:
        TypeChecker();
        ~TypeChecker();
        /**
            Singleton
        */
        static TypeChecker& GetInstance();
        /**
            @brief Use as OBJ_REGISTER(type) in GameObject constructor
            *
            *Register an type. Used internal in GameObject constructor
            *
            *Its called from an macro OBJ_REGISTER(type);
        */
        template<typename T> int Register(int hash,std::string name,T *object){
            if (TypeMap[hash] != ""){
                return 1;
            }
            TypeMap[hash] = name;
            TypeMapi[name] = hash;
            std::cout << "Registered type " << name << "["<<hash<<"]\n";
            return 0;
        }
        /**
            @brief Compare two object to see if they are the same
            *
            @param object1 Pointer to object 1
            @param object2 Pointer to object 2
            @return true is they are the same type
        */
        bool checkType(GameObject *object1,GameObject *object2);
        /**
            @brief Compare an object to an hash to see if they are the same
            *
            @param object1 Pointer to object 1
            @param hash the hash from an type
            @return true is they are the same type
        */
        bool checkType(GameObject *object1,int hash);
        /**
            @brief Compare two object to see if they are the same
            *
            @param object1 [Template]Reference to object 1
            @param object2 [Template]Reference to object 2
            @return true is they are the same type
        */
        template<typename T,typename T2> bool checkType(T &object1,T2 &object2){
            return object1.GetHash() == object2.GetHash();
        }
        /**
            @brief Compare an object to an hash to see if they are the same
            *
            @param object1 [Template]Reference to object 1
            @param hash the hash from an type
            @return true is they are the same type
        */
        template<typename T> bool checkType(T &object1,int hash){
            return object1.GetHash() == hash;
        }

        /**
            @brief [Template]Compare an object to an hash to see if they are the same
            *
            @param object1 [template]Pointer to object 1
            @param hash the hash from an type
            @return true is they are the same type
        */
        template<typename T> bool checkType(T *object1,int hash){
            return object1->GetHash() == hash;
        }
        /**
            @brief [Template]Compare an object to an string to see if they are the same
            *
            @param object1 [template]Reference to object 1
            @param name typename
            @return true is they are the same type
        */
        template<typename T> bool checkType(T &object1,std::string name){
            return object1.GetHash() == TypeMapi[name];
        }


    private:
        std::map<int,std::string> TypeMap;
        std::map<std::string,int> TypeMapi;
};


#endif // TYPECHECKERH
