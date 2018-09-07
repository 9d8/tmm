#ifndef INSTALLER_H_
#define INSTALLER_H_

#include "mod.h"

void installer_init();
void installer_close();
void installer_install_mod(mod* mod);
//void installer_uninstall_mod(mod* mod);
void installer_uninstall_all();

#endif
