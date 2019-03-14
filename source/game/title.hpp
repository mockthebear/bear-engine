#ifndef TitleH
#define TitleH

#include "state.hpp"
#include <stack>
#include "../performance/console.hpp"
#include "../performance/graph.hpp"
#include "../engine/pathfind.hpp"
#include "../engine/tiles/buffertilemap.hpp"
class Title: public State{
    public:
        Title();
        ~Title();
        void Update(float);
        void Render();
        void Input();
        void Begin();
        void Resume(GenericState *s){std::cout << "Resumed\n\n";};
        void End(){std::cout << "End\n\n";};
        void Pause(GenericState *s){std::cout << "Paused\n\n";};

        void BorderTile(PointInt3 pos,bool side);
    private:
        BufferTileMap btset;
        BearPolygon region;

};
#endif
