#include "locale.hpp"

Locale::Locale(std::string localeName){
    wordMap.clear();
}


Locale::Locale(uint16_t Key){
    wordMap.clear();
}


std::string Locale::Get(std::string key){
    auto it = wordMap.find(key);
    if (it == wordMap.end()){
        return key;
    }
    return it->second;
}

void Locale::SetKey(std::string key, std::string word){
    wordMap[key] = word;
}
