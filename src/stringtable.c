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

#include "stringtable.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define INITIAL_TABLE_SIZE 64

unsigned int fnv1a_hash(const char* str, int len);

stringtable* stringtable_initialize() {
	stringtable* st = malloc(sizeof(stringtable));	
	st->items = calloc(INITIAL_TABLE_SIZE, sizeof(item*));
	st->table_size = INITIAL_TABLE_SIZE;
	st->total_entries = 0;
	return st;
}

void stringtable_destroy(stringtable* st) {
	void free_item(item* it, void* param);
	stringtable_iterate(st, free_item, NULL);	
	free(st->items);
	free(st);
}

void free_item(item* it, void* param) {
	free(it->string);
	free(it);
}

stringtable* stringtable_read(const char* path) {
	FILE* fp;
   	if((fp = fopen(path ,"rb")) == NULL) {
		return NULL;
	}
	stringtable* st = malloc(sizeof(stringtable));	
	
	fread(&st->table_size, sizeof(int), 1, fp);
	fread(&st->total_entries, sizeof(int), 1, fp);
	
	st->items = calloc(st->table_size, sizeof(item*));

	int string_len;
	while(fread(&string_len, sizeof(int), 1, fp)) {
		char string[string_len];
		fread(string, sizeof(char), string_len, fp);
		stringtable_add(st, string);
	}

	fclose(fp);
	return st;
}

int stringtable_write(stringtable* st, const char* path) {
	FILE* fp;
	if((fp = fopen(path, "wb")) == NULL) {
		return 1;
	}
	
	fwrite(&st->table_size, sizeof(st->table_size), 1, fp);
	fwrite(&st->total_entries, sizeof(st->total_entries), 1, fp);
	
	void write_string(item* it, void* fp);
	stringtable_iterate(st, write_string, fp);

	fclose(fp);
	return 0;
}

void write_string(item* it, void* fp) {
	char* string = it->string;
	int string_len = strlen(string) + 1;
	fwrite(&string_len, sizeof(int), 1, fp);
	fwrite(string, sizeof(char), string_len, fp);
}

int stringtable_add(stringtable* st, const char* string) {
	int return_code = 0;
	
	st->total_entries++;
	if(st->total_entries/0.75 > st->table_size) {
		//resize and rehash
		void resize_and_rehash(stringtable* st);
		resize_and_rehash(st);
	}

	unsigned int string_hash = fnv1a_hash(string, strlen(string)) & st->table_size - 1;	
	item** it = &st->items[string_hash];
	
//	printf("hash: %u\n", string_hash);

	int exists = 0;
	
	while((exists = *it != NULL) && strcmp((*it)->string, string)) {
		it = &(*it)->next;
	}

	if(!exists) {
		item* new_item = malloc(sizeof(item));	
		new_item->string = malloc(sizeof(char)*(strlen(string) + 1)); 
		strcpy(new_item->string, string);
		new_item->next = NULL;
		
		*it = new_item;
	} else {
		return_code = 1;
	} 

	return return_code;
}

int stringtable_del(stringtable* st, const char* string) {
	int return_code = 0;

	unsigned int string_hash = fnv1a_hash(string, strlen(string)) % st->table_size;	
	item* it = st->items[string_hash];

	while(it != NULL && strcmp(it->string, string)) {
		it = it->next;	
	}

	item* next = it->next;
	free(it);
	if(next == NULL) {
		st->total_entries--;	
		st->items[string_hash] = NULL;
	} else {
		st->items[string_hash] = next;
	}
}

int stringtable_iterate(stringtable* st, void (*item_action)(item* it, void* param), void* param) {
	for(int i = 0; i < st->table_size; i++) {
		item* it = st->items[i];
		if(it == NULL) {
			continue;
		}

		do {
			item* next = it->next;
			item_action(it, param);
			it = next;
		} while(it != NULL);
	}
}

int stringtable_string_exists(stringtable* st, const char* string) {
	int exists;
	unsigned int string_hash = fnv1a_hash(string, strlen(string)) % st->table_size;	
	item* it = st->items[string_hash];

	while((exists = it != NULL) && strcmp(it->string, string)) {
		it = it->next;
	}

	return exists;	
}

void resize_and_rehash(stringtable* st) {
	int new_size = st->table_size << 1;
	
	stringtable* new_table = malloc(sizeof(stringtable));	
	new_table->items = calloc(new_size, sizeof(item*));
	new_table->table_size = new_size;
	new_table->total_entries = 0;
	
	void add_item(item* it, void* table);
	stringtable_iterate(st, add_item, new_table);

	/* We want to set new_table to the old table so we can easily free all the 
	 * old strings and stuff without having to destroy the original 
	 * stringtable */
	item** items_buff = new_table->items;
	new_table->items = st->items;
	new_table->table_size = st->table_size;

	st->items = items_buff;
	st->table_size = new_size;

	stringtable_destroy(new_table);
}

void add_item(item* it, void* table) {
	stringtable_add(table, it->string);
}

unsigned int fnv1a_hash(const char* str, int len) {
	unsigned int hash = 2166136261;

	for(int i = 0; i < len; i++) {
		hash = (hash ^ str[i]) * 16777619;
	}

	return hash;
}
