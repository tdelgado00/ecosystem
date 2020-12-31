#ifndef _INTERFACE_H_
    #define _INTERFACE_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <string>
#include <vector>
#include "LTexture.h"
#include "defines.h"
#include "ecosystem.h"
#include "individual.h"
#include <stdlib.h>
#include <time.h>

const int FPS = 30;
const SDL_Color BLACK({0,0,0});

class interface {
public:
  interface(int,int,int,int,int);
  int OnExecute();
private:
  void OnCleanup();
  void OnEvent(SDL_Event* Event);
  void OnLoop();
  void OnRender();
  void load_media();
  void write_to_screen(string s, int x, int y, TTF_Font* font, const SDL_Color& color);
  void print_controls();
  vector<LTexture*> terrain;
  vector<LTexture*> animals;
  vector<LTexture*> item_media;
  SDL_Renderer* renderer;
  SDL_Window* window;
  SDL_Surface* display;
  bool Running = true;
  bool render_needed = true;
  bool paused = true;
  double step_freq = (double)FPS; //velocidad normal, cada FPS frames hago un step (un step por segundo)
  int frame_count = 0;
  int todo_steps = 0;
  ecosystem* e;

  int initial_rabbits;
  int initial_lions;
  int initial_humans;
  int e_width;
  int e_height;
  int screen_width;
  int screen_height;
  double px;
  int square_s;
};

#endif
