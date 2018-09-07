#include "installer.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stringtable.h"
#include "util.h"
#include "tmmpaths.h"

stringtable* st;

void installer_init() {
	if(!access(PROGRAM_DATA, F_OK | W_OK | R_OK)) {
		st = stringtable_read(PROGRAM_DATA);	
	} else {
		st = stringtable_initialize();
	}
}

void installer_close() {
	stringtable_write(st, PROGRAM_DATA);
	stringtable_destroy(st);
}

void installer_install_mod(mod* m) {
	for(int i = 0; i < m->total_files; i++) {
		//create relative path to item gamefile path
		char* back = back_path(m->file_paths[i]);
		char mod_file_path[strlen(back) + strlen(MOD_DIR) + strlen(m->name) + strlen(m->file_paths[i]) + 2];
		strcpy(mod_file_path, back);
		strcat(mod_file_path, MOD_DIR);
		strcat(mod_file_path, m->name);
		strcat(mod_file_path, "/");
		strcat(mod_file_path, m->file_paths[i]);
		free(back);

		//check already exists in game
		int file_exists = !access(m->file_paths[i], F_OK);
		int file_is_owned = stringtable_add(st, m->file_paths[i]);

		if(file_exists) {
			if(file_is_owned) {
				//delete game file
				remove(m->file_paths[i]);
			} else {
				char backup_file_path[strlen(BACKUP_DIR) + strlen(m->file_paths[i]) + 1];
				strcpy(backup_file_path, BACKUP_DIR);
				strcat(backup_file_path, m->file_paths[i]);
				
				mkdir_r(dirname_const(backup_file_path));
				rename(m->file_paths[i], backup_file_path);
			}
		}

//		printf("%s\n", dirname_const(m->file_paths[i]));
//		printf("%s\n", m->file_paths[i]);

		//symlink mod file
		mkdir_r(dirname_const(m->file_paths[i]));
		symlink(mod_file_path, m->file_paths[i]);
	}
}

void installer_uninstall_all() {
	void installer_uninstall_file(item* it, void* param);
	stringtable_iterate(st, installer_uninstall_file, NULL);
}

void installer_uninstall_file(item* it, void* param) {
	char* file_path = it->string;
	char backup_file_path[strlen(BACKUP_DIR) + strlen(file_path) + 1];
	strcpy(backup_file_path, BACKUP_DIR);
	strcat(backup_file_path, file_path);

	remove(file_path);
	
	if(!access(backup_file_path, F_OK)) {
		rename(backup_file_path, file_path);
		rmdir_r(dirname_const(backup_file_path));
	} else {
		rmdir_r(dirname_const(file_path));
	}
	
	stringtable_del(st, file_path);
}
