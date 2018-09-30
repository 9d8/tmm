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

#include "util.h"
#include <string.h>
#include <sys/stat.h>
#include <libgen.h>
#include <stdlib.h>
#include <unistd.h>

#ifdef _WIN32
#define MKDIR(path, mode) mkdir(path)
#else
#define MKDIR(path, mode) mkdir(path, mode)
#endif

void mkdir_r(const char* path) {
	char tmp[256];
	strncpy(tmp, path, sizeof(tmp));
	int len = strlen(tmp);

	if(tmp[len - 1] == '/') {
		tmp[len - 1] = 0;
	}

	for(char* p = tmp + 1; *p; p++) {
		if(*p == '/') {
			*p = 0;
			MKDIR(tmp, 0755);
			*p = '/';
		}
	}

	MKDIR(tmp, 0755);
}

void rmdir_r(const char* path) {
	char tmp[256];
	strncpy(tmp, path, sizeof(tmp));
	int len = strlen(tmp);

	if(tmp[len - 1] == '/') {
		tmp[len - 1] = 0;
	}

	rmdir(tmp);

	for(char* p = tmp + len - 1; p != tmp; p--) {
		if(*p == '/') {
			*p = 0;
			if(rmdir(tmp)) {
				break;
			}
			*p = '/';
		}
	}
}

char* back_path(const char* path) {
	char path_buff[strlen(path) + 1];
	strcpy(path_buff, path);
	char* path_pt = path_buff;

	int i;
	for(i = 0; (path_pt = strchr(path_pt, '/')) != NULL; i++) {
		path_pt++;
	}

	char* back_str = malloc(sizeof(char) * (i * 3 + 1));
	back_str[0] = 0;
	for(int j = 0; j < i; j++) {
		strcat(back_str, "../");
	}
	return back_str;
}

char* basename_const(const char* path) {
	char path_buff[strlen(path) + 1];
	strcpy(path_buff, path);
	return basename(path_buff);
}

char* dirname_const(const char* path) {
	char path_buff[strlen(path) + 1];
	strcpy(path_buff, path);
	return dirname(path_buff);
}
