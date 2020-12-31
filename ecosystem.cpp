#include "ecosystem.h"

ecosystem::ecosystem(int w_in, int h_in, vector<int> count_species)
    : w(w_in), h(h_in), m(h_in, vector<tipo>(w_in, GRASS)),
      im(h_in, vector<individual *>(w_in, NULL)),
      items(h_in, vector<item_cell>(w_in, item_cell())) {
  srand(time(0));
  add_water();
  add_resources();

  // defino a las especies
  for (int i = 0; i < 3; i++) {
    ecosystem_species[i] = new species();
  }

  ecosystem_species[0]->id = RABBIT;
  ecosystem_species[0]->look_r = 6;
  ecosystem_species[0]->pregnancy_time = (int)(0.2 * year_dur);
  ecosystem_species[0]->eats_vegetables = true;
  ecosystem_species[0]->eats_raw_meat = false;
  ecosystem_species[0]->can_pick_up = false;
  ecosystem_species[0]->atk = vector<int>({0, 1, 0});
  ecosystem_species[0]->max_hp = vector<int>({3, 10, 8});
  ecosystem_species[0]->max_thirst = vector<int>({10, 3, 4}); // definir
  ecosystem_species[0]->max_hunger = vector<int>({5, 3, 4});  // definir
  ecosystem_species[0]->age_stages = vector<int>(
      {(int)(0.5 * year_dur), (int)(2.25 * year_dur), (int)(3 * year_dur)});
  ecosystem_species[0]->fear_dist = 3;
  ecosystem_species[0]->ai = get_action_rabbit;

  ecosystem_species[1]->id = LION;
  ecosystem_species[1]->look_r = 6;
  ecosystem_species[1]->pregnancy_time = (int)(0.5 * year_dur);
  ecosystem_species[1]->eats_vegetables = false;
  ecosystem_species[1]->eats_raw_meat = true;
  ecosystem_species[1]->can_pick_up = false;
  ecosystem_species[1]->atk = vector<int>({3, 10, 8});
  ecosystem_species[1]->max_hp = vector<int>({15, 50, 40});
  ecosystem_species[1]->max_thirst = vector<int>({10, 3, 4}); // definir
  ecosystem_species[1]->max_hunger = vector<int>({5, 3, 4});  // definir
  ecosystem_species[1]->age_stages =
      vector<int>({3 * year_dur, 10 * year_dur, 14 * year_dur});
  ecosystem_species[1]->fear_dist = 3;
  ecosystem_species[1]->ai = get_action_lion;

  ecosystem_species[2]->id = HUMAN;
  ecosystem_species[2]->look_r = 5;
  ecosystem_species[2]->pregnancy_time = (int)(0.66 * year_dur);
  ecosystem_species[2]->eats_vegetables = true;
  ecosystem_species[2]->eats_raw_meat = false;
  ecosystem_species[2]->can_pick_up = true;
  ecosystem_species[2]->atk = vector<int>({1, 4, 3});
  ecosystem_species[2]->max_hp = vector<int>({15, 50, 40});
  ecosystem_species[2]->max_thirst = vector<int>({10, 3, 4}); // definir
  ecosystem_species[2]->max_hunger = vector<int>({5, 3, 4});  // definir
  ecosystem_species[2]->age_stages =
      vector<int>({14 * year_dur, 35 * year_dur, 50 * year_dur});
  ecosystem_species[0]->fear_dist = 3;
  ecosystem_species[2]->ai = get_action_human;

  // agrego a los individuos
  for (unsigned i = 0; i < count_species.size(); i++) {
    for (int j = 0; j < count_species[i]; j++) {
      pop[i].push_back(
          new individual(ecosystem_species[i], rand() % 2, get_random_valid()));
    }
  }
};

void ecosystem::add_water() {
  for (int i = 0; i < h; i++) {
    m[i][0] = WATER;
    m[i][w - 1] = WATER;
  }
  for (int j = 0; j < w; j++) {
    m[0][j] = WATER;
    m[h - 1][j] = WATER;
  }
  double p_water = 0.002, p_near_water = 0.6;
  for (int k = 1; k < min(w, h) / 2 + min(w, h) % 2; k++) {
    for (int i = k; i < h - 1 - k; i++)
      add_water_cell(pos({i, k}), p_water, p_near_water);
    for (int i = k; i < h - 1 - k; i++)
      add_water_cell(pos({i, w - 1 - k}), p_water, p_near_water);
    for (int j = 1 + k; j < w - 2 - k; j++)
      add_water_cell(pos({k, j}), p_water, p_near_water);
    for (int j = 1 + k; j < w - 2 - k; j++)
      add_water_cell(pos({h - 1 - k, j}), p_water, p_near_water);
  }

  save_stats(true);
}

void ecosystem::add_resources() {
  double p_food = 0.02;
  double p_wood = 0.01;
  double p_rock = 0.01;
  for (int i = 0; i < h; i++) {
    for (int j = 0; j < w; j++) {
      if (m[i][j] != WATER) {
        float r = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
        if (r < p_food)
          m[i][j] = FOOD;
        else if (r < p_food + p_wood)
          m[i][j] = WOOD;
        else if (r < p_food + p_wood + p_rock)
          m[i][j] = ROCK;
      }
    }
  }
}

void ecosystem::step() {
  for (int i = 0; i < 3; i++)
    compute_actions((species_id)i);
  for (int i = 0; i < 3; i++)
    kill_dead((species_id)i);
  for (int i = 0; i < 3; i++)
    for (unsigned j = 0; j < pop[i].size(); j++)
      update_stats(pop[i][j]);
  for (int i = 0; i < 3; i++)
    check_reproduction((species_id)i);

  if (get_step() > 1 && get_step() % year_dur == 0) {
    save_stats();
  }
  step_count++;
};

void ecosystem::compute_actions(species_id i) {
  for (auto it = pop[i].begin(); it != pop[i].end(); it++) {
    action act = (*it)->sp->ai(*it, this);
    pos look = get_look(*it);

    if (act < INTERACT) // es un movimiento
      move_ind(*it, act);
    else if (act == INTERACT) {
      pos p = (*it)->s.p;
      if ((*it)->sp->id == LION && items[p.i][p.j].raw_meat) {
        (*it)->s.hunger = min((*it)->s.hunger + 5,
                              (*it)->sp->max_hunger[(*it)->get_age_stage()]);
        items[p.i][p.j].raw_meat--;
      } else if (m[look.i][look.j] == WATER) {
        (*it)->s.thirst = min((*it)->s.thirst + 1,
                              (*it)->sp->max_thirst[(*it)->get_age_stage()]);
      } else if (m[look.i][look.j] == FOOD) {
        (*it)->s.hunger = min((*it)->s.hunger + 1,
                              (*it)->sp->max_hunger[(*it)->get_age_stage()]);
      } else if (m[look.i][look.j] == WOOD) {
        (*it)->s.bag.wood++;
        stats.wood_collected++;
      } else if (m[look.i][look.j] == ROCK) {
        (*it)->s.bag.rock++;
        stats.rock_collected++;
      } else if (m[look.i][look.j] == FIRE) {
        if ((*it)->s.bag.raw_meat > 0) {
          stats.meat_cooked++;
          (*it)->s.bag.raw_meat--;
          (*it)->s.hunger = min((*it)->s.hunger + 5,
                                (*it)->sp->max_hunger[(*it)->get_age_stage()]);
        }
      } else if (m[look.i][look.j] == GRASS) {
        individual *other = im[look.i][look.j];
        if ((*it)->sp->id == HUMAN)
          give_items(*it, items[p.i][p.j]);
        if (other != NULL) {
          if (attack((*it), other))
            items[look.i][look.j].raw_meat +=
                other->sp->max_hp[other->get_age_stage()];
        }
      }
    } else if (act == DROP_WOOD && (*it)->s.bag.wood > 0 &&
               m[look.i][look.j] == GRASS) {
      craft(WOOD_I, *it);
      (*it)->s.bag.wood--;
    } else if (act == DROP_ROCK && (*it)->s.bag.rock > 0 &&
               m[look.i][look.j] == GRASS) {
      craft(ROCK_I, *it);
      (*it)->s.bag.rock--;
    }
  }
}

void ecosystem::give_items(individual *ind, item_cell &cell) {
  ind->s.bag.wood += cell.wood;
  ind->s.bag.rock += cell.rock;
  ind->s.bag.raw_meat += cell.raw_meat;
  cell.wood = 0;
  cell.rock = 0;
  cell.raw_meat = 0;
}

void ecosystem::craft(item it, individual *ind) {
  pos p = get_look(ind);
  item_cell &cell = items[p.i][p.j];
  // asume que it es rock o wood, los unicos crafteos posibles son:
  // WALL: 2 rock
  // FIRE: 2 wood
  // SPEAR: 1 wood 1 rock
  // por lo tanto, a lo sumo puede haber 1 rock o 1 wood en una celda
  // se podria hacer mas lindo me parece, muchos ifs
  if (((cell.wood > 0) && (it == ROCK_I)) ||
      ((it == WOOD_I) && (cell.rock > 0))) {
    ind->s.bag.spears++;
    cell.wood = 0;
    cell.rock = 0;
    stats.spears_crafted++;
  } else if (cell.wood > 0) {
    m[p.i][p.j] = FIRE;
    cell.wood--;
    stats.fires_crafted++;
  } else if (cell.rock > 0) {
    stats.walls_crafted++;
    cell.rock--;
    m[p.i][p.j] = WALL;
  } else {
    if (it == WOOD_I)
      cell.wood++;
    else
      cell.rock++;
  }
}

void ecosystem::check_reproduction(species_id i) {
  vector<individual *> new_individuals;
  int size = pop[i].size();
  for (int ind_num = 0; ind_num < size; ind_num++) {
    individual *ind = pop[i][ind_num];
    if (ind->female) {
      if (ind->s.pregnancy == 0) {
        individual *other = get_ind_look(ind);
        if (other != NULL && can_reproduce(ind, other))
          ind->s.pregnancy = ind->sp->pregnancy_time;
      } else if (ind->s.pregnancy == 1) {
        pos ady;
        if (get_empty_ady(ind, ady)) {
          pop[i].push_back(
              new individual(ecosystem_species[i], rand() % 2, ady));
          stats.total[i]++;
          ind->s.pregnancy = 0;
        }
      } else {
        ind->s.pregnancy--;
      }
    }
  }
}

bool ecosystem::can_reproduce(individual *ind1, individual *ind2) {
  return ind1->sp->id == ind2->sp->id && ind1->female != ind2->female &&
         ind1->get_age_stage() == 1 && ind2->get_age_stage() == 1 &&
         ind1->s.pregnancy + ind2->s.pregnancy == 0;
}

void ecosystem::kill_dead(species_id i) {
  auto it = pop[i].begin();
  while (it != pop[i].end())
    if ((*it)->s.hp <= 0) {
      im[(*it)->s.p.i][(*it)->s.p.j] = NULL;
      delete (*it);
      it = pop[i].erase(it);
    } else
      it++;
}

bool ecosystem::attack(individual *ind, individual *prey) {
  int atk = ind->sp->atk[ind->get_age_stage()] + 8 * (ind->s.bag.spears > 0);
  prey->s.hp -= atk;
  if (ind->s.bag.spears > 0) {
    float x = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    if (x < 0.25) {
      ind->s.bag.spears--;
      stats.broken_spears++;
    }
  }
  if (prey->s.hp <= 0)
    stats.deaths[prey->sp->id][(death)ind->sp->id]++;
  return prey->s.hp <= 0;
}

void ecosystem::update_stats(individual *ind) {
  ind->s.thirst_counter++;
  ind->s.hunger_counter++;
  if (ind->s.thirst >=
          (int)((float)ind->sp->max_thirst[ind->get_age_stage()] * HPREG_REQ) &&
      ind->s.hunger >=
          (int)((float)ind->sp->max_hunger[ind->get_age_stage()] * HPREG_REQ) &&
      ind->s.hp != ind->sp->max_hp[ind->get_age_stage()])
    ind->s.hpreg_counter++;

  if (ind->s.hpreg_counter == HPREG_TICKS &&
      ind->s.hp != ind->sp->max_hp[ind->get_age_stage()]) {
    ind->s.hp++;
    ind->s.hpreg_counter = 0;
  }

  if (ind->s.thirst_counter == THIRST_TICKS) {
    ind->s.thirst_counter = 0;
    if (ind->s.thirst > 0) {
      ind->s.thirst--;
    } else {
      ind->s.hp -= ind->sp->max_hp[ind->get_age_stage()] / 10;
      if (ind->s.hp <= 0)
        stats.deaths[ind->sp->id][D_THIRST]++;
    }
  }

  if (ind->s.hunger_counter == HUNGER_TICKS) {
    ind->s.hunger_counter = 0;
    if (ind->s.hunger > 0) {
      ind->s.hunger--;
    } else {
      ind->s.hp -= ind->sp->max_hp[ind->get_age_stage()] / 10;
      if (ind->s.hp <= 0)
        stats.deaths[ind->sp->id][D_HUNGER]++;
    }
  }

  ind->s.age++;
  if (ind->s.age == ind->sp->age_stages[0])
    ind->s.hp += ind->sp->max_hp[1] - ind->sp->max_hp[0];
  else if (ind->s.age == ind->sp->age_stages[1])
    ind->s.hp += ind->sp->max_hp[2] -
                 ind->sp->max_hp[1]; // Obs: esto en general resta vida

  if (ind->s.age % year_dur == 0) { // no es el cumpleaños mas feliz...
    // la probabilidad de morir aumenta con la edad, en max_age se mueren todos
    float x = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    if (x < pow(ind->s.age / ind->sp->age_stages[2], 6)) {
      ind->s.hp = 0;
      stats.deaths[ind->sp->id][D_AGE]++;
    }
  }
}

void ecosystem::move_ind(individual *r, int act) {
  im[r->s.p.i][r->s.p.j] = NULL;
  int new_i = r->s.p.i;
  int new_j = r->s.p.j;
  switch (act) {
  case UP:
    new_i -= 1;
    r->s.a = N;
    break;
  case DOWN:
    new_i += 1;
    r->s.a = S;
    break;
  case LEFT:
    new_j -= 1;
    r->s.a = W;
    break;
  case RIGHT:
    new_j += 1;
    r->s.a = E;
  }
  if (valid_pos(pos({new_i, new_j}))) {
    r->s.p.i = new_i;
    r->s.p.j = new_j;
  }
  im[r->s.p.i][r->s.p.j] = r;
};

bool ecosystem::valid_pos(pos p) {
  bool in_range = p.j > 0 && p.j < w && p.i > 0 && p.i < h;
  return in_range && m[p.i][p.j] == GRASS && im[p.i][p.j] == NULL;
};

pos ecosystem::get_look(individual *r) {
  pos p = r->s.p;
  switch (r->s.a) {
  case N:
    p.i--;
    break;
  case S:
    p.i++;
    break;
  case W:
    p.j--;
    break;
  case E:
    p.j++;
  }
  return p;
}

individual *ecosystem::get_ind_look(individual *r) {
  pos p = get_look(r);
  return im[p.i][p.j];
}

tipo ecosystem::get_geo_look(individual *r) {
  pos p = get_look(r);
  return m[p.i][p.j];
}

item_cell &ecosystem::get_item_look(individual *r) {
  pos p = get_look(r);
  return items[p.i][p.j];
}

ecosystem::~ecosystem() {
  for (int i = 0; i < 3; i++)
    for (unsigned j = 0; j < pop[i].size(); j++)
      delete pop[i][j];
}

pos ecosystem::get_random_valid() {
  pos p({rand() % h, rand() % w});
  while (!valid_pos(p)) {
    p.i = rand() % h;
    p.j = rand() % w;
  }
  return p;
}

void ecosystem::add_water_cell(pos p, double p_water, double p_near_water) {
  float r = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
  int near_water = (p.i > 0 && m[p.i - 1][p.j] == WATER) ||
                   (p.i < h - 1 && m[p.i + 1][p.j] == WATER);
  near_water += (p.j > 0 && m[p.i][p.j - 1] == WATER) +
                (p.j < w - 1 && m[p.i][p.j + 1] == WATER);
  if (r < p_water || (near_water && r < 0.6))
    m[p.i][p.j] = WATER;
}

bool ecosystem::get_empty_ady(individual *ind, pos &ady) {
  vector<pos> posibles;
  pos ady_up = pos({ind->s.p.i - 1, ind->s.p.j});
  pos ady_down = pos({ind->s.p.i + 1, ind->s.p.j});
  pos ady_left = pos({ind->s.p.i, ind->s.p.j - 1});
  pos ady_right = pos({ind->s.p.i, ind->s.p.j + 1});
  if (valid_pos(ady_up) && !ind_at(ady_up))
    posibles.push_back(ady_up);
  if (valid_pos(ady_down) && !ind_at(ady_down))
    posibles.push_back(ady_down);
  if (valid_pos(ady_left) && !ind_at(ady_left))
    posibles.push_back(ady_left);
  if (valid_pos(ady_right) && !ind_at(ady_right))
    posibles.push_back(ady_right);
  if (posibles.size() == 0)
    return false;
  else {
    ady = posibles[rand() % posibles.size()];
    return true;
  }
}

void ecosystem::save_stats(bool is_start) {
  fstream fout;
  fout.open("data/log", ios::app);
  if (is_start) {
    fout << "year,step,gen_rabbits,gen_lions,gen_humans,curr_rabbits,curr_"
            "lions,curr_humans,total_rabbits,total_lions,total_humans,fires,"
            "walls,spears,broken_sp,meat_cooked,rocks,woods,"
         << endl;
  }
  fout << step_count / year_dur << "," << step_count << ",";
  for (int i = 0; i < 3; i++)
    fout << stats.max_gen[i] << ",";
  for (int i = 0; i < 3; i++)
    fout << pop[i].size() << ",";
  for (int i = 0; i < 3; i++)
    fout << stats.total[i] << ",";
  fout << stats.fires_crafted << ",";
  fout << stats.walls_crafted << ",";
  fout << stats.spears_crafted << ",";
  fout << stats.broken_spears << ",";
  fout << stats.meat_cooked << ",";
  fout << stats.rock_collected << ",";
  fout << stats.wood_collected << ",";
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 6; j++)
      fout << stats.deaths[i][j] << ",";
  }
  fout << endl;
  fout.close();
}

void ecosystem::print_stats() {
  cout << "-------------------------------" << endl;
  cout << "Years: " << step_count / year_dur << endl;
  cout << "Steps: " << step_count << endl;
  cout << "Gen: ";
  for (int i = 0; i < 3; i++)
    cout << stats.max_gen[i] << " ";
  cout << endl;
  cout << "Total: ";
  for (int i = 0; i < 3; i++)
    cout << stats.total[i] << " ";
  cout << endl;
  cout << "fires_crafted: " << stats.fires_crafted << endl;
  cout << "walls_crafted: " << stats.walls_crafted << endl;
  cout << "spears_crafted: " << stats.spears_crafted << endl;
  cout << "broken_spears: " << stats.broken_spears << endl;
  cout << "meat_cooked: " << stats.meat_cooked << endl;
  cout << "rock_collected: " << stats.rock_collected << endl;
  cout << "wood_collected: " << stats.wood_collected << endl;
  cout << "Deaths" << endl;
  cout << "SPECIES RABBIT LION HUMAN AGE THIRST HUNGER" << endl;
  for (int i = 0; i < 3; i++) {
    cout << s_species[i] << ": ";
    for (int j = 0; j < 6; j++)
      cout << stats.deaths[i][j] << " ";
    cout << endl;
  }
}
