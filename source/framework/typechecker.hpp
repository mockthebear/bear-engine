#ifndef TypesH
#define TypesH

#include <map>
#include <iostream>
#include <string>
#include <typeinfo>
#include "../engine/bear.hpp"
#include "../engine/object.hpp"

#define TYPEOF(var) Types::Get<var>()



class GameObject;

/**
 * @brief Used to check types of objects
 *
 * Use this class to handle run time type checking
 */



class Types{
    public:
        Types();
        ~Types();

        static int typeCounter;

        template<class T,class gameobj> static T *Get(gameobj *obj){
            if (obj && obj->Is(Types::Get<T>()) ){
                return (T*)obj;
            }
            return nullptr;
        }
        template<class T> static int Get(){
            static int cnter = (typeCounter++);
            static bool registered = false;
            if (!registered){
                registered = true;
                GetInstance().Register(cnter,typeid(T).name());
            }
            return cnter;
        };

       static  std::string FormatName(std::string name){
            if (name == ""){
                return name;
            }
            while(name.at(0) >= '0' && name.at(0) <= '9'){
                name.erase(0,1);
            }
            // In visual studio the class name is always something like:
            // [class ClassName]. So this check for a c and a space then remove the "class" from the name
			if (name.at(0) == 'c' && name.at(5) == ' ') {
				name.erase(0, 6);
			}
            return name;
        }

        /**
            Singleton
        */
        static Types& GetInstance();
        /**
            @brief Use as OBJ_REGISTER(type) in GameObject constructor
            *
            *Register an type. Used internal in GameObject constructor
            *
            *Its called from an macro OBJ_REGISTER(type);
        */
        int Register(int hash,std::string name){
            name = FormatName(name);
            if (TypeMap[hash] != ""){
                return 1;
            }
            TypeMap[hash] = name;
            TypeMapi[name] = hash;
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
        template<typename T,typename R> static bool checkType(T *object1){
            return object1->GetHash() == Get<R>();
        }
        template<typename T,typename R> static bool checkType(T &object1){
            return object1.GetHash() == Get<R>();
        }

        template<typename T,typename R> static bool checkType(){
            return Get<T>() == Get<R>();
        }


        template<typename T> bool checkType(T &object1,std::string name){
            name = FormatName(name);
            return object1.GetHash() == TypeMapi[name];
        }

        template<typename T> int getObjectType(T *object1){
            return object1->GetHash();
        }
        template<typename T> int getObjectType(T &object1){
            return object1.GetHash();
        }
        int getTypeId(std::string str){
            str = FormatName(str);
            return TypeMapi[str];
        }

        std::string getTypeName(int id){
            return TypeMap[id];
        }



    private:
        std::map<int,std::string> TypeMap;
        std::map<std::string,int> TypeMapi;
};


#endif // TypesH
