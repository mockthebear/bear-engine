#include "targettexturetilemap.hpp"
#include "../screenmanager.hpp"
#include "../timer.hpp"
#include "../renderhelp.hpp"


TargetTextureTileMap::TargetTextureTileMap(PointInt _tileSize, PointInt3 _tilesetSize, PointInt _maxTextureSize,bool silent):TileMap(_tileSize, _tilesetSize, Sprite("data/tiles.png")){
    Stopwatch timer;
    m_isValid = false;
    m_silence = silent;


    PointInt engineMaxSize = ScreenManager::GetInstance().GetMaxTextureSize();
    if (_maxTextureSize.x == -1 || _maxTextureSize.x > engineMaxSize.x){
        _maxTextureSize.x = engineMaxSize.x;
    }
    if (_maxTextureSize.y == -1 || _maxTextureSize.y > engineMaxSize.y){
        _maxTextureSize.y = engineMaxSize.y;
    }
    PointInt truncator = _maxTextureSize/_tileSize;

    m_maxTextureSize = truncator*_tileSize;

    m_tilesetCompatSize = Point(m_size.x, m_size.y) * _tileSize;
    PointInt tilesPerBlock = m_maxTextureSize/_tileSize;

    if (m_silence){
        bear::out << "Max texture size set to " << m_maxTextureSize.x << " x " << m_maxTextureSize.y << "\n";
        bear::out << "Each texture has "<<tilesPerBlock.x<<","<<tilesPerBlock.y<<" tiles \n";
        bear::out << "Map size is " << m_size.x<<","<<m_size.y <<" and "<<m_size.z<<" layers\n";
        bear::out << "Compat size is "<<m_tilesetCompatSize<<"\n";
    }



    if (m_size.x == 0 || m_size.y == 0 || m_size.z == 0){
        return;
    }
    m_framesOnMap = PointInt(m_tilesetCompatSize.x, m_tilesetCompatSize.y)/m_maxTextureSize;
    PointInt leftPixels = PointInt(m_tilesetCompatSize.x%m_maxTextureSize.x,m_tilesetCompatSize.y%m_maxTextureSize.y);

    if (leftPixels.x > 0){
        m_framesOnMap.x++;
    }
    if (leftPixels.y > 0){
        m_framesOnMap.y++;
    }
    if (m_silence)
        bear::out << "The texture matrix size is " << m_framesOnMap  << " and each texture has the size of "<<m_maxTextureSize<<"\n";

    bool isOkay = true;


    int vecSize_small = m_size.z * m_framesOnMap.x * m_framesOnMap.y;

    TargetTexture *a_textureMap = new TargetTexture[vecSize_small];
    bool *a_needRemake = new bool[vecSize_small];
    bool *a_isUsed = new bool[vecSize_small];
    for (int i=0;i < vecSize_small; i++){
        if (m_silence)
            bear::out << "Generating "<<i<<"/"<<vecSize_small<<"\n";
        a_textureMap[i].Generate(m_maxTextureSize.x, m_maxTextureSize.y);
    }


    m_textureMap = std::shared_ptr<TargetTexture>(a_textureMap, [](TargetTexture *p) {
           delete [] p;
    });

    m_needRemake = std::shared_ptr<bool>(a_needRemake, [](bool *p) {
           delete [] p;
    });

    m_isUsed = std::shared_ptr<bool>(a_isUsed, [](bool *p) {
           delete [] p;
    });

    if (m_silence)
        bear::out << "Textures were made.\n";
    if (isOkay)
        m_isValid = true;
    if (m_silence)
        bear::out << "Finished at "<<float(timer.Get()/1000.0)<<"\n";
}

TargetTexture& TargetTextureTileMap::GetTextureFromPosition(int layer,int x,int y){
    PointInt3 tilePosition = PointInt3(x/m_maxTextureSize.x,y/m_maxTextureSize.y, layer);
    static TargetTexture nullRet;
    if (tilePosition.x >= m_framesOnMap.x) {
        if (m_silence)
            bear::out << "[1]Invalid texture on position " << tilePosition << " versus "<<m_framesOnMap<<"\n";
        return nullRet;
    }
    if (tilePosition.y >= m_framesOnMap.y) {
        if (m_silence)
            bear::out << "[2]Invalid texture on position " << tilePosition << " versus "<<m_framesOnMap<<"\n";
        return nullRet;
    }
    if (tilePosition.z >= m_size.z) {
        if (m_silence)
            bear::out << "[3]Invalid texture on layer " << tilePosition.z << " versus "<<m_size.z<<"\n";
        return nullRet;
    }
    uint32_t offs = CalculateSmallOffset(tilePosition);
    return m_textureMap.get()[offs];
}

uint32_t TargetTextureTileMap::CalculateSmallOffset(PointInt3 pos){
    return (pos.x + (pos.y * m_framesOnMap.x) + (pos.z * m_framesOnMap.x * m_framesOnMap.y));
}

bool TargetTextureTileMap::MakeMap(){
    if (!m_isValid)
        return false;
    Stopwatch timer;
    if (m_silence)
        bear::out << "Making the map "<<float(timer.Get()/1000.0)<<"\n";

    /*
        Clear textures
    */
    for (int l=0;l<m_size.z;l++){
        for (int y=0;y<m_framesOnMap.y;y++){
            for (int x=0;x<m_framesOnMap.x;x++){
                TargetTexture& thisT = GetTextureFromPosition(l, x,y);
                thisT.Bind();
                thisT.Clear();
                TargetTexture::UnBind();
                m_needRemake.get()[CalculateSmallOffset(PointInt3(x, y, l))] = false;
            }
        }
    }
    if (m_silence)
        bear::out << "Textures " << float(timer.Get()/1000.0) << " : "<< m_size << "\n";
    PointInt tilePosition;
    PointInt tileOffset;
    for (int l=0;l<m_size.z;l++){
        for (int y=0;y<m_size.y;y++){
            for (int x=0;x<m_size.x;x++){
                uint32_t memOffset = CalculateSmallOffset(PointInt3(x,y,l));
                Tile & localTile = m_map.get()[memOffset];
                if (localTile.id > 0){
                    tilePosition = PointInt(x,y);
                    tilePosition = tilePosition * m_tileSize;
                    tileOffset = PointInt(tilePosition.x%m_maxTextureSize.x,tilePosition.y%m_maxTextureSize.y);
                    //Shader::Unbind();
                    TargetTexture& thisT = GetTextureFromPosition(l,tilePosition.x,tilePosition.y);
                    if (thisT.IsValid()){
                        thisT.Bind();
                        RenderTile(localTile.id, tileOffset, localTile.rotate, localTile.flip);
                        thisT.UnBind();
                        //Shader::ReBind();
                    }
                }
            }
        }
    }
    if (m_silence)
        bear::out << "Finished in " << float(timer.Get()/1000.0) << "\n";
    return true;
}

void TargetTextureTileMap::SetTile(PointInt3 pos, Tile t){
    if (!m_isValid)
        return;
    if (pos.z >= m_size.z){
        return;
    }
    if (pos.x >= m_size.x){
        return;
    }
    if (pos.y >= m_size.y){
        return;
    }
    PointInt tilesPerBlock = m_maxTextureSize/m_tileSize;
    PointInt3 tilePosition = PointInt3(pos.x/tilesPerBlock.x,pos.y/tilesPerBlock.y, pos.z);

    uint32_t memOffset = CalculateOffset(pos);
    uint32_t memOffset2 = CalculateSmallOffset(tilePosition);

    if (pos.x >= m_size.x || pos.y >= m_size.y || pos.z >= m_size.z){
        if (m_silence)
            bear::out << "Trying to set tile when there is no how."<<pos<<"\n";
    }
    m_map.get()[memOffset] = t;


    if (tilePosition.x < m_framesOnMap.x && tilePosition.y < m_framesOnMap.y) {
        m_needRemake.get()[memOffset2] = true;
    }
}


void TargetTextureTileMap::SetTileDirect(PointInt3 pos, Tile t){
    if (!m_isValid)
        return;
    if (pos.z >= m_size.z){
        return;
    }
    if (pos.x >= m_size.x){
        return;
    }
    if (pos.y >= m_size.y){
        return;
    }

    uint32_t memOffset = CalculateOffset(pos);



    m_map.get()[memOffset] = t;

    PointInt tilesPerBlock = m_maxTextureSize/m_tileSize;
    PointInt3 tilePosition = PointInt3(pos.x/tilesPerBlock.x,pos.y/tilesPerBlock.y, pos.z);

    uint32_t memOffset2 = CalculateSmallOffset(tilePosition);


    if (tilePosition.x < m_framesOnMap.x && tilePosition.y < m_framesOnMap.y) {
        PointInt tileOffset = PointInt( int(pos.x * m_tileSize.x)%m_maxTextureSize.x, int(pos.y * m_tileSize.y)%m_maxTextureSize.y);
        m_textureMap.get()[memOffset2].Bind();
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_DST_ALPHA);
        RenderHelp::DrawSquareColor(RectColor(tileOffset.x, tileOffset.y, m_tileSize.x, m_tileSize.y, 0, 0, 0, 0));
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        RenderTile(t.id,  tileOffset, t.rotate, t.flip );
        TargetTexture::UnBind();
    }
}


void TargetTextureTileMap::Render(Point offset, Rect vision){
    for (int l=0;l<m_size.z;l++){
        RenderLayer(l, offset, vision, false);
    }
}


void TargetTextureTileMap::Update(float dt){
    if (!m_isValid)
        return;
    for (int l=0;l<m_size.z;l++){
        for (int y=0;y<m_framesOnMap.y;y++){
            for (int x=0;x<m_framesOnMap.x;x++){
                uint32_t memOffset = CalculateSmallOffset(PointInt3(x, y, l));
                if ( m_needRemake.get()[memOffset]){
                    m_needRemake.get()[memOffset] = false;
                    PointInt tilesPerBlock = m_maxTextureSize / m_tileSize;
                    m_textureMap.get()[memOffset].Bind();
                    for (int tx = 0; tx < tilesPerBlock.x; tx++){
                        for (int ty = 0; ty < tilesPerBlock.y; ty++){
                            int addy = std::min((y * tilesPerBlock.y) + ty,m_tilesetCompatSize.y-1);
                            int addx = std::min((x * tilesPerBlock.x) + tx,m_tilesetCompatSize.x-1);
                            uint32_t memOffset2 = CalculateOffset(PointInt3(addx, addy, l));
                            Tile &t = m_map.get()[memOffset2];
                            RenderTile(t.id,  Point(tx*m_tileSize.x , ty*m_tileSize.x), t.rotate, t.flip);

                        }
                    }
                    TargetTexture::UnBind();
                }
            }
        }
    }
}

void TargetTextureTileMap::RenderLayer(int layer,Point offset, Rect vision, bool showBoundary){
    if (!m_isValid)
        return;
    RectInt canvasArea(0,0,m_maxTextureSize.x,m_maxTextureSize.y);
    for (int y=0;y<m_framesOnMap.y;y++){
        for (int x=0;x<m_framesOnMap.x;x++){
            canvasArea.x = (x * canvasArea.w) ;
            canvasArea.y = (y * canvasArea.h) ;
            if (Collision::IsColliding(vision, canvasArea)){
                int memOffset2 = CalculateSmallOffset(PointInt3(x, y, layer));
                if (memOffset2 < m_size.z * m_size.x * m_size.y){
                    m_textureMap.get()[memOffset2].Render(Point(canvasArea.x - vision.x + offset.x,canvasArea.y - vision.y + offset.y));
                    RenderHelp::DrawSquareColor(RectColor(canvasArea.x - vision.x + offset.x,canvasArea.y - vision.y + offset.y,canvasArea.w,canvasArea.h,0,0,255,255),true);
                }
            }
        }
    }
}


