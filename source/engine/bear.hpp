#pragma once

#include <string>
#include "../framework/geometry.hpp"
/**
    @brief Custom outstream from bear engine
*/
namespace bear {
    class outstream{
        public:
            void printme(int n);
            void printme(unsigned int n);
            void printme(long unsigned int n);
            void printme(float n);
            void printme(double n);
            void printme(const char *c);
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
    /**
        generic point specializer
    */
    template<typename T> static inline bear::outstream &operator << ( bear::outstream &o ,GenericPoint<T> pointPos){
        o.printme("(");
        o.printme(pointPos.x);
        o.printme(",");
        o.printme(pointPos.y);
        o.printme(")");

        return o;
    }
    /**
        used to avoid warnings.
    */
    class randomStuff{
        bear::outstream out2;
        randomStuff(int a){
			(void)a;
			out2 = out;
        }
    };

}



