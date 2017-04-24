#include "bear.hpp"
#include <string.h>
#include "../performance/console.hpp"


//auto warning_ignore =  bear::out;

void bear::outstream::printme(int n){
    Console::GetInstance().Store("%d",n);
}
void bear::outstream::printme(unsigned int n){
    Console::GetInstance().Store("%ud",n);
}

void bear::outstream::printme(long unsigned int n){
    Console::GetInstance().Store("%lud",n);
}

void bear::outstream::printme(float n){
    Console::GetInstance().Store("%f",n);
}

void bear::outstream::printme(double n){
    Console::GetInstance().Store("%lf",n);
}


void bear::outstream::printme(const char *c){
    bool found = false;
    char *alt = new char[strlen(c)+1];
    for (uint32_t i=0;i<strlen(c);i++){
        if (c[i] == '\n'){
            alt[i] = '\0';
            found = true;
            break;
        }else{
            alt[i] = c[i];
            alt[i+1] = '\0';
        }
    }
    Console::GetInstance().Store(alt);
    if (found){
        Console::GetInstance().Deploy();
    }
    delete []alt;
}
