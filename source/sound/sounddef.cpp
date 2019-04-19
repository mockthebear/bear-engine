#include "sounddef.hpp"
#include "../engine/bear.hpp"
#include "soundloader.hpp"
#include "../framework/utils.hpp"

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
			//bear::out << it.volume << "\n";
			it.volume = std::max(0.0f, it.volume);
			it.volume = std::min(1.0f, it.volume);
            alSourcef(it.sourceID, AL_GAIN,  it.volume  );
			//SoundLoader::ShowError( utils::format("Set gain as %f", it.volume) );
            if (it.volume <= 0){
                alSourceStop(it.sourceID);
				it.dead = true;
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
