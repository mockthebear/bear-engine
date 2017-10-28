#include "smarttileset.hpp"
#include "screenmanager.hpp"
#include "collision.hpp"
#include "camera.hpp"
#include "bear.hpp"
#include "renderhelp.hpp"
#include "timer.hpp"




SmartTileset::SmartTileset(PointInt tileSize,PointInt tilesetSize,int layers,PointInt maxTextureSize):tileSize(PointInt(8,8)){
    isValid = false;
    lastTarget = nullptr;
    PointInt engineMaxSize = ScreenManager::GetInstance().GetMaxTextureSize();
    if (maxTextureSize.x == -1 || maxTextureSize.x > engineMaxSize.x){
        maxTextureSize.x = engineMaxSize.x;
    }
    if (maxTextureSize.y == -1 || maxTextureSize.y > engineMaxSize.y){
        maxTextureSize.y = engineMaxSize.y;
    }

    PointInt truncator = maxTextureSize/tileSize;

    maxTextureSize = truncator*tileSize;

    this->tilesetCompatSize = tilesetSize;
    std::cout << "Max texture size set to " << maxTextureSize.x << " x " << maxTextureSize.y << "\n";
    PointInt tilesPerBlock = maxTextureSize/tileSize;
    tilesetSize = tilesetSize * tileSize;
    std::cout << "Each texture has "<<tilesPerBlock.x<<","<<tilesPerBlock.y<<" tiles \n";
    std::cout << "Map size is " << tilesetSize.x<<','<<tilesetSize.y <<" \n";
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
    tileMap     = new int**[layers];
    for (int l=0;l<layers;l++){
        textureMap[l]   = new TargetTexture**[framesOnMap.y];
        needRemake[l]   = new bool*[framesOnMap.y];
        for (int y=0;y<framesOnMap.y;y++){
            needRemake[l][y] = new bool[framesOnMap.x];
            textureMap[l][y] = new TargetTexture*[framesOnMap.x];
            for (int x=0;x<framesOnMap.x;x++){
                needRemake[l][y][x] = false;
                if (!isOkay){
                    textureMap[l][y][x] = nullptr;
                }else{
                    textureMap[l][y][x] = new TargetTexture();//SDL_CreateTexture( g_game.GetRenderer(), SDL_PIXELFORMAT_RGBA8888, fTUREACCESS_TARGET, maxTextureSize.x, maxTextureSize.y );
                    textureMap[l][y][x]->Generate(maxTextureSize.x, maxTextureSize.y);//SDL_CreateTexture( g_game.GetRenderer(), SDL_PIXELFORMAT_RGBA8888, fTUREACCESS_TARGET, maxTextureSize.x, maxTextureSize.y );
                    if (!textureMap[l][y][x]){
                        bear::out << "Failed to create the texture "<<l<<"x"<<x<<"x"<<y<<" because "<<SDL_GetError()<<"\n";
                        isOkay = false;
                    }else{
                        //SDL_SetTextureBlendMode(textureMap[l][y][x], SDL_BLENDMODE_BLEND);
                        //SDL_SetTextureAlphaMod(textureMap[l][y][x], 255);
                    }
                }
            }
        }
        tileMap[l]      = new int*[tilesetCompatSize.y];
        for (int y=0;y<tilesetCompatSize.y;y++){
            tileMap[l][y]    = new int[tilesetCompatSize.x];
            for (int x=0;x<tilesetCompatSize.x;x++){
                tileMap[l][y][x] = 1;
            }
        }
    }
    bear::out << "Textures were made.\n";
    if (isOkay)
        isValid = true;
}

TargetTexture *SmartTileset::GetTextureFromPosition(int layer,int x,int y){
    PointInt tilePosition = PointInt(x/maxTextureSize.x,y/maxTextureSize.y);
    if (tilePosition.x >= framesOnMap.x) {
        bear::out << "[1]Invalid texture on position " << tilePosition << " versus "<<framesOnMap<<"\n";
        return nullptr;
    }
    if (tilePosition.y >= framesOnMap.y) {
        bear::out << "[2]Invalid texture on position " << tilePosition << " versus "<<framesOnMap<<"\n";
        return nullptr;
    }
    if (layer >= Layers) {
        bear::out << "[3]Invalid texture on layer " << layer << " versus "<<Layers<<"\n";
        return nullptr;
    }
    return textureMap[layer][tilePosition.y][tilePosition.x];
}
void SmartTileset::SetSprite(Sprite spr){
    sp = spr;
    sheetSizes.x = sp.GetWidth()/tileSize.x;
    sheetSizes.y = sp.GetHeight()/tileSize.y;
    if (!sp.IsLoaded()){
        bear::out << "[SmartTileset]Sprite from the tile was not loaded.\n";
    }
}
void SmartTileset::RenderTile(int x,int y,int index){
    if (sheetSizes.x == 0 || sheetSizes.y == 0){
        return;
    }
    int cx = index%sheetSizes.x;
    int cy = index/sheetSizes.x;
    sp.SetClip(tileSize.x*cx,tileSize.y*cy,tileSize.x,tileSize.y);
    sp.Render(x,y,0);
}

bool SmartTileset::MakeMap(){
    if (!isValid)
        return false;
    Stopwatch timer;
    bear::out << "Making the map\n";
    TargetTexture *lastTexture = nullptr;

    //todo: SetRenderTarget  < not working rn

    for (int l=0;l<Layers;l++){
        for (int y=0;y<framesOnMap.y;y++){
            for (int x=0;x<framesOnMap.x;x++){
                /*textureMap[l][y][x];
                SDL_SetRenderDrawBlendMode(BearEngine->GetRenderer(), SDL_BLENDMODE_NONE);
                SDL_SetRenderDrawColor(BearEngine->GetRenderer(), 0, 0, 0, 0);
                SDL_RenderClear(BearEngine->GetRenderer());
                SDL_SetRenderDrawBlendMode(BearEngine->GetRenderer(), SDL_BLENDMODE_BLEND);
                needRemake[l][y][x] = false;*/
            }
        }
    }
    bear::out << "Textures " << float(timer.Get()/1000.0) << "\n";
    for (int l=0;l<Layers;l++){
        for (int y=0;y<tilesetCompatSize.y;y++){
            for (int x=0;x<tilesetCompatSize.x;x++){
                PointInt tilePosition = PointInt(x,y);
                tilePosition = tilePosition * tileSize;
                PointInt tileOffset = PointInt(tilePosition.x%maxTextureSize.x,tilePosition.y%maxTextureSize.y);
                TargetTexture *thisT = GetTextureFromPosition(l,tilePosition.x,tilePosition.y);
                if (thisT != nullptr){
                    if (thisT != lastTexture){
                        TargetTexture::UnBind();
                        lastTexture = thisT;
                        thisT->Bind();
                    }
                }
                RenderTile(tileOffset.x,tileOffset.y,tileMap[l][y][x]);
            }
        }
    }
    bear::out << "Finished in " << float(timer.Get()/1000.0) << "\n";
    TargetTexture::UnBind();

    return true;
}

void SmartTileset::SetTile(int l,int x,int y,int tile){
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
    tileMap[l][y][x] = tile;
    PointInt tilesPerBlock = maxTextureSize/tileSize;
    PointInt tilePosition = PointInt(x/tilesPerBlock.x,y/tilesPerBlock.y);
    if (tilePosition.x < framesOnMap.x && tilePosition.y < framesOnMap.y) {
        needRemake[l][tilePosition.y][tilePosition.x] = true;
    }
}
void SmartTileset::ResetFocus(){
    if (!isValid)
        return;
    TargetTexture::UnBind();
    //ScreenManager::GetInstance().SetRenderTarget(nullptr,true);
    lastTarget = nullptr;
}
void SmartTileset::SetTileDirect(int l,int x,int y,int tile){
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
    tileMap[l][y][x] = tile;
    PointInt tilesPerBlock = maxTextureSize/tileSize;
    PointInt tilePosition = PointInt(x/tilesPerBlock.x,y/tilesPerBlock.y);
    if (tilePosition.x < framesOnMap.x && tilePosition.y < framesOnMap.y) {

        if (lastTarget != textureMap[l][tilePosition.y][tilePosition.x]){
            lastTarget = textureMap[l][tilePosition.y][tilePosition.x];
        //

            //ScreenManager::GetInstance().SetRenderTarget(textureMap[l][tilePosition.y][tilePosition.x]);
        }
        PointInt tileOffset = PointInt((x * tileSize.x)%maxTextureSize.x,(y * tileSize.y)%maxTextureSize.y);
        #ifndef RENDER_OPENGL
        SDL_SetRenderDrawBlendMode(BearEngine->GetRenderer(), SDL_BLENDMODE_NONE);
        SDL_SetRenderDrawColor(BearEngine->GetRenderer(), 255, 255, 255, 0);
        SDL_Rect d;
        d.x = tileOffset.x;
        d.y = tileOffset.y;
        d.w = d.h = tileSize;
        SDL_RenderFillRect(BearEngine->GetRenderer(), &d);
        SDL_SetRenderDrawBlendMode(BearEngine->GetRenderer(), SDL_BLENDMODE_BLEND);
        #else
        textureMap[l][tilePosition.y][tilePosition.x]->Bind();
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_DST_ALPHA);
        RenderHelp::DrawSquareColor(Rect(tileOffset.x,tileOffset.y,tileSize.x,tileSize.y),0,0,0,0);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        RenderTile(  tileOffset.x, tileOffset.y, tile);
        TargetTexture::UnBind();
        #endif // RENDER_OPENGL


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
                            int addy = (y * tilesPerBlock.y) + ty;
                            int addx = (x * tilesPerBlock.x) + tx;
                            RenderTile(  tx * tileSize.x, ty * tileSize.y,tileMap[l][ addy ][ addx ] );
                        }
                    }
                    TargetTexture::UnBind();
                }
            }
        }
    }

    if (g_input.OnScreenResize()){
        MakeMap();
    }

}


void SmartTileset::RenderLayer(int layer){
    if (!isValid)
        return;
    RectInt canvasArea(0,0,maxTextureSize.x,maxTextureSize.y);
    for (int y=0;y<framesOnMap.y;y++){
        for (int x=0;x<framesOnMap.x;x++){
            canvasArea.x = (x * canvasArea.w) ;
            canvasArea.y = (y * canvasArea.h) ;
            if (Collision::IsColliding(Camera::pos,canvasArea)){
                /*SDL_Rect dimensions2;
                double scaleRatioW = ScreenManager::GetInstance().GetScaleRatioW();
                double scaleRatioH = ScreenManager::GetInstance().GetScaleRatioH();

                dimensions2.h = canvasArea.w*scaleRatioH;
                dimensions2.w = canvasArea.h*scaleRatioW;
                dimensions2.x = (canvasArea.x - Camera::pos.x)*scaleRatioW+ ScreenManager::GetInstance().GetOffsetW();
                dimensions2.y = (canvasArea.y - Camera::pos.y)*scaleRatioH+ ScreenManager::GetInstance().GetOffsetH();
                SDL_RenderCopyEx(BearEngine->GetRenderer(), textureMap[layer][y][x], nullptr, &dimensions2, 0, NULL, SDL_FLIP_NONE );
                //RenderHelp::DrawSquareColorA(canvasArea.x - Camera::pos.x,canvasArea.y - Camera::pos.y,canvasArea.w,canvasArea.h,0,0,0,255,true);
                */
                textureMap[layer][y][x]->Render(canvasArea.GetPos());
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
                delete (textureMap[l][y][x]);
            }
            delete []textureMap[l][y];
            delete []needRemake[l][y];
        }
        delete []textureMap[l];
        delete []needRemake[l];
    }
    delete []needRemake;
    delete []textureMap;
    for (int l=0;l<Layers;l++){
        for (int y=0;y<tilesetCompatSize.y;y++){
            delete []tileMap[l][y];
        }
        delete []tileMap[l];
    }
    delete []tileMap;
}
