#ifndef DIRMANAGERH
#define DIRMANAGERH
#include <string>
/**
    @brief Get the right path in different plataforms
*/
class DirManager{
    public:
        /**
            Get the right asset path using macros from C during compilation
        */
        static std::string AdjustAssetsPath(std::string path);
        static std::string AdjustUserPath(std::string path);
};
#endif // DIRMANAGERH
