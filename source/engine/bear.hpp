#pragma once

#include <string>

namespace bear {
    class outstream{
        public:
            void printme(int n);
            void printme(const char *c);
    };
    static outstream out;
    const char endl = '\n';


    static inline bear::outstream &operator << ( bear::outstream &o ,int n){
        o.printme(n);
        return o;
    }

    static inline bear::outstream &operator << ( bear::outstream &o ,const char *e){
        o.printme(e);
        return o;
    }

    static inline bear::outstream &operator << ( bear::outstream &o ,std::string str){
        o.printme(str.c_str());
        return o;
    }

    class randomStuff{
        bear::outstream out2;
        randomStuff(int a){
            out2 = out;
        }
    };

}



