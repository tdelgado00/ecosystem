#include "ecosystem.h"

action get_action_rabbit(individual *ind, ecosystem *e) {
  // return (action)(rand() % 5);

  // posicion de arriba a la izquierda del cuadrado que mira
  pos tl({ind->s.p.i - ind->sp->look_r, ind->s.p.j - ind->sp->look_r});

  context c;
  // bfs para obtener el contexto
  ind->look_rad(c, e);

  // busca pareja y amenazas
  pos couple({-1, -1});
  pos food({-1, -1});
  vector<pos> threats;
  for (unsigned i = 0; i < c.inds.size(); i++) {
    individual *other = e->ind_at(c.inds[i].i + tl.i, c.inds[i].j + tl.j);
    if (e->can_reproduce(ind, other) && couple.i == -1)
      couple = c.inds[i];
    else if (other->sp->id != RABBIT)
      threats.push_back(c.inds[0]);
  }

  // es medio dificil elegir una accion para escapar, por ahora digamos que
  // entran en panico
  if (threats.size() > 0 &&
      c.dist[threats[0].i][threats[0].j] < ind->sp->fear_dist)
    return (action)(rand() % 4);

  tipo look = e->get_geo_look(ind);

  if (look == WATER &&
      ind->s.thirst != ind->sp->max_thirst[ind->get_age_stage()])
    return INTERACT;
  if (look == FOOD &&
      ind->s.hunger != ind->sp->max_hunger[ind->get_age_stage()])
    return INTERACT;

  int thirst_need = 0.7 * ((double)ind->sp->max_thirst[ind->get_age_stage()]);
  int hunger_need = 0.7 * ((double)ind->sp->max_hunger[ind->get_age_stage()]);
  bool water_possible = ind->s.thirst < thirst_need && c.water.size() > 0;
  bool food_possible = ind->s.hunger < hunger_need && c.food.size() > 0;
  if (water_possible && (!food_possible || !ind->food_or_water()))
    return ind->get_first_action(c.padre, c.water[0]);
  if (food_possible)
    return ind->get_first_action(c.padre, c.food[0]);

  // la pareja es la ultima prioridad
  if (couple.i != -1)
    return ind->get_first_action(c.padre, couple);

  return (action)(rand() % 4);
}

action get_action_lion(individual *ind, ecosystem *e) {
  pos look = e->get_look(ind);
  tipo t_look = e->geo_at(look);
  individual *other = e->ind_at(look);
  item_cell &items_at_pos = e->items_at(ind->s.p);

  // si el leon tiene agua en frente y tiene un poquito de sed, toma agua
  if (t_look == WATER &&
      ind->s.thirst < ind->sp->max_thirst[ind->get_age_stage()])
    return INTERACT;

  // si el leon tiene a alguien en frente lo ataca incluso si no tiene hambre
  if (other != NULL && other->sp->id != LION) {
    return INTERACT;
  }

  int hunger_need = 0.7 * ((double)ind->sp->max_hunger[ind->get_age_stage()]);
  if (items_at_pos.raw_meat && ind->s.hunger < hunger_need) {
    return INTERACT;
  }

  context c;
  ind->look_rad(c, e);
  // es razonable asumir que los primeros elementos son los mas cercanos (bfs)

  pos arr_izq(
      {ind->s.p.i - ind->sp->look_r,
       ind->s.p.j - ind->sp->look_r}); // posicion de arriba a la izquierda
                                       // del cuadrado que mira

  // busco pareja y conejo o persona para atacar
  pos couple({-1, -1});
  pos rabbit({-1, -1});
  pos human({-1, -1});
  int found = 0;
  for (unsigned i = 0; i < c.inds.size() && found < 3; i++) {
    individual *other = e->ind_at(c.inds[i] + arr_izq);
    if (other->sp->id == RABBIT && rabbit.i == -1) {
      rabbit = c.inds[i];
      found++;
    } else if (other->sp->id == HUMAN && human.i == -1) {
      human = c.inds[i];
      found++;
    } else if (e->can_reproduce(ind, other) && couple.i == -1) {
      couple = c.inds[i];
      found++;
    }
  }
  // si hay un humano a distancia menor a fear_dist y tiene hambre lo ataca
  if (human.i != -1 && c.dist[human.i][human.j] <= ind->sp->fear_dist &&
      ind->s.hunger < hunger_need)
    return ind->get_first_action(c.padre, human);
  // si los humanos estan afuera de fear_dist, los ignora

  int thirst_need = 0.7 * ((double)ind->sp->max_thirst[ind->get_age_stage()]);
  bool water_possible = ind->s.thirst < thirst_need && c.water.size() > 0;
  bool meat_possible = ind->s.hunger < hunger_need && c.meat.size() > 0;
  if (water_possible && (!meat_possible || !ind->food_or_water()))
    return ind->get_first_action(c.padre, c.water[0]);
  if (meat_possible)
    return ind->get_first_action(c.padre, c.meat[0]);

  if (ind->s.hunger < hunger_need && rabbit.i != -1)
    return ind->get_first_action(c.padre, rabbit);

  // la pareja es la ultima prioridad
  if (couple.i != -1)
    return ind->get_first_action(c.padre, couple);

  return (action)(rand() % 4);
}

action get_action_human(individual *ind, ecosystem *e) {
  ////humano primitivo
  pos tl({ind->s.p.i - ind->sp->look_r, ind->s.p.j - ind->sp->look_r});
  // posicion de arriba a la izquierda del cuadrado que mira

  context c;
  ind->look_rad(c, e);

  // busco pareja, amenazas y conejos
  pos couple({-1, -1});
  pos rabbit({-1, -1});
  vector<pos> threats;
  for (unsigned i = 0; i < c.inds.size(); i++) {
    individual *other = e->ind_at(c.inds[i] + tl);
    if (e->can_reproduce(ind, other) && couple.i == -1)
      couple = c.inds[i];
    else if (other->sp->id == LION)
      threats.push_back(c.inds[i]);
    else if (other->sp->id == RABBIT && rabbit.i == -1)
      rabbit = c.inds[i];
  }

  // es medio dificil elegir una accion para escapar, por ahora digamos que
  // entran en panico
  if (threats.size() > 0 &&
      c.dist[threats[0].i][threats[0].j] < ind->sp->fear_dist) {
    return (action)(rand() % 4);
  }

  tipo look = e->get_geo_look(ind);

  if (look == WATER &&
      ind->s.thirst != ind->sp->max_thirst[ind->get_age_stage()]) {
    return INTERACT;
  }
  if (look == FOOD &&
      ind->s.hunger != ind->sp->max_hunger[ind->get_age_stage()]) {
    return INTERACT;
  }

  int hunger_need = 0.7 * ((double)ind->sp->max_hunger[ind->get_age_stage()]);
  int thirst_need = 0.7 * ((double)ind->sp->max_thirst[ind->get_age_stage()]);
  bool water_possible = ind->s.thirst < thirst_need && c.water.size() > 0;
  bool food_possible = ind->s.hunger < hunger_need && c.food.size() > 0;

  if (look == WOOD && !(water_possible || food_possible))
    return INTERACT;
  if (look == ROCK && !(water_possible || food_possible))
    return INTERACT;

  if (water_possible && (!food_possible || !ind->food_or_water()))
    return ind->get_first_action(c.padre, c.water[0]);
  if (food_possible) {
    cout << "going for food" << endl;
    return ind->get_first_action(c.padre, c.food[0]);
  }

  // la pareja es la ultima prioridad
  if (couple.i != -1)
    return ind->get_first_action(c.padre, couple);

  return (action)(rand() % 4);
}
