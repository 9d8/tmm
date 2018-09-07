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
