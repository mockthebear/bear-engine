#pragma once
#define PFDIR 8
#define pf_mapsize 32
#define PFCCELLSIZE 16

#define PF_DIR8X {1, 1, 0, -1, -1, -1, 0, 1}
#define PF_DIR4X {1, 0, -1, 0}


#define PF_DIR8Y {0, 1, 1, 1, 0, -1, -1, -1}
#define PF_DIR4Y {0, 1, 0, -1}

#include "../framework/geometry.hpp"
#include "object.hpp"
#include <queue>
#include <stack>

class PFNode: public Point{
    public:
        int level;
        int priority;
        PFNode():Point(){
            level = 0;
            priority = 0;
        }
        PFNode(int xp, int yp, int d, int p){
            x=(float)xp;
            y= (float)yp;
            priority=p;
        }

        void updatePriority(const int & xd, const int & yd){
             priority=level+GetDistance(Point((float)xd, (float)yd))*10.0f;
        }
};
bool operator<(const PFNode & a, const PFNode & b);


class PathFind{
    public:

        void Close();
        PathFind();
        PathFind(int sizeX,int sizeY,int directions=8,int cellSize=16);
        int &at(int xarg,int yarg){return m_map[xarg][yarg];};
        bool AddBlock(int xarg,int yarg,bool block=true);

        std::stack<Point> Find(PointInt From,PointInt To);

        void Render();

    private:
        PointInt size;
        int **m_map;
        int **m_dir;
        int **m_close;
        int **m_open;

        int directions;
        int cellSize;

        int dx[8];
        int dy[8];


        int pqi;

        PFNode n0;
        PFNode m0;

        int i, j, x, y, xdx, ydy;

        PointInt pf_start;



};


