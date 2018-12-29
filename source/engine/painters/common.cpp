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
    for (uint32_t i=0;i< 32*32; i++){
        uint8_t ran = rand()%255;
        uint8_t ranR = ran;
        uint32_t x = i % 32;
        uint32_t y = i / 32;
        if (x == y ||  (32-x) == y || x == 0 || y == 0 || x == 31 || y == 31){
            ranR = 255;
            ran = 0;
        }
        data[i] = RenderHelp::FormatRGBA(ranR,ran,ran,255);
    }
    st = MakeTexture(Point(32,32),GL_RGBA,(unsigned char*)data,filter);    //UnloadedTextureId
    UnloadedTextureId = st->GLimageId;
    UnloadedTexture = st;
    UnloadedTexturePtr = chain_ptr<BearTexture>::make(UnloadedTexture);
}


