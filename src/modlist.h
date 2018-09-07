#ifndef MODLIST_H_
#define MODLIST_H_

#include "mod.h"

void modlist_update();
void modlist_iterate(void (*mod_action)(mod* m));

#endif
