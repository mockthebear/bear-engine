#pragma once

#include <string>
#include <typeindex>
#include "../framework/geometry.hpp"
#include "../glm/glm.hpp"
/**
    @brief Custom outstream from bear engine
*/


namespace bear {
    class outstream{
        public:
            void printme(int n);
            void printme(unsigned int n);
            void printme(long unsigned int n);
            //void printme(uint64_t n);
            void printme(glm::mat4 &m);
            void printme(float n);
            void printme(double n);
            void printme(const char *c);
            void printme(char *c);
            void printme(void *c);
    };
    static outstream out;
    const char endl = '\n';

    /**
        Int specializer
    */
    static inline bear::outstream &operator << ( bear::outstream &o ,int n){
        o.printme(n);
        return o;
    }
    /**
        unsigned Int specializer
    */
    static inline bear::outstream &operator << ( bear::outstream &o ,unsigned int n){
        o.printme(n);
        return o;
    }
    /**
        long unsigned Int specializer
    */
    static inline bear::outstream &operator << ( bear::outstream &o ,long unsigned int n){
        o.printme(n);
        return o;
    }
    /**
        float specializer
    */
    static inline bear::outstream &operator << ( bear::outstream &o ,float n){
        o.printme(n);
        return o;
    }
    /**
        double specializer
    */
    static inline bear::outstream &operator << ( bear::outstream &o ,double n){
        o.printme(n);
        return o;
    }
    /**
        const char specializer
    */
    static inline bear::outstream &operator << ( bear::outstream &o ,char *e){
        o.printme(e);
        return o;
    }
    /**
        const char specializer
    */
    static inline bear::outstream &operator << ( bear::outstream &o ,const char *e){
        o.printme(e);
        return o;
    }
    /**
        std::string specializer
    */
    static inline bear::outstream &operator << ( bear::outstream &o ,std::string str){
        o.printme(str.c_str());
        return o;
    }

    static inline bear::outstream &operator << ( bear::outstream &o ,glm::mat4 m4){
        o.printme(m4);
        return o;
    }
    /**
        generic point specializer
    */
    template<typename T> static inline bear::outstream &operator << ( bear::outstream &o ,GenericPoint<T> pointPos){
        o.printme("(");
        o.printme(pointPos.x);
        o.printme(", ");
        o.printme(pointPos.y);
        o.printme(")");

        return o;
    }

    template<typename T> static inline bear::outstream &operator << ( bear::outstream &o ,GenericPoint3<T> pointPos){
        o.printme("(");
        o.printme(pointPos.x);
        o.printme(", ");
        o.printme(pointPos.y);
        o.printme(", ");
        o.printme(pointPos.z);
        o.printme(")");

        return o;
    }

    template<typename T> static inline bear::outstream &operator << ( bear::outstream &o ,GenericRect<T> pointPos){
        o.printme("(");
        o.printme(pointPos.x);
        o.printme(", ");
        o.printme(pointPos.y);
        o.printme(", ");
        o.printme(pointPos.w);
        o.printme(", ");
        o.printme(pointPos.h);
        o.printme(")");

        return o;
    }


    template<typename T> static inline bear::outstream &operator << ( bear::outstream &o ,T *n){
        o.printme(typeid(T).name());
        o.printme("_ptr");
        o.printme(static_cast<void*>(n));
        return o;
    }

    /**
        used to avoid warnings.
    */

    class randomStuff{
        bear::outstream out2;
        randomStuff(int a):out2(out){
			(void)a;
        }
    };

    #ifdef MINGW32
    #define lout out << "[" << utils::methodName(__PRETTY_FUNCTION__) << " " <<__LINE__ << "]"
    #else
    #define lout out << "[" << utils::methodName(__FUNCTION__) << " " <<__LINE__ << "]"
    #endif



}

//#define bear::location "[" << utils::methodName(__FUNCTION__) << __LINE__ << "]"





