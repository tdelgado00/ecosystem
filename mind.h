#ifndef _MIND_H_
#define _MIND_H_

#include "defines.h"

typedef action (*mind)(individual*, ecosystem*);

action get_action_rabbit(individual*, ecosystem*);
action get_action_lion(individual*, ecosystem*);
action get_action_human(individual*, ecosystem*);

#endif
