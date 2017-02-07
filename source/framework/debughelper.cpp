#include "debughelper.hpp"
#include "../engine/bear.hpp"
#include <iostream>
#include <stdio.h>

void DebugHelper::AssertAlloc(void *ptr,std::string file,std::string func,int line){
    if (ptr == NULL){
        bear::out << "Allocation error: 0x0 on " << func << ":"<<line<<"\n\n\n"<<file<<"\nSeg fault incoming!\n";
        getchar();
    }
}
