#pragma once

#include "../settings/definitions.hpp"

#ifdef __GNUC__
#define WHERE_ARG __FILE__,__PRETTY_FUNCTION__,__LINE__
#else

#define WHERE_ARG __FILE__,__FUNCTION__,__LINE__
#endif



#define AssertE(ptr) AssertAlloc(ptr,WHERE_ARG)
#define Bearssert(ptr) DebugHelper::Assert(ptr,WHERE_ARG,"")
#define Bearssertc(ptr,com) DebugHelper::Assert(ptr,WHERE_ARG,com)

#include <string>
#include <stdlib.h>
/**
    @brief So far, only an assert
    @code
        DebugHelper::AssertAlloc(ptr,WHERE_ARG);
        DebugHelper::Assert(ptr);
    @endcode
*/
class DebugHelper{
    public:
        static void AssertAlloc(void *,std::string file,std::string func,int line,std::string msg="");
        static void Assert(bool p,std::string file,std::string func,int line,std::string msg){
            AssertAlloc((void*)p,file,func,line,msg);
        }
        static void DisplayGlErrFunc(std::string where = "undefined");
};

#ifndef SILENT_GL
    #define DisplayGlError(arg) DebugHelper::DisplayGlErrFunc(arg)
#else
    #define DisplayGlError(arg) ;
#endif // SILENT_GL

