#ifdef RENDER_OPENGL3
#include "painters.hpp"
#include "../bear.hpp"
#include "../screenmanager.hpp"
#include "../../framework/debughelper.hpp"

bool Painter::RenderTexture(BearTexture *t_texture, RenderData &t_data){
    if (!t_texture){
        return false;
    }



    return true;
}

BearTexture* Painter::MakeTexture(PointInt size,int mode,unsigned char* pixels,TextureLoadMethod &filter){
    if (size.x == 0 || size.y == 0){
        return nullptr;
    }
}


bool Painter::SetupEnvoriment(ScreenManager *sm){


}

int Painter::GetMaxTextureSize(){

    return 1;
}

void Painter::ResetViewPort(PointInt originalSize, PointInt newSize){

}

#endif // RENDER_OPENGL
