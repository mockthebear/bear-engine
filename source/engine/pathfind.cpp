#include "pathfind.hpp"
#include "renderhelp.hpp"


bool operator<(const PFNode & a, const PFNode & b)
{
    return a.priority > b.priority;
}


PathFind::PathFind(){
    m_map = nullptr;
    m_dir = nullptr;
    m_close = nullptr;
    m_open = nullptr;
}

PathFind::PathFind(int sizeX,int sizeY,int dirs,int cells):PathFind(){
    sizeX = sizeX/cells;
    sizeY = sizeY/cells;

    m_map   = new int*[sizeX];
    m_dir   = new int*[sizeX];
    m_close = new int*[sizeX];
    m_open  = new int*[sizeX];
    for (int y=0;y<sizeX;y++){
        m_map[y] = new int[sizeY];
        m_dir[y] = new int[sizeY];
        m_close[y] = new int[sizeY];
        m_open[y] = new int[sizeY];
    }
    size.x = sizeX;
    size.y = sizeY;
    directions = dirs;
    cellSize = cells;
    if (directions == 4){
        int cx[] = PF_DIR4X;
        int cy[] = PF_DIR4Y;
        for (int i=0;i<directions;i++){
            dx[i] = cx[i];
            dy[i] = cy[i];
        }
    }else{
        directions = 8;
        int cx[] = PF_DIR8X;
        int cy[] = PF_DIR8Y;
        for (int i=0;i<directions;i++){
            dx[i] = cx[i];
            dy[i] = cy[i];
        }
    }
    for(y=0;y<size.y;y++){
        for(x=0;x<size.x;x++){
            m_close[x][y]   =0;
            m_open[x][y]    =0;
            m_dir[x][y]    = 0;
            m_map[x][y]    = 0;
        }
    }
}

void PathFind::Close(){
    if (m_map != nullptr){
        for (int y=0;y<size.x;y++){
            delete m_map[y];
            delete m_dir[y];
            delete m_close[y];
            delete m_open[y];
        }
        delete m_map;
        delete m_dir;
        delete m_close;
        delete m_open;
    }
    m_map = nullptr;
}
void PathFind::Render(){
    for(y=0;y<size.y;y++){
        for(x=0;x<size.x;x++){
            if (m_map[x][y]){
                RenderHelp::DrawSquareColorA(x*cellSize,y*cellSize,cellSize,cellSize,100,100,100,200);
                RenderHelp::DrawSquareColorA(x*cellSize,y*cellSize,cellSize,cellSize,255,100,100,200,true);
            }
        }
    }
}
bool PathFind::AddBlock(int bx,int by,bool block){
    PointInt pf_pos = PointInt( floor( bx - (int)bx%(int)cellSize ), floor( by - (int)by%(int)cellSize ));
    pf_pos.x /= cellSize;
    pf_pos.y /= cellSize;
    m_map[pf_pos.x][pf_pos.y] = block;
}
std::stack<Point> PathFind::Find(PointInt A,PointInt B){
    pf_start = PointInt( floor( A.x - (int)A.x%(int)cellSize ), floor( A.y - (int)A.y%(int)cellSize ));
    PointInt pf_end = PointInt( floor( B.x - (int)B.x%(int)cellSize ), floor( B.y - (int)B.y%(int)cellSize ));

    pf_start.x /= cellSize;
    pf_start.y /= cellSize;
    pf_end.x /= cellSize;
    pf_end.y /= cellSize;
    std::stack<Point> path;
    pqi=0;
    std::priority_queue<PFNode> pq[2];
    for(y=0;y<size.y;y++){
        for(x=0;x<size.x;x++){
            m_close[x][y]   =0;
            m_open[x][y]    =0;
            m_dir[x][y]    = 0;
        }
    }

    n0  = PFNode(pf_start.x, pf_start.y, 0, 0);
    n0.updatePriority(pf_end.x, pf_end.x);
    pq[pqi].push(n0);
    int steps = size.y*size.x/10.0;

    while(!pq[pqi].empty()){
        steps--;
        if (steps <= 0){
            return path;
        }
        n0=pq[pqi].top();
        x=n0.x;
        y=n0.y;
        pq[pqi].pop();
        m_open[x][y]=0;
        m_close[x][y]=1;
        if(x == pf_end.x && y == pf_end.y){
            while(!(x==pf_start.x && y==pf_start.y)){
                j=m_dir[x][y];
                int px = (x)*cellSize ;
                int py = (y)*cellSize ;
                path.push(Point( px ,py  ));
                //m_map[x][y] = 3;
                x+=dx[j];
                y+=dy[j];
            }
             return path;
        }
        for(i=0;i<directions;i++){
            xdx=x+dx[i];
            ydy=y+dy[i];
            if(!(xdx<0 || xdx>size.x-1 || ydy<0 || ydy>size.y-1 || m_map[xdx][ydy]==1 || m_close[xdx][ydy]==1)){
                m0=PFNode( xdx, ydy, n0.level,  n0.priority);
                m0.updatePriority(pf_end.x, pf_end.y);
                if(m_open[xdx][ydy]==0){
                    m_open[xdx][ydy]=m0.priority;
                    pq[pqi].push(m0);
                    m_dir[xdx][ydy]=(i+directions/2)%directions;
                }else if(m_open[xdx][ydy]>m0.priority){
                    m_open[xdx][ydy]=m0.priority;
                    m_dir[xdx][ydy]=(i+directions/2)%directions;
                    while(!(pq[pqi].top().x == xdx && pq[pqi].top().y == ydy)){
                        pq[1-pqi].push(pq[pqi].top());
                        pq[pqi].pop();
                    }
                    pq[pqi].pop();
                    if(pq[pqi].size()>pq[1-pqi].size()) pqi=1-pqi;
                    while(!pq[pqi].empty()){
                        pq[1-pqi].push(pq[pqi].top());
                        pq[pqi].pop();
                    }
                    pqi=1-pqi;
                    pq[pqi].push(m0); // add the better node instead
                }
            }
        }
    }
    return path;
}
