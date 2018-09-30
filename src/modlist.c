/*
	This file belongs to Tiny Mod Manager, a simple console based mod manager.
	Copyright (C) 2018 9d8

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.

	https://github.com/9d8/tmm/
*/

#include "modlist.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include "stringtable.h"
#include "tmmpaths.h"
#include "util.h"

#ifdef _WIN32
#include <sys/stat.h>
#define NAME_MAX PATH_MAX
#endif

#define COMMENT_CHAR '/'

void modlist_update() {
	if(access(MOD_DIR, F_OK)) {
		mkdir_r(MOD_DIR);
	}

	FILE* modlist_file;
	if((modlist_file = fopen(MODLIST_FILE_PATH, "r+")) == NULL && (modlist_file = fopen(MODLIST_FILE_PATH, "w+")) == NULL) {
		return;
	}

	// Load list of mod names into memory based on directories found in the mods directory.
	
	int is_valid_mod_dir(const struct dirent* ent);

	DIR* mod_dir;
	if((mod_dir = opendir(MOD_DIR)) == NULL) {
		fclose(modlist_file);
		return;
	}

	stringtable* unaccounted_mods = stringtable_initialize(); 
	struct dirent* dir_file;
	while((dir_file = readdir(mod_dir)) != NULL) {
		int is_dir;
#ifdef _WIN32
		char dir_path[strlen(MOD_DIR) + strlen(dir_file->d_name) + 1];
		strcpy(dir_path, MOD_DIR);
		strcat(dir_path, dir_file->d_name);

		struct stat dirstats;
		stat(dir_path, &dirstats);
		is_dir = dirstats.st_mode & S_IFDIR;
#else
		is_dir = dir_file->d_type == DT_DIR;
#endif
		
		// We dont want to add the . and .. directories.
		int is_valid_dir = strcmp(dir_file->d_name, ".") && strcmp(dir_file->d_name, "..");
		if(is_dir && is_valid_dir) {
			stringtable_add(unaccounted_mods, dir_file->d_name);
		}
	}	

	closedir(mod_dir);

	// Load list of mod names stored in mods.txt into memory.
	
	int find_dirent_d_name(struct dirent** file_list, int len, char* str);
	void delete_chars(FILE* fp, int num);
	
	char strbuf[NAME_MAX];
	while(fgets(strbuf, NAME_MAX, modlist_file)) {
		strbuf[strcspn(strbuf, "\n")] = 0;
		
		// Do not remove or write commented mods from mods.txt
		if(strbuf[0] == COMMENT_CHAR) {
			stringtable_del(unaccounted_mods, strbuf+1);
			continue;
		}
		
		int exists = stringtable_string_exists(unaccounted_mods, strbuf);
		if(exists) {
//			printf("Found %s in mods!\n", strbuf);
			stringtable_del(unaccounted_mods, strbuf);
		} else {
//			printf("Delete %s!\n", strbuf);
			fseek(modlist_file, -(strlen(strbuf)+1), SEEK_CUR);
			delete_chars(modlist_file, strlen(strbuf) + 1);
		}
	}
	
	void write_mods_to_file(item* it, void* fp);
	stringtable_iterate(unaccounted_mods, write_mods_to_file, modlist_file);

	stringtable_destroy(unaccounted_mods);
	fclose(modlist_file);
}

void write_mods_to_file(item* it, void* fp) {
	fwrite(it->string, sizeof(char), strlen(it->string), fp);
	fwrite("\n", sizeof(char), 1, fp);
}

void modlist_iterate(void (*mod_action)(mod* m)) {
	FILE* modlist_file;
	if((modlist_file = fopen(MODLIST_FILE_PATH, "r")) == NULL) {
		return;
	}

	int mod_dir_len = strlen(MOD_DIR);
	char strbuf[NAME_MAX + mod_dir_len];
	strcpy(strbuf, MOD_DIR);
	while(fgets(strbuf + mod_dir_len, NAME_MAX, modlist_file)) {
		if(strbuf[mod_dir_len] == COMMENT_CHAR) {
			continue;
		}

		strbuf[strcspn(strbuf, "\n")] = 0;

		mod* m;
	   	if((m = mod_initialize(strbuf)) != NULL) {
			mod_action(m);
			mod_destroy(m);
		}
	}

	fclose(modlist_file);
}

void delete_chars(FILE* fp, int num) {
	long int start_pos = ftell(fp);
	fseek(fp, 0L, SEEK_END);
	long int file_length = ftell(fp);

	rewind(fp);

	char file_buff[file_length - num];
	fread(file_buff, sizeof(char), start_pos, fp);

	fseek(fp, num, SEEK_CUR);
	fread(file_buff + start_pos, sizeof(char), file_length - start_pos - num, fp);

	if(freopen(NULL, "w+", fp) == NULL) {
		return;
	}

	fwrite(file_buff, sizeof(char), sizeof(file_buff), fp);

	fseek(fp, start_pos, SEEK_SET);
}
