#include "../../settings/definitions.hpp"

#include "buffertilemap.hpp"
#include "../../framework/segment/node.hpp"
#include "../../framework/segment/segmenter.hpp"
#pragma once


class TilesetNode: public SegmentNode{
    public:
        TilesetNode():doin(false){};

        void Start(Segdata *owner, PointInt location){
            SegmentNode::Start(owner, location);

        };
        void Setup(PointInt tilesize, int layers, Sprite& spr){
            m_tilesPerNode = Point(m_nodeSize.x/tilesize.x, m_nodeSize.y/tilesize.y);
            m_tset = BufferTileMap(tilesize, PointInt3(m_nodeSize.x/tilesize.x, m_nodeSize.y/tilesize.y, layers), spr);
        }

        void Update(float dt){

        }


        bool Call(uint32_t mode, PointInt3 tilePos = PointInt3(0,0,0), Tile t = Tile()){
            if (mode == 0){
                Rect r = GetBox();
                m_tset.Render(r.GetPos());
            }else if(mode == 1){
                tilePos.x -= m_location.x * m_tilesPerNode.x;
                tilePos.y -= m_location.y * m_tilesPerNode.y;
                m_tset.SetTile(tilePos, t);
            }else if(mode == 2){
                m_tset.UpdateBuffers();
            }else if(mode == 3){
                //RenderHelp::DrawSquareColor(GetBox(), 255,255,255,80, true);
            }
            return true;
        }

        template <int SizeX> void Deactivate(TilesetNode seg[][SizeX]){
            doin = false;
        };
        template <int SizeX> void Activate(TilesetNode seg[][SizeX]){
            doin = true;
        };


    private:
        BufferTileMap m_tset;
        bool doin;
        Point m_tilesPerNode;
};



template<int SizeX, int SizeY>class SegmentTileset: public Segmenter<TilesetNode,SizeX,SizeY>{
    public:
        SegmentTileset(PointInt NodeSize, PointInt tilesize, int layers, Sprite& spr, Point offset = Point(0,0)):Segmenter<TilesetNode,SizeX,SizeY>(NodeSize, offset){
            for (int Y = 0; Y<SizeX;Y++){
                for (int X = 0; X<SizeY;X++){
                    Segmenter<TilesetNode,SizeX,SizeY>::GetNode(X,Y).Setup(tilesize, layers, spr);
                }
            }
        }
};


