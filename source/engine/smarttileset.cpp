#include "smarttileset.hpp"
#include "screenmanager.hpp"
#include "collision.hpp"
#include "camera.hpp"
#include "bear.hpp"
#include "renderhelp.hpp"
#include "timer.hpp"




SmartTileset::SmartTileset(PointInt _tileSize,PointInt _tilesetSize,int layers,PointInt _maxTextureSize):tileSize(PointInt(8,8)){
    Stopwatch timer;
    bear::out << "Begin at "<<float(timer.Get()/1000.0)<<"\n";
    isValid = false;
    lastTarget = nullptr;
    PointInt engineMaxSize = ScreenManager::GetInstance().GetMaxTextureSize();
    if (_maxTextureSize.x == -1 || _maxTextureSize.x > engineMaxSize.x){
        _maxTextureSize.x = engineMaxSize.x;
    }
    if (_maxTextureSize.y == -1 || _maxTextureSize.y > engineMaxSize.y){
        _maxTextureSize.y = engineMaxSize.y;
    }
    tileSize = _tileSize;
    PointInt truncator = _maxTextureSize/tileSize;

    maxTextureSize = truncator*tileSize;

    this->tilesetCompatSize = _tilesetSize;
    tilesetSize = _tilesetSize;
    bear::out << "Max texture size set to " << maxTextureSize.x << " x " << maxTextureSize.y << "\n";
    PointInt tilesPerBlock = maxTextureSize/tileSize;
    tilesetSize = tilesetSize * tileSize;
    bear::out << "Each texture has "<<tilesPerBlock.x<<","<<tilesPerBlock.y<<" tiles \n";
    bear::out << "Map size is " << tilesetSize.x<<','<<tilesetSize.y <<" \n";
    if (tilesetSize.x == 0 || tilesetSize.y == 0){
        return;
    }
    framesOnMap = tilesetSize/maxTextureSize;
    PointInt leftPixels = PointInt(tilesetSize.x%maxTextureSize.x,tilesetSize.y%maxTextureSize.y);

    if (leftPixels.x > 0){
        framesOnMap.x++;
    }
    if (leftPixels.y > 0){
        framesOnMap.y++;
    }

    bear::out << "The texture matrix size is " << framesOnMap  << " and each texture has the size of "<<maxTextureSize<<"\n";
    this->maxTextureSize = maxTextureSize;
    this->tileSize = tileSize;
    this->tilesetSize = tilesetSize;
    this->Layers = layers;

    bool isOkay = true;
    textureMap  = new TargetTexture***[layers];
    needRemake  = new bool**[layers];
    m_isUsed  = new bool**[layers];
    tileMap     = new TileInfo**[layers];
    for (int l=0;l<layers;l++){
        textureMap[l]   = new TargetTexture**[framesOnMap.y];
        for (int y=0;y<framesOnMap.y;y++){
            textureMap[l][y] = new TargetTexture*[framesOnMap.x];
            for (int x=0;x<framesOnMap.x;x++){
                textureMap[l][y][x] = new TargetTexture();
                textureMap[l][y][x]->Generate(maxTextureSize.x, maxTextureSize.y);
            }
        }
    }
    for (int l=0;l<layers;l++){
        needRemake[l]   = new bool*[framesOnMap.y];
        m_isUsed[l]   = new bool*[framesOnMap.y];
        for (int y=0;y<framesOnMap.y;y++){
            needRemake[l][y] = new bool[framesOnMap.x];
            m_isUsed[l][y] = new bool[framesOnMap.x];
            for (int x=0;x<framesOnMap.x;x++){
                needRemake[l][y][x] = false;
                m_isUsed[l][y][x] = false;
            }
        }
        tileMap[l] = new TileInfo*[tilesetCompatSize.y];
        for (int y=0;y<tilesetCompatSize.y;y++){
            tileMap[l][y]    = new TileInfo[tilesetCompatSize.x];
            for (int x=0;x<tilesetCompatSize.x;x++){
                tileMap[l][y][x].id = 0;
            }
        }
    }
    bear::out << "Textures were made.\n";
    if (isOkay)
        isValid = true;
    bear::out << "Finished at "<<float(timer.Get()/1000.0)<<"\n";
}

TargetTexture*& SmartTileset::GetTextureFromPosition(int layer,int x,int y){
    PointInt tilePosition = PointInt(x/maxTextureSize.x,y/maxTextureSize.y);
    static TargetTexture* nullRet;
    nullRet = nullptr;
    if (tilePosition.x >= framesOnMap.x) {
        bear::out << "[1]Invalid texture on position " << tilePosition << " versus "<<framesOnMap<<"\n";
        return nullRet;
    }
    if (tilePosition.y >= framesOnMap.y) {
        bear::out << "[2]Invalid texture on position " << tilePosition << " versus "<<framesOnMap<<"\n";
        return nullRet;
    }
    if (layer >= Layers) {
        bear::out << "[3]Invalid texture on layer " << layer << " versus "<<Layers<<"\n";
        return nullRet;
    }
    return textureMap[layer][tilePosition.y][tilePosition.x];
}
void SmartTileset::SetSprite(Sprite spr){
    if (spr.IsLoaded()){
        sp = spr;
        sheetSizes.x = sp.GetWidth()/tileSize.x;
        sheetSizes.y = sp.GetHeight()/tileSize.y;
    }else{
        bear::out << "[SmartTileset]Sprite from the tile was not loaded.\n";
    }
}
void SmartTileset::RenderTile(int x,int y,TileInfo &t){
    if (sheetSizes.x == 0 || sheetSizes.y == 0 || t.id == 0){
        return;
    }
    int cx = t.id%sheetSizes.x;
    int cy = t.id/sheetSizes.x;
    Shader::Unbind();
    sp.SetClip(tileSize.x*cx,tileSize.y*cy,tileSize.x,tileSize.y);
    sp.SetFlip(t.flip);
    sp.Render(x,y,t.rotation);

    Shader::ReBind();
}

bool SmartTileset::MakeMap(){
    if (!isValid)
        return false;
    Stopwatch timer;
    bear::out << "Making the map "<<float(timer.Get()/1000.0)<<"\n";

    for (int l=0;l<Layers;l++){
        for (int y=0;y<framesOnMap.y;y++){
            for (int x=0;x<framesOnMap.x;x++){
                //if (m_isUsed[l][y][x]){

                    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_DST_ALPHA);
                    RenderHelp::DrawSquareColor(Rect(0,0,maxTextureSize.x,maxTextureSize.y),0,0,0,0);
                    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                    TargetTexture::UnBind();



                needRemake[l][y][x] = false;

            }
        }
    }
    bear::out << "Textures " << float(timer.Get()/1000.0) << "\n";
    PointInt tilePosition;
    PointInt tileOffset;
    for (int l=0;l<Layers;l++){
        for (int y=0;y<tilesetCompatSize.y;y++){
            for (int x=0;x<tilesetCompatSize.x;x++){
                if (tileMap[l][y][x].id > 0){
                    tilePosition = PointInt(x,y);
                    tilePosition = tilePosition * tileSize;
                    tileOffset = PointInt(tilePosition.x%maxTextureSize.x,tilePosition.y%maxTextureSize.y);
                    //if (m_isUsed[l][tilePosition.y][tilePosition.x]){
                        Shader::Unbind();
                        TargetTexture*& thisT = GetTextureFromPosition(l,tilePosition.x,tilePosition.y);
                        if (thisT){
                            thisT->Bind();
                            RenderTile(tileOffset.x,tileOffset.y,tileMap[l][y][x]);
                            thisT->UnBind();
                            Shader::ReBind();
                        }
                    //}
                }
            }
        }
    }
    bear::out << "Finished in " << float(timer.Get()/1000.0) << "\n";

    return true;
}

void SmartTileset::SetTile(int l,int x,int y,int tile,double angle,SDL_RendererFlip fliping){
    if (!isValid)
        return;
    if (l >= Layers){
        return;
    }
    if (x >= tilesetCompatSize.x){
        return;
    }
    if (y >= tilesetCompatSize.y){
        return;
    }
    PointInt tilesPerBlock = maxTextureSize/tileSize;
    PointInt tilePosition = PointInt(x/tilesPerBlock.x,y/tilesPerBlock.y);
    /*if (tile > 0){
        if (!m_isUsed[l][tilePosition.y][tilePosition.x]){
            if (!textureMap[l][tilePosition.y][tilePosition.x]){
                bear::out << "Making texture on " << l << ":"<< tilePosition.x << ":" << tilePosition.y << "\n";
                textureMap[l][tilePosition.y][tilePosition.x] = new TargetTexture();
                textureMap[l][tilePosition.y][tilePosition.x]->Generate(maxTextureSize.x, maxTextureSize.y);
            }
        }
        m_isUsed[l][tilePosition.y][tilePosition.x] = true;
    }*/

    tileMap[l][y][x].id = tile;
    tileMap[l][y][x].rotation = angle;
    tileMap[l][y][x].flip = fliping;


    if (tilePosition.x < framesOnMap.x && tilePosition.y < framesOnMap.y) {
        needRemake[l][tilePosition.y][tilePosition.x] = true;
    }
}
void SmartTileset::ResetFocus(){
    if (!isValid)
        return;

    lastTarget = nullptr;
}
void SmartTileset::SetTileDirect(int l,int x,int y,int tile, double angle,SDL_RendererFlip fliping){
    if (!isValid)
        return;
    if (l >= Layers){
        return;
    }
    if (x >= tilesetCompatSize.x){
        return;
    }
    if (y >= tilesetCompatSize.y){
        return;
    }
    tileMap[l][y][x].id = tile;
    tileMap[l][y][x].rotation = angle;
    tileMap[l][y][x].flip = fliping;

    PointInt tilesPerBlock = maxTextureSize/tileSize;
    PointInt tilePosition = PointInt(x/tilesPerBlock.x,y/tilesPerBlock.y);
    if (tilePosition.x < framesOnMap.x && tilePosition.y < framesOnMap.y) {
        PointInt tileOffset = PointInt((x * tileSize.x)%maxTextureSize.x,(y * tileSize.y)%maxTextureSize.y);

        textureMap[l][tilePosition.y][tilePosition.x]->Bind();
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_DST_ALPHA);
        RenderHelp::DrawSquareColor(Rect(tileOffset.x,tileOffset.y,tileSize.x,tileSize.y),0,0,0,0);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        RenderTile(  tileOffset.x, tileOffset.y, tileMap[l][y][x]);
        TargetTexture::UnBind();
    }
}

void SmartTileset::Update(float dt){
    if (!isValid)
        return;
    for (int l=0;l<Layers;l++){
        for (int y=0;y<framesOnMap.y;y++){
            for (int x=0;x<framesOnMap.x;x++){
                if (needRemake[l][y][x]){
                    needRemake[l][y][x] = false;
                    PointInt tilesPerBlock = maxTextureSize/tileSize;
                    textureMap[l][y][x]->Bind();
                    for (int tx = 0; tx < tilesPerBlock.x; tx++){
                        for (int ty = 0; ty < tilesPerBlock.y; ty++){
                            int addy = std::min((y * tilesPerBlock.y) + ty,tilesetCompatSize.y-1);
                            int addx = std::min((x * tilesPerBlock.x) + tx,tilesetCompatSize.x-1);
                            RenderTile(  tx*tileSize.x , ty*tileSize.x ,tileMap[l][ addy ][ addx ] );

                        }
                    }
                    TargetTexture::UnBind();
                }
            }
        }
    }
}


void SmartTileset::RenderLayer(int layer,bool showBoundary){
    if (!isValid)
        return;
    RectInt canvasArea(0,0,maxTextureSize.x,maxTextureSize.y);
    for (int y=0;y<framesOnMap.y;y++){
        for (int x=0;x<framesOnMap.x;x++){
            canvasArea.x = (x * canvasArea.w) ;
            canvasArea.y = (y * canvasArea.h) ;
            if (Collision::IsColliding(Camera::pos,canvasArea)){

                if (textureMap[layer][y][x])
                    textureMap[layer][y][x]->Render(Point(canvasArea.x - Camera::pos.x,canvasArea.y - Camera::pos.y));

                if (showBoundary)
                    RenderHelp::DrawSquareColor(Rect(canvasArea.x - Camera::pos.x,canvasArea.y - Camera::pos.y,canvasArea.w,canvasArea.h),0,0,255,255,true);
            }
        }
    }
}

SmartTileset::~SmartTileset(){
    if (!isValid)
        return;
    for (int l=0;l<Layers;l++){
        for (int y=0;y<framesOnMap.y;y++){
            for (int x=0;x<framesOnMap.x;x++){
                if (textureMap[l][y][x])
                    textureMap[l][y][x]->FreeTexture();
                delete (textureMap[l][y][x]);
            }
            delete []textureMap[l][y];
            delete []needRemake[l][y];
            delete []m_isUsed[l][y];
        }
        delete []textureMap[l];
        delete []needRemake[l];
        delete []m_isUsed[l];
    }
    delete []needRemake;
    delete []m_isUsed;
    delete []textureMap;
    for (int l=0;l<Layers;l++){
        for (int y=0;y<tilesetCompatSize.y;y++){
            delete []tileMap[l][y];
        }
        delete []tileMap[l];
    }
    delete []tileMap;
}
