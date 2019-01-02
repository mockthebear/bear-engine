#pragma once
#include "../geometry.hpp"
#include "../utils.hpp"

class SegmentNode;

typedef utils::MatN<SegmentNode, 2> NodeMatrix;

class Segdata{
    public:
        PointInt m_gridSize;
        PointInt m_nodeSize;
        Point m_offset;

        bool InBounds(PointInt pos){
            return pos.x >= 0 && pos.x < m_gridSize.x && pos.y >= 0 && pos.y < m_gridSize.y;
        }
        PointInt GetCoordinateByPosition(Point pos){
            pos.x -= m_offset.x;
            pos.y -= m_offset.y;
            if (pos.x < 0){
                pos.x -= m_nodeSize.x;
            }
            if (pos.y < 0){
                pos.y -= m_nodeSize.y;
            }
            return PointInt(pos.x / m_nodeSize.x, pos.y / m_nodeSize.y);
        }
};

class SegmentNode: public Segdata{
    public:
        virtual void Start(Segdata *owner, PointInt location){
            m_owner = owner;
            m_location = location;
            m_id = location.x + location.y * m_owner->m_nodeSize.x;
            m_nodeSize = m_owner->m_nodeSize;
            m_gridSize = m_owner->m_gridSize;
            m_offset = m_owner->m_offset;
        };

        virtual void Update(float dt) = 0;

        /**
            Must contain one of each
            bool Call(uint32_t mode, ... your arguments here);
            template <int SizeX> void Deactivate(DummyNode seg[][SizeX]);
            template <int SizeX> void Aactivate(DummyNode seg[][SizeX]);
        */

        //virtual void Deactivate(SegmentNode *seg){};
        //virtual void Activate(SegmentNode *seg){};

        PointInt GetSize(){return m_nodeSize;};
        int32_t GetId(){return m_id;};
        Rect GetBox(){
            return Rect(m_nodeSize.x*m_location.x + m_offset.x, m_nodeSize.y*m_location.y + m_offset.y, m_nodeSize.x, m_nodeSize.y);
        }


    protected:
        Segdata *m_owner;
        int32_t m_id;
        PointInt m_location;
};


