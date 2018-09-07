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

#include "mod.h"
#include <stdlib.h>
#include <string.h>
#include <ftw.h>
#include "util.h"

struct string_list {
	char** list;
	int len;
	int size;
};
static struct string_list* file_list_buff = NULL;

int add_file_to_file_list(const char* fpath, const struct stat* sb, int typeflag);

mod* mod_initialize(char* mod_path) {
	mod* m = malloc(sizeof(mod));
	
	char mod_name[strlen(mod_path) + 1];
   	strcpy(mod_name, basename_const(mod_path));
	m->name = malloc(sizeof(char) * (strlen(mod_name) + 1));
	strcpy(m->name, mod_name);
	
	// will return null if no files are found inside the mod.
	if(ftw(mod_path, add_file_to_file_list, 15) || file_list_buff == NULL) {
		//error
		free(m->name);
		free(m);
		return NULL;
	}

	m->file_paths = malloc(sizeof(char*) * file_list_buff->len);
	memcpy(m->file_paths, file_list_buff->list, sizeof(char*) * file_list_buff->len);
	m->total_files = file_list_buff->len;

	free(file_list_buff->list);
	free(file_list_buff);
	file_list_buff = NULL;

	return m;
}

void mod_destroy(mod* m) {
	for(int i = 0; i < m->total_files; i++) {
		free(m->file_paths[i]);
	}
	free(m->file_paths);
	free(m->name);
	free(m);
}

int add_file_to_file_list(const char* fpath, const struct stat* sb, int typeflag) {
	if(typeflag != FTW_F) {
		return 0;
	}

	if(file_list_buff == NULL) {
		if((file_list_buff = malloc(sizeof(struct string_list))) == NULL) {
			return 1;
		}
		file_list_buff->list = malloc(sizeof(char*) * 20);
		file_list_buff->len = 0;
		file_list_buff->size = 20;
	}

	if(file_list_buff->len >= file_list_buff->size) {
		file_list_buff->size = file_list_buff->size * 2;
		if((file_list_buff->list = realloc(file_list_buff->list, sizeof(char*) * file_list_buff->size)) == NULL) {
			return 1;
		}
	}

	//fpath should be relative to game root, otherwise this doesnt work.
	char* game_root_path = strchr(strchr(fpath, '/') + 1, '/') + 1;

	file_list_buff->list[file_list_buff->len] = malloc(sizeof(char)*strlen(game_root_path));
	strcpy(file_list_buff->list[file_list_buff->len++], game_root_path);

	return 0;
}
