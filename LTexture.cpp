#include "LTexture.h"

using namespace std;

LTexture::LTexture(SDL_Renderer* r, string path){
	renderer = r;
	loadFromFile(path);
}

LTexture::LTexture(SDL_Renderer* r, SDL_Texture* t){
	renderer = r;
	mTexture = t;
}

LTexture::~LTexture(){
	free();
}

bool LTexture::loadFromFile(string path){
	free();
	SDL_Texture* newTexture = NULL;
	SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
	SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));
  newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface );
	SDL_FreeSurface( loadedSurface );
	mTexture = newTexture;
	return mTexture != NULL;
}

void LTexture::free(){
	if(mTexture != NULL){
		SDL_DestroyTexture(mTexture);
		mTexture = NULL;
	}
}

void LTexture::setColor(Uint8 red, Uint8 green, Uint8 blue){
	SDL_SetTextureColorMod(mTexture, red, green, blue); //multiplica por estos colores
}

void LTexture::setBlendMode(SDL_BlendMode blending){
    SDL_SetTextureBlendMode(mTexture, blending);
}

void LTexture::setAlpha(Uint8 alpha){
    SDL_SetTextureAlphaMod(mTexture, alpha);
}

void LTexture::render(SDL_Rect* rect, SDL_Rect* src_rect){
	//SDL_RenderCopyEx(renderer, mTexture, NULL, rect, 0, NULL, SDL_FLIP_NONE);
	SDL_RenderCopy(renderer, mTexture, src_rect, rect);
}

void LTexture::renderAngle(SDL_Rect* rect, double angle, int center_x, int center_y){
	SDL_Point center = {center_x, center_y};
	SDL_RenderCopyEx(renderer, mTexture, NULL, rect, angle, &center, SDL_FLIP_NONE);
}

void LTexture::renderInvX(SDL_Rect* rect, SDL_Rect* src_rect){
	SDL_RendererFlip flip = SDL_FLIP_HORIZONTAL;
	SDL_RenderCopyEx(renderer, mTexture, src_rect, rect, 0, NULL, flip);
}
