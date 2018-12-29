#pragma once
#include "../geometry.hpp"

class SegmentNode{
    public:
        virtual void Start(int32_t id, PointInt size, PointInt location) = 0;

        virtual void Update(float dt) = 0;

        /**
            Must contain a
            virtual bool Call(uint32_t mode, ... your arguments here) = 0;
        */

        virtual void Deactivate(){};
        virtual void Activate(){};

        PointInt GetSize(){return m_size;};
    protected:
        int32_t m_id;
        PointInt m_size;
};
