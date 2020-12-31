#ifndef _ECOSYSTEM_H_
#define _ECOSYSTEM_H_

#include "defines.h"
#include "individual.h"
#include "mind.h"
#include <cmath>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <vector>

using namespace std;

const double HPREG_REQ = 0.8;
const int HPREG_TICKS = 8;
const int THIRST_TICKS = 5;
const int HUNGER_TICKS = 10;

struct statistics {
  vector<int> max_gen = vector<int>(3, 0);
  vector<int> total = vector<int>(3, 0);
  int fires_crafted = 0;
  int walls_crafted = 0;
  int spears_crafted = 0;
  int broken_spears = 0;
  int meat_cooked = 0;
  int rock_collected = 0;
  int wood_collected = 0;
  vector<vector<int>> deaths =
      vector<vector<int>>(3, vector<int>(6, 0)); // muertes por especie
};

class ecosystem {
private:
  int w;
  int h;

  geo_map m;
  ind_map im;
  item_map items;

  int step_count = 0;

  species *ecosystem_species[3];
  vector<individual *> pop[3];

  statistics stats;

  void update_stats(individual *r);
  void move_ind(individual *r, int act);
  bool valid_pos(pos p);
  bool get_empty_ady(individual *r, pos &ady);
  void add_water();
  void add_water_cell(pos p, double p_water, double p_near_water);
  void add_resources();
  pos get_random_valid();
  void kill_dead(species_id i);
  bool attack(individual *ind, individual *prey);
  void compute_actions(species_id i);
  void check_reproduction(species_id i);
  void craft(item it, individual *ind);
  void give_items(individual *ind, item_cell &cell);

public:
  ecosystem(int w, int h, vector<int> count_species);
  ~ecosystem();
  void step();

  pos get_look(individual *r);
  individual *get_ind_look(individual *r);
  tipo get_geo_look(individual *r);
  item_cell &get_item_look(individual *r);
  int get_w() { return w; };
  int get_h() { return h; };
  int get_step() { return step_count; };
  tipo geo_at(int i, int j) { return m[i][j]; };
  individual *ind_at(int i, int j) { return im[i][j]; };
  tipo geo_at(pos p) { return m[p.i][p.j]; };
  individual *ind_at(pos p) { return im[p.i][p.j]; };
  item_cell &items_at(int i, int j) { return items[i][j]; };
  item_cell &items_at(pos p) { return items[p.i][p.j]; };
  bool can_reproduce(individual *ind1, individual *ind2);

  void print_stats();
  void save_stats(bool is_start = false);

  const int year_dur = 100;
};

#endif
