#include "smarttexture.hpp"
#include "gamebase.hpp"

SmartTexture::SmartTexture(int xx,int yy,int ww,int hh,bool generatePixels){
   h = hh;
   w = ww;
   pixels = nullptr;
   t = SDL_CreateTexture( BearEngine->GetRenderer(),SDL_PIXELFORMAT_ARGB8888,  SDL_TEXTUREACCESS_STATIC , ww, hh);

   pixels = new Uint32[w * h];
   SDL_SetTextureBlendMode( t, SDL_BLENDMODE_BLEND );
   x = xx;
   y = yy;
}



void SmartTexture::UpdateTexture(){
   SDL_UpdateTexture(t, NULL, pixels, w * sizeof(Uint32));
}
SmartTexture::~SmartTexture(){
    delete []pixels;

}
void SmartTexture::Render(PointInt pos,float angle){
    double scaleRatioW = floor(ScreenManager::GetInstance().GetScaleRatioW()*(w+0.0))/(w+0.0);
    double scaleRatioH = floor(ScreenManager::GetInstance().GetScaleRatioH()*(h+0.0))/(h+0.0);
    SDL_Rect rr;
    rr.x = round((double)(x+pos.x)*scaleRatioW);
    rr.y = round((double)(y+pos.y)*scaleRatioH);
    rr.h = round((double)(h)*scaleRatioH);
    rr.w = round((double)(w)*scaleRatioW);
    SDL_RenderCopyEx(BearEngine->GetRenderer(),t,NULL,&rr,angle,nullptr,SDL_FLIP_NONE);
}

SmartSurface::SmartSurface()
{
	texture = NULL;
	w = 0;
	h = 0;
	mPixels = NULL;
	pitch = 0;
}

SmartSurface::SmartSurface(std::string path)
{
	texture = NULL;
	w = 0;
	h = 0;
	mPixels = NULL;
	pitch = 0;
	loadFromFile(  path );
}
SmartSurface::~SmartSurface()
{
	free();
}

bool SmartSurface::loadFromFile( std::string path )
{
	free();
	SDL_Texture* texture_aux = NULL;
	SDL_Surface* surf = IMG_Load( path.c_str() );
	if( surf == NULL ){
		printf( "Cannot load img: %s\n", path.c_str() );
		return false;
	}else{
		SDL_Surface* aux_surf = SDL_ConvertSurfaceFormat( surf, SDL_PIXELFORMAT_ARGB8888, 0 );
		if( aux_surf == NULL ){
			printf( "Unable to convert loaded surface to display format! %s\n", SDL_GetError() );
		}else{
			texture_aux = SDL_CreateTexture( Game::GetInstance()->GetRenderer(), SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, aux_surf->w, aux_surf->h );
            if (not texture_aux){
                return false;
            }
            SDL_SetTextureBlendMode( texture_aux, SDL_BLENDMODE_BLEND );
            SDL_LockTexture( texture_aux, &aux_surf->clip_rect, &mPixels, &pitch );
            memcpy( mPixels, aux_surf->pixels, aux_surf->pitch * aux_surf->h );
            w = aux_surf->w;
            h = aux_surf->h;
            Uint32* pixels = (Uint32*)mPixels;
            int count = ( pitch / 4 ) * h;
            Uint32 colorKey = SDL_MapRGB( aux_surf->format, 0xFF, 0xFF, 0xFF );
            Uint32 transparent = SDL_MapRGBA( aux_surf->format, 0x00, 0x0, 0x0, 0x00 );
            for( int i = 0; i < count; ++i ){
                if( pixels[ i ] == colorKey ){
                    pixels[ i ] = transparent;
                }
            }

            SDL_UnlockTexture( texture_aux );
            mPixels = NULL;
        }
        SDL_FreeSurface( aux_surf );
		SDL_FreeSurface( surf );
	}

	//Return success
	texture = texture_aux;
	return true;
}

void SmartSurface::free(){
	//Free texture if it exists
	if( texture != NULL )
	{
		SDL_DestroyTexture( texture );
		texture = NULL;
		w = 0;
		h = 0;
		mPixels = NULL;
		pitch = 0;
	}
}

void SmartSurface::setColor( Uint8 red, Uint8 green, Uint8 blue ){
	SDL_SetTextureColorMod( texture, red, green, blue );
}

void SmartSurface::setBlendMode( SDL_BlendMode blending ){
	SDL_SetTextureBlendMode( texture, blending );
}

void SmartSurface::setAlpha( Uint8 alpha ){
	SDL_SetTextureAlphaMod( texture, alpha );
}

void SmartSurface::Render( int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip ){
	SDL_Rect dimensions2;
    double scaleRatioW = ScreenManager::GetInstance().GetScaleRatioW();
    double scaleRatioH = ScreenManager::GetInstance().GetScaleRatioH();
    dimensions2.h = h*scaleRatioH;
    dimensions2.w = w*scaleRatioW;
	if( clip != NULL )
	{
		dimensions2.w = clip->w*scaleRatioW;
		dimensions2.h = clip->h*scaleRatioH;
	}

    dimensions2.x = x*scaleRatioW+ ScreenManager::GetInstance().GetOffsetW();
    dimensions2.y = y*scaleRatioH+ ScreenManager::GetInstance().GetOffsetH();

	SDL_RenderCopyEx( Game::GetInstance()->GetRenderer(), texture, clip, &dimensions2, angle, center, flip );

}


bool SmartSurface::lockTexture(){
	if( mPixels != NULL ){
		return false;
	}else if( SDL_LockTexture( texture, NULL, &mPixels, &pitch ) != 0 ){
        return false;
	}
	return true;
}

bool SmartSurface::unlockTexture(){
	if( mPixels == NULL ){
		return false;
	}else{
		SDL_UnlockTexture( texture );
		mPixels = NULL;
		pitch = 0;
		return true;
	}
}


Uint32 SmartSurface::getPixel32( unsigned int x, unsigned int y ){
    Uint32 *pixels = (Uint32*)mPixels;
    return pixels[ ( y * ( pitch / 4 ) ) + x ];
}
