#include "basetypes.hpp"
#include "painters/painters.hpp"
#include "../framework/geometry.hpp"


const BearColor BCWhite4[4] = {
    BearColor(),
    BearColor(),
    BearColor(),
    BearColor(),
};

const BearColor BCWhite2[2] = {
    BearColor(),
    BearColor(),
};

void BearTexture::ApplyPixels(uint8_t *pixels){
    if (id != 0){
        Painter::SetTexturePixels(id, PointInt(size_w,size_h), mode, pixels);
    }
};

void BearTexture::ClearTexture(){
    uint32_t tex = DropTexture();
    if (tex > 0)
        glDeleteTextures(1, &tex);
}

uint32_t BearTexture::DropTexture(){
    uint32_t ret = id;
    id = 0;
    return ret;
}
