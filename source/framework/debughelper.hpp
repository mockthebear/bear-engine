#ifndef DEBUGHELPERBE
#define DEBUGHELPERBE
#ifdef __GNUC__
#define WHERE_ARG __FILE__,__PRETTY_FUNCTION__,__LINE__
#else

#define WHERE_ARG __FILE__,__FUNCTION__,__LINE__
#endif

#define AssertE(ptr) AssertAlloc(ptr,WHERE_ARG)

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
        static void AssertAlloc(void *,std::string file,std::string func,int line);
};
#endif // DEBUGHELPERBE
