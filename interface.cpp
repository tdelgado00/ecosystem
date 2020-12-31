#include "interface.h"

using namespace std;

int main(int argc, char *argv[]) {
  int e_width, e_height, initial_rabbits, initial_lions, initial_humans;
  ifstream fin("params");
  if (fin.is_open()) {
    string buff;
    fin >> buff;
    fin >> e_width;
    fin >> buff;
    fin >> e_height;
    fin >> buff;
    fin >> initial_rabbits;
    fin >> buff;
    fin >> initial_lions;
    fin >> buff;
    fin >> initial_humans;
    fin.close();
  } else {
    cout << "Error loading parameters" << endl;
  }

  interface theInterface(e_width, e_height, initial_rabbits, initial_lions,
                         initial_humans);

  return theInterface.OnExecute();
}

interface::interface(int e_width, int e_height, int initial_rabbits,
                     int initial_lions, int initial_humans) {

  px = 1.0 / (((double)e_width) /
              100.0); // cantidad de pixeles reales por cada pixel de la textura
  square_s = 16.0 * px; // las texturas son de 16x16 px
  screen_width = (int)(square_s * e_width);
  screen_height = (int)(square_s * e_height);
  if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
    printf("SDL could not be initialized! SDL_Error: %s\n", SDL_GetError());
    exit(2);
  }
  if (TTF_Init() == -1) {
    printf("TTF_Init: %s\n", TTF_GetError());
    exit(2);
  }
  if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
    printf("Warning: Linear texture filtering not enabled!");
    exit(2);
  }
  window = SDL_CreateWindow("Fight", SDL_WINDOWPOS_UNDEFINED,
                            SDL_WINDOWPOS_UNDEFINED, screen_width,
                            screen_height, SDL_WINDOW_SHOWN);
  if (window == NULL) {
    printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
    exit(2);
  }
  renderer = SDL_CreateRenderer(
      window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (renderer == NULL) {
    printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
    exit(2);
  }
  display = SDL_GetWindowSurface(window);
  int imgFlags = IMG_INIT_PNG;
  if (!(IMG_Init(imgFlags) & imgFlags)) {
    printf("SDL_image could not initialize! SDL_image Error: %s\n",
           IMG_GetError());
    exit(2);
  }
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
  load_media();
  e = new ecosystem(e_width, e_height,
                    {initial_rabbits, initial_lions, initial_humans});
}

int interface::OnExecute() {
  print_controls();
  SDL_Event Event;
  double time_step = (1 / (double)FPS) * 1000.0;
  double start_time = SDL_GetTicks();
  double next_step = time_step;
  while (Running) {
    while (SDL_PollEvent(&Event))
      OnEvent(&Event);
    OnLoop();
    OnRender();
    double curr_time = SDL_GetTicks();
    double delta = next_step - (curr_time - start_time);
    if (delta > 0.0)
      SDL_Delay(delta);
    next_step += time_step;
  }
  OnCleanup();
  return 0;
}

void interface::load_media() {
  terrain.push_back(new LTexture(renderer, "img/grass.png"));
  terrain.push_back(new LTexture(renderer, "img/water.png"));
  terrain.push_back(new LTexture(renderer, "img/food.png"));
  terrain.push_back(new LTexture(renderer, "img/wood.png"));
  terrain.push_back(new LTexture(renderer, "img/rock.png"));
  terrain.push_back(new LTexture(renderer, "img/wall.png"));
  terrain.push_back(new LTexture(renderer, "img/fire.png"));
  item_media.push_back(new LTexture(renderer, "img/food_item.png"));
  item_media.push_back(new LTexture(renderer, "img/wood_item.png"));
  item_media.push_back(new LTexture(renderer, "img/rock_item.png"));
  item_media.push_back(new LTexture(renderer, "img/rawmeat_item.png"));
  item_media.push_back(new LTexture(renderer, "img/cookedmeat_item.png"));

  animals.push_back(new LTexture(renderer, "img/child_rabbit.png"));
  animals.push_back(new LTexture(renderer, "img/rabbit.png"));
  animals.push_back(new LTexture(renderer, "img/old_rabbit.png"));
  animals.push_back(new LTexture(renderer, "img/child_lion.png"));
  animals.push_back(new LTexture(renderer, "img/lion.png"));
  animals.push_back(new LTexture(renderer, "img/old_lion.png"));
  animals.push_back(new LTexture(renderer, "img/child_man.png"));
  animals.push_back(new LTexture(renderer, "img/man.png"));
  animals.push_back(new LTexture(renderer, "img/old_man.png"));
  animals.push_back(new LTexture(renderer, "img/child_woman.png"));
  animals.push_back(new LTexture(renderer, "img/woman.png"));
  animals.push_back(new LTexture(renderer, "img/old_woman.png"));
  animals.push_back(new LTexture(renderer, "img/child_spear_man.png"));
  animals.push_back(new LTexture(renderer, "img/spear_man.png"));
  animals.push_back(new LTexture(renderer, "img/old_spear_man.png"));
  animals.push_back(new LTexture(renderer, "img/child_spear_woman.png"));
  animals.push_back(new LTexture(renderer, "img/spear_woman.png"));
  animals.push_back(new LTexture(renderer, "img/old_spear_woman.png"));
}

void interface::OnCleanup() {
  e->print_stats();
  for (unsigned int i = 0; i < terrain.size(); i++)
    delete terrain[i];
  for (unsigned int i = 0; i < animals.size(); i++)
    delete animals[i];
  delete e;
  SDL_DestroyRenderer(renderer);
  SDL_FreeSurface(display);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

void interface::OnEvent(SDL_Event *Event) {
  if (Event->type == SDL_QUIT ||
      (Event->type == SDL_KEYDOWN && (Event->key).keysym.sym == SDLK_ESCAPE)) {
    Running = false;
  } else if (Event->type == SDL_KEYDOWN) {
    switch (Event->key.keysym.sym) {
    case SDLK_RIGHT:
      if (paused)
        todo_steps = 1;
      break;
    case SDLK_SPACE:
      paused = !paused;
      break;
    case SDLK_UP:
      if (!paused)
        step_freq /= 2.0;
      break;
    case SDLK_DOWN:
      if (!paused)
        step_freq *= 2.0;
      break;
    case SDLK_1:
      if (paused)
        todo_steps += e->year_dur * 1;
      break;
    case SDLK_2:
      if (paused)
        todo_steps += e->year_dur * 10;
      break;
    case SDLK_3:
      if (paused)
        todo_steps += e->year_dur * 100;
      break;
    case SDLK_4:
      if (paused)
        todo_steps += e->year_dur * 1000;
      break;
    case SDLK_5:
      if (paused)
        todo_steps += e->year_dur * 10000;
      break;
    case SDLK_6:
      if (paused)
        todo_steps += e->year_dur * 100000;
      break;
    }
  }
}

void interface::OnLoop() {
  if (paused) {
    frame_count = 0;
    if (todo_steps) {
      while (todo_steps) {
        e->step();
        todo_steps--;
      }
      render_needed = true;
    }
  } else {
    frame_count++;
    if (frame_count > step_freq) {
      int step_num = ((double)frame_count) / step_freq;
      for (int i = 0; i < step_num; i++)
        e->step();
      frame_count = 0;
      render_needed = true;
    }
  }
  if (e->get_step() > 1 && e->get_step() % e->year_dur == 0) {
    e->print_stats();
  }
}

void interface::OnRender() {
  if (render_needed) {
    SDL_RenderClear(renderer);
    TTF_Font *font10 = TTF_OpenFont("fonts/Lato-Black.ttf", 10);
    for (int i = 0; i < e->get_h(); i++) {
      for (int j = 0; j < e->get_w(); j++) {

        // render terreno
        SDL_Rect r = {square_s * j, square_s * i, square_s, square_s};
        terrain[e->geo_at(i, j)]->render(&r);

        // render items
        item_cell &curr_items = e->items_at(i, j);
        if (curr_items.raw_meat) {
          SDL_Rect r_meat = {r.x, r.y, r.w / 2, r.h / 2};
          item_media[RAW_MEAT_I]->render(&r_meat);
        }
        if (curr_items.rock) {
          SDL_Rect r_rock = {r.x, r.y + r.h / 2, r.w / 2, r.h / 2};
          item_media[ROCK_I]->render(&r_rock);
        }
        if (curr_items.wood) {
          SDL_Rect r_wood = {r.x + r.w / 2, r.y + r.h / 2, r.w / 2, r.h / 2};
          item_media[WOOD_I]->render(&r_wood);
        }

        individual *ind = e->ind_at(i, j);
        if (ind != NULL) {
          if (ind->sp->id == HUMAN) {
            int media =
                (HUMAN + 2 * (ind->s.bag.spears > 0) + ind->female) * 3 +
                ind->get_age_stage();
            animals[media]->renderAngle(&r, ind->s.a * 90, square_s / 2,
                                        square_s / 2);
          } else
            animals[ind->sp->id * 3 + ind->get_age_stage()]->renderAngle(
                &r, ind->s.a * 90, square_s / 2, square_s / 2);

          // if(ind -> female)
          //  write_to_screen("O", r.x+square_s-6*px, r.y+square_s-6*px, font10,
          //  BLACK);
          // else
          //  write_to_screen("X", r.x+square_s-6*px, r.y+square_s-6*px, font10,
          //  BLACK);

          // render de las barras de vida, hambre y sed
          SDL_Rect r_thirst = {
              square_s * j, (int)(square_s * i - 2 * px),
              (int)(((double)square_s) *
                    (((double)ind->s.thirst) /
                     (double)(ind->sp->max_thirst[ind->get_age_stage()]))),
              (int)(2 * px)};
          SDL_Rect r_hunger = {
              square_s * j, (int)(square_s * i - 4 * px),
              (int)(((double)square_s) *
                    (((double)ind->s.hunger) /
                     (double)(ind->sp->max_hunger[ind->get_age_stage()]))),
              (int)(2 * px)};
          SDL_Rect r_hp = {
              square_s * j, (int)(square_s * i - 6 * px),
              (int)(((double)square_s) *
                    (((double)ind->s.hp) /
                     (double)(ind->sp->max_hp[ind->get_age_stage()]))),
              (int)(2.0 * px)};
          SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
          SDL_RenderFillRect(renderer, &r_thirst);
          SDL_SetRenderDrawColor(renderer, 0x81, 0x34, 0x05, 255);
          SDL_RenderFillRect(renderer, &r_hunger);
          SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
          SDL_RenderFillRect(renderer, &r_hp);

          // render de edad
          // write_to_screen(to_string(ind -> age), r.x, r.y, font10, BLACK);
        }
      }
    }
    SDL_RenderPresent(renderer);
    TTF_CloseFont(font10);
    render_needed = false;
  }
}

void interface::write_to_screen(string s, int x, int y, TTF_Font *font,
                                const SDL_Color &color) {
  SDL_Surface *surface = TTF_RenderText_Blended(font, s.c_str(), color);
  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
  SDL_Rect dstrect = {x, y, 0, 0};
  SDL_QueryTexture(texture, NULL, NULL, &dstrect.w, &dstrect.h);
  SDL_RenderCopy(renderer, texture, NULL, &dstrect);
  SDL_DestroyTexture(texture);
  SDL_FreeSurface(surface);
}

void interface::print_controls() {
  cout << "Left arrow: advance 1 step" << endl;
  cout << "Space bar: enable or disable automatic time steps" << endl;
  cout << "Up arrow: double automatic time steps speed" << endl;
  cout << "Down arrow: halve automatic time steps speed" << endl;
}
