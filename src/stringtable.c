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
	st->filled_buckets = 0;
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
	fread(&st->filled_buckets, sizeof(int), 1, fp);
	
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
	fwrite(&st->filled_buckets, sizeof(st->filled_buckets), 1, fp);
	
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

	unsigned int string_hash = fnv1a_hash(string, strlen(string)) & st->table_size - 1;	
	item* it = st->items[string_hash];
	
//	printf("hash: %u\n", string_hash);

	int exists;
	
	if(it == NULL) {
		exists = 0;
		st->filled_buckets++;
		if(st->filled_buckets/0.75 > st->table_size) {
			//resize and rehash
			void resize_and_rehash(stringtable* st);
			resize_and_rehash(st);
		}
	} else {
		while((exists = it != NULL) && strcmp(it->string, string)) {
			it = it->next;
//			printf("exists %d\n", it != NULL);
		}
	}

	if(!exists) {
		it = malloc(sizeof(item));	
		it->string = malloc(sizeof(char)*(strlen(string) + 1)); 
		strcpy(it->string, string);
		it->next = NULL;
		
		st->items[string_hash] = it;
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
		st->filled_buckets--;	
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
	new_table->filled_buckets = 0;
	
	void add_item(item* it, void* table);
	stringtable_iterate(st, add_item, new_table);

	stringtable_destroy(st);
	st = new_table;
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
