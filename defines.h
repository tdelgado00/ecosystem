#ifndef _DEFINES_H_
    #define _DEFINES_H_

#include <vector>
#include <iostream>

using namespace std;

const int INT_MAX = 2147483647;

class ecosystem;
class individual;
struct species;

struct pos{
  int i;
  int j;
  void print(){cout << "(" << i << ", " << j << ") ";}
  pos operator+(pos p){return pos({p.i+i, p.j+j});};
};

enum tipo{
  GRASS,
  WATER,
  FOOD,
  WOOD,
  ROCK,
  WALL,
  FIRE,
  T_NONE
};

enum item{
  FOOD_I,
  WOOD_I,
  ROCK_I,
  RAW_MEAT_I,
  COOKED_MEAT_I,
};

struct item_cell{
  int wood = 0;
  int rock = 0;
  int raw_meat = 0;
};

enum species_id{
  RABBIT,
  LION,
  HUMAN
};

enum dir{
  N,
  E,
  S,
  W,
};

enum action{
  LEFT,
  RIGHT,
  UP,
  DOWN,
  INTERACT,
  DROP_WOOD,
  DROP_ROCK,
  //GIVE_WOOD,
  //GIVE_ROCK,
  //GIVE_FOOD,
  A_NONE
};

enum death{
  D_RABBIT,
  D_LION,
  D_HUMAN,
  D_AGE,
  D_THIRST,
  D_HUNGER,
};

typedef vector<vector<tipo> > geo_map;
typedef vector<vector<individual*> > ind_map;
typedef vector<vector<item_cell> > item_map;
const pos deltas[4] = {pos({0,-1}),pos({0,1}),pos({-1,0}),pos({1,0})};
const string s_actions[6] = {"LEFT", "RIGHT", "UP", "DOWN", "INTERACT", "NONE"};
const string s_species[3] = {"RABBIT", "LION", "HUMAN"};

#endif
