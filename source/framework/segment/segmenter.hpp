#pragma once
#include "node.hpp"


template<class SegType, int SizeX, int SizeY> class Segmenter : public Segdata{
    public:
        Segmenter(PointInt NodeSize, Point offset = Point(0,0)){
            m_swapLevel = 0;
            m_offset = offset;
            m_nodeSize = NodeSize;
            m_gridSize = PointInt(SizeX, SizeY);
            for (int Y = 0; Y < SizeY; Y++){
                for (int X = 0; X < SizeX; X++){
                    n_nodes[Y][X].Start(this, PointInt(X,Y)) ;
                    m_activeMap[0][ (Y * SizeX) + X ] = false;
                    m_activeMap[1][ (Y * SizeX) + X ] = false;
                }
            }
        };

        void Update(float dt, bool onlyActives = true){
            if (onlyActives){
                for (auto &it : m_active){
                    it->Update(dt);
                }
            }else{
                for (int Y = 0; Y<SizeY;Y++){
                    for (int X = 0; X<SizeX;X++){
                        n_nodes[Y][X].Update(dt);
                    }
                }
            }
        }
        template<typename... Args> bool CallByPos(uint32_t mode,Point pos, Args ... arguments){
            PointInt locate = GetCoordinateByPosition(pos);
            if (InBounds(locate)){
                return n_nodes[locate.y][locate.x].Call(mode, arguments...);
            }
            return false;
        }

        template<typename... Args> bool Call(uint32_t mode, bool onlyActives, Args ... arguments){
            if (onlyActives){
                for (auto &it : m_active){
                    it->Call(mode, arguments...);
                }
            }else{
                for (int Y = 0; Y<SizeY;Y++){
                    for (int X = 0; X<SizeX;X++){
                        n_nodes[Y][X].Call(mode, arguments...);
                    }
                }
            }
            return true;
        }
        bool UpdateActives(Rect area){
            m_nodeSize = n_nodes[0][0].GetSize();
            if (m_nodeSize.x == 0 || m_nodeSize.y == 0){
                return false;
            }
            area.x -= m_offset.x;
            area.y -= m_offset.y;

            float minX = area.x / (float)m_nodeSize.x;
            float minY = area.y / (float)m_nodeSize.y;





            float width = (area.w / (float)m_nodeSize.x);
            float height = (area.h / (float)m_nodeSize.y) ;


            if (width == 0 || height == 0){
                return false;
            }


            if (m_cachedCoordinates.x != minX || m_cachedCoordinates.y != minY || m_cachedCoordinates.w != width || m_cachedCoordinates.h != height){
                m_cachedCoordinates = RectInt(minX, minY, width, height);;
                uint32_t TotalSize = (SizeY * SizeX);
                /**
                    There are two lists of actives.
                    1 and 0.
                    When this function is called, the loop set to 0 all the auxiliar vector
                    Then the auxiliar is filled only where is active.
                    Then the main and auxiliar are compared to know where changed
                    Then m_swapLevel = auxSwap; This means the auxvector become the main vector
                */

                uint32_t auxSwap = m_swapLevel == 0 ? 1 : 0;
                for (uint32_t Id = 0; Id < TotalSize; Id++){
                    m_activeMap[auxSwap][Id] = false;
                }
                m_active.clear();
                for (int32_t Y = minY; Y< (minY+height); Y++){
                    if (Y < 0 || Y >= SizeY)
                        continue;
                    for (int32_t X = minX; X< (minX+width); X++){
                        if (X < 0 || X >= SizeX)
                            continue;
                        m_activeMap[auxSwap][(Y * SizeX) + X] = true;
                        m_active.emplace_back(&n_nodes[Y][X]);
                    }
                }

                for (uint32_t Id = 0; Id < TotalSize; Id++){
                    //If the same are both active or inactive, there is nothing to do.
                    if (m_activeMap[m_swapLevel][Id] != m_activeMap[auxSwap][Id]){
                        uint32_t X = Id % SizeX;
                        uint32_t Y = Id / SizeY;
                        if (m_activeMap[m_swapLevel][Id]){
                            //The older one were active and is not anymore.
                            n_nodes[Y][X].Deactivate(n_nodes);
                        }else{
                            //This means the new swapList is active and the old one were not. Activate!
                            n_nodes[Y][X].Activate(n_nodes);
                        }
                    }
                }
                m_swapLevel = auxSwap;
                return true;
            }else{
                return false;
            }




        };
        SegType &GetNode(int X, int Y){
            return n_nodes[Y][X];
        };
    private:

        RectInt m_cachedCoordinates;
        bool m_activeMap[2][SizeY * SizeX];
        uint32_t m_swapLevel;
        std::vector<SegType*> m_active;
        SegType n_nodes[SizeY][SizeX];
};
