#include "individual.h"

state::state(species *sp, pos _p)
    : p(_p), hp(sp->max_hp[0]), a((dir)(rand() % 4)), age(0), pregnancy(0),
      thirst(sp->max_thirst[0]), hunger(sp->max_hunger[0]), thirst_counter(0),
      hunger_counter(0), hpreg_counter(0){};

individual::individual(species *_sp, bool _female, pos _p)
    : sp(_sp), female(_female), s(_sp, _p){};

action individual::get_first_action(const vector<vector<pos>> &padres, pos v) {
  action act = A_NONE;
  while (padres[v.i][v.j].i != -2) {
    pos padre = padres[v.i][v.j];
    if (padre.i + 1 == v.i)
      act = DOWN;
    if (padre.i - 1 == v.i)
      act = UP;
    if (padre.j + 1 == v.j)
      act = RIGHT;
    if (padre.j - 1 == v.j)
      act = LEFT;
    v = padre;
  }
  return act;
}

void individual::look_rad(context &c, ecosystem *e) {
  // bfs desde la posicion del individuo
  // asume que todos los vectores vienen vacios

  pos tl({s.p.i - sp->look_r,
          s.p.j - sp->look_r}); // posicion de arriba a la izquierda del
                                // cuadrado que recorro

  c.dist = vector<vector<int>>(sp->look_r * 2 + 1,
                               vector<int>(sp->look_r * 2 + 1, INT_MAX));
  c.padre = vector<vector<pos>>(sp->look_r * 2 + 1,
                                vector<pos>(sp->look_r * 2 + 1, pos({-1, -1})));
  queue<pos> cola;

  // seteo la posicion actual
  c.dist[sp->look_r][sp->look_r] = 0;
  c.padre[sp->look_r][sp->look_r] = pos({-2, -2});
  cola.push(pos({sp->look_r, sp->look_r}));

  while (!cola.empty()) {
    pos actual = cola.front();
    cola.pop();
    for (int k = 0; k < 4; k++) { // por cada vecino de actual
      pos v = pos({actual.i + deltas[k].i, actual.j + deltas[k].j});
      pos v_real = pos({v.i + tl.i, v.j + tl.j});
      bool v_en_rango = v.i >= 0 && v.i < sp->look_r * 2 + 1 && v.j >= 0 &&
                        v.j < sp->look_r * 2 + 1;
      bool v_real_en_rango = v_real.i >= 0 && v_real.i < e->get_h() &&
                             v_real.j >= 0 && v_real.j < e->get_w();
      if (v_en_rango && v_real_en_rango && c.padre[v.i][v.j].i == -1) {
        tipo t = e->geo_at(v_real);
        individual *ind = e->ind_at(v_real);
        item_cell &cell = e->items_at(v_real);
        if (t == FOOD)
          c.food.push_back(v);
        if (t == WATER)
          c.water.push_back(v);
        if (t == ROCK || cell.rock)
          c.rock.push_back(v);
        if (t == WOOD || cell.wood)
          c.wood.push_back(v);
        if (cell.raw_meat)
          c.meat.push_back(v);
        if (ind != NULL)
          c.inds.push_back(v);

        c.padre[v.i][v.j] = actual;
        c.dist[v.i][v.j] = c.dist[actual.i][actual.j] + 1;
        if (t == GRASS && ind == NULL) // solo puede avanzar si t es GRASS y no
                                       // hay ningun individuo
          cola.push(v);
      }
    }
  }
}

bool individual::food_or_water() {
  // calcula con cual se va a morir primero
  // true si prefiere prefiere comer a beber
  return THIRST_TICKS * s.thirst >= HUNGER_TICKS * s.hunger;
}

int individual::get_age_stage() {
  if (s.age >= sp->age_stages[1])
    return 2;
  if (s.age >= sp->age_stages[0])
    return 1;
  return 0;
}
