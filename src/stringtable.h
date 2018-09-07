#ifndef STRINGTABLE_H_
#define STRINGTABLE_H_

typedef struct item item;

struct item {
	char* string;
	item* next;
};

typedef struct {
	item** items;	
	int table_size;
	int filled_buckets;
} stringtable;

stringtable* stringtable_initialize();
void stringtable_destroy(stringtable* st);
stringtable* stringtable_read(const char* path);
int stringtable_write(stringtable* st, const char* path);
int stringtable_add(stringtable* st, const char* string);
int stringtable_del(stringtable* st, const char* string);
int stringtable_iterate(stringtable* st, void (*item_action)(item* it, void* param), void* param);
int stringtable_string_exists(stringtable* st, const char* string);

#endif
