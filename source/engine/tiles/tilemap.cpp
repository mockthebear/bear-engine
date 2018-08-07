#include "tilemap.hpp"
#include "../bear.hpp"


TileMap::TileMap(){
    m_blankTile = 0;
}

TileMap::TileMap(PointInt tileSize,PointInt3 mapSize, Sprite set):TileSet(tileSize,set){
    m_blankTile = 0;
    Tile *lst = new Tile[mapSize.z * mapSize.x * mapSize.y];
    m_map = std::shared_ptr<Tile>(lst, [](Tile *p) {
           delete [] p;
    });
    m_size = mapSize;
}

void TileMap::Render(Point offset, Rect vision){
    int l,y,x;

    /*
        todo vision camp
    */
    for (l = 0; l < m_size.z; l++){
        for (y = 0; y < m_size.y; y++){
            for (x = 0; x < m_size.x; x++){
                Tile &t = m_map.get()[CalculateOffset(PointInt3(x,y,l))];
                if (t.id != m_blankTile){
                    Point pos( x * m_tileSize.x + offset.x, y * m_tileSize.y + offset.y );
                    RenderTile(t.id, pos, t.rotate, t.flip);
                }
            }
        }
    }
}


void TileMap::SetTile(PointInt3 pos, Tile t){
    uint32_t memOffset = CalculateOffset(pos);
    if (pos.x >= m_size.x || pos.y >= m_size.y || pos.z >= m_size.z){
        bear::out << "Trying to set tile when there is no how."<<pos<<"\n";
    }
    m_map.get()[memOffset] = t;
}

uint32_t TileMap::CalculateOffset(PointInt3 pos){
    uint32_t ret = pos.x + (pos.y * m_size.x) + (pos.z * m_size.x * m_size.y);
    return ret;
}
