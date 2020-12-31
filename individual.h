#ifndef _INDIVIDUAL_H_
    #define _INDIVIDUAL_H_

#include <iostream>
#include <vector>
#include <stdlib.h>
#include <cmath>
#include <queue>
#include "defines.h"
#include "ecosystem.h"
#include "mind.h"

using namespace std;

struct context {
  vector<pos> water;
  vector<pos> food; //bayas
  vector<pos> rock; //naturales+items
  vector<pos> wood; //naturales+items
  vector<pos> inds;
  vector<pos> meat;
  vector<vector<int> > dist;
  vector<vector<pos> > padre;
};

struct species {
  species_id id;
  int look_r;
  vector<int> age_stages;
  int pregnancy_time;
  bool eats_vegetables, eats_raw_meat;
  bool can_pick_up;
  vector<int> atk;
  vector<int> max_hp;
  vector<int> max_thirst;
  vector<int> max_hunger;
  int fear_dist;
  mind ai;
};

struct item_count {
  int rock = 0;
  int wood = 0;
  int food = 0;
  int raw_meat = 0;
  int spears = 0;
};

struct state {
  state(species* sp, pos p);
  pos p;
  int hp;
  dir a;
  int age;
  int pregnancy; //0 si no esta embarazado, si no es la cantidad de tiempo hasta que nazca
  int thirst;
  int hunger;
  int thirst_counter;
  int hunger_counter;
  int hpreg_counter;
  item_count bag;
};

class individual {
public:
  individual(species* sp, bool female, pos p);
//  void look_rad(context& c, ecosystem* e);
  int get_age_stage();
  action get_first_action(const vector<vector<pos> >& padres, pos v);
  void look_rad(context& c, ecosystem* e);
  bool food_or_water();

  species* sp;
  bool female;
  state s;
};

#endif
