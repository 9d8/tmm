#ifndef UTIL_H_
#define UTIL_H_

void mkdir_r(const char* path);
void rmdir_r(const char* path);
char* basename_const(const char* path);
char* dirname_const(const char* path);

char* back_path(const char* path);

#endif
