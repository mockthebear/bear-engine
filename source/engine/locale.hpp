#pragma once
#include <string>
#include <map>
#include "../settings/definitions.hpp"


class Locale{
    public:
        Locale(std::string location);
        Locale(uint16_t key);

        std::string Get(std::string key);

        void SetKey(std::string key, std::string word);

    private:
        std::map<std::string,std::string> wordMap;
        uint16_t key;
        std::string locationName;
};

