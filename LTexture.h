#ifndef _LTEXTURE_H_
		#define _LTEXTURE_H_
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>

using namespace std;

class LTexture
{
	public:
		LTexture(SDL_Renderer*);
		LTexture(SDL_Renderer*, SDL_Texture*);
		LTexture(SDL_Renderer*, string path);
		~LTexture();
		bool loadFromFile(string path);
		void free();

		//rendering functions
		void render(SDL_Rect* rect, SDL_Rect* src_rect = NULL);
		void renderAngle(SDL_Rect* rect, double angle, int center_x, int center_y);
		void renderInvX(SDL_Rect* rect, SDL_Rect* src_rect = NULL);

		//editing functions
		void setColor(Uint8 red, Uint8 green, Uint8 blue);
    void setBlendMode(SDL_BlendMode blending);
		void setAlpha(Uint8 alpha);

		SDL_Renderer* renderer;
		SDL_Texture* mTexture;
};

#endif
