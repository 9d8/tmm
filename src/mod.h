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
