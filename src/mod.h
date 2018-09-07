#ifndef MOD_H_
#define MOD_H_

typedef struct {
	char* name;
	char** file_paths;
	int total_files;
} mod;

mod* mod_initialize(char* mod_path);
void mod_destroy(mod* m);

#endif
