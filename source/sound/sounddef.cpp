#include "sounddef.hpp"
#include "../engine/bear.hpp"

std::vector<SoundFaderInstance> SoundWorker::FaderList;

SoundWorker::SoundWorker(){

}

SoundWorker::~SoundWorker(){

}

void SoundWorker::Clear(){
    FaderList.clear();
}

void SoundWorker::Update(float dt){
    for (auto &it : FaderList){
        if (it.dead){
            continue;
        }
        if (it.snd.get()){
            it.volume += it.increase*dt;
            if (it.increase > 0){
                if (it.volume >= it.max){
                    it.volume = std::max(it.volume,(float)it.max);
                    it.dead = true;
                }
            }else{
                if (it.volume <= it.max){
                    it.volume = std::min(it.volume,(float)it.max);
                    it.dead = true;
                }
            }

            alSourcef(it.sourceID, AL_GAIN,  it.volume);
            if (it.volume <= 0){
                alSourceStop(it.sourceID);
            }
        }else{
            it.dead = true;
        }
    }
    for (auto it = FaderList.begin(); it != FaderList.end();) {
        if ((*it).dead){
            it = FaderList.erase(it);
        }else{
            it++;
        }
    }
}
