#include "installer.h"
#include "modlist.h"

int main(int argc, char** argv) {
	modlist_update();
	installer_init();

	installer_uninstall_all();
	
	void install_mod(mod* m);
	modlist_iterate(install_mod);
	
	installer_close();

	return 0;
}

void install_mod(mod* m) {
	installer_install_mod(m);
}
