#include "modlist.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include "stringtable.h"
#include "tmmpaths.h"
#include "util.h"

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
	struct dirent* dirfile;
	while((dirfile = readdir(mod_dir)) != NULL) {
		if(dirfile->d_type == DT_DIR && strcmp(dirfile->d_name, ".") && strcmp(dirfile->d_name, "..")) {
			stringtable_add(unaccounted_mods, dirfile->d_name);
		}
	}	

	closedir(mod_dir);

	// Load list of mod names stored in mods.txt into memory.
	
	int find_dirent_d_name(struct dirent** file_list, int len, char* str);
	void delete_chars(FILE* fp, int num);
	
	char strbuf[NAME_MAX];
	while(fgets(strbuf, NAME_MAX, modlist_file)) {
		strbuf[strcspn(strbuf, "\n")] = 0;
		
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
