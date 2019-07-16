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
    PointInt start(
        std::max<int>(vision.x / m_tileSize.x, 0),
        std::max<int>(vision.y / m_tileSize.y, 0)
    );
    PointInt finish(
        std::min<int>((vision.x+vision.w) / m_tileSize.x +1, m_size.x),
        std::min<int>((vision.y+vision.h) / m_tileSize.y +1, m_size.y)
    );
    tileset.BeginRender();
    for (l = 0; l < m_size.z; l++){
        for (y = start.y; y < finish.y; y++){
            for (x = start.x; x < finish.x; x++){
                Tile &t = m_map.get()[CalculateOffset(PointInt3(x,y,l))];
                if (t.id != m_blankTile){
                    Point pos( x * m_tileSize.x + offset.x, y * m_tileSize.y + offset.y );
                    RenderTile(t.id, pos, t.rotate, t.flip);
                }
            }
        }
    }
    tileset.EndRender();
}


void TileMap::SetTile(PointInt3 pos, Tile t){
    uint32_t memOffset = CalculateOffset(pos);
    if (pos.x >= m_size.x || pos.y >= m_size.y || pos.z >= m_size.z){
        bear::out << "Trying to set tile when there is no how."<<pos<<" and the size is "<<m_size<<"\n";
    }
    m_map.get()[memOffset] = t;
}

Tile& TileMap::GetTile(PointInt3 pos){
    static Tile EmptyTile;
    uint32_t memOffset = CalculateOffset(pos);
    if (pos.x >= m_size.x || pos.y >= m_size.y || pos.z >= m_size.z){
        return EmptyTile;
    }
    return m_map.get()[memOffset];
}

uint32_t TileMap::CalculateOffset(PointInt3 pos){
    uint32_t ret = pos.x + (pos.y * m_size.x) + (pos.z * m_size.x * m_size.y);
    return ret;
}
