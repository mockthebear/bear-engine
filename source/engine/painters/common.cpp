#include "painters.hpp"
#include "../renderhelp.hpp"
#include "../bear.hpp"
#include "../sprite.hpp"

BearTexture *Painter::UnloadedTexture = nullptr;
TexturePtr Painter::UnloadedTexturePtr;

void Painter::SetEmptyTexture(){
    BearTexture *st;


    TextureLoadMethod filter;
    filter.mode = TEXTURE_NEAREST;
    uint32_t data[32*32];
    for (int i=0;i< 32*32; i++){
        uint8_t ran = rand()%255;
        data[i] = RenderHelp::FormatRGBA(ran,ran,ran,255);
    }
    st = MakeTexture(Point(32,32),GL_RGBA,(unsigned char*)data,filter);    //UnloadedTextureId
    UnloadedTextureId = st->GLimageId;
    UnloadedTexture = st;
    UnloadedTexturePtr = chain_ptr<BearTexture>::make(UnloadedTexture);
}


