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
