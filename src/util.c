#include "util.h"
#include <string.h>
#include <sys/stat.h>
#include <libgen.h>
#include <stdlib.h>
#include <unistd.h>

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
			mkdir(tmp, 0755);
			*p = '/';
		}
	}

	mkdir(tmp, 0755);
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
