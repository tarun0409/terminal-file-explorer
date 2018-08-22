#include "../headers/default_headers.h"
#include "../headers/syscalls.h"
const char * dot = ".";
map<string,struct stat *> list_dir(const char * dir_name)
{
	DIR * currentDir = opendir(dir_name);
	map<string,struct stat *> dir_entries;
	if(currentDir!=NULL)
	{
		struct dirent * dirEntry;
		while((dirEntry = readdir(currentDir))!=NULL)
		{
			char * entryName = dirEntry->d_name;
			struct stat * statbuf = (struct stat *)malloc(sizeof(struct stat));
			lstat(entryName,statbuf);
			dir_entries.insert(make_pair(entryName,statbuf));
			//dir_names.push_back(statbuf);
		}
	}
	return dir_entries;
}

map<string,struct stat *> list_dir()
{
	return list_dir(dot);
}