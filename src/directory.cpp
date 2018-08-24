#include "../headers/default_headers.h"
#include "../headers/syscalls.h"

string convert_unix_time(time_t);
string convert_bytes_to_readable(size_t);
string get_user_name(uid_t);
string get_string_for_mode(mode_t);
void clr_screen();
char * convert_string_to_char(string);

const char * d_dot = ".";

struct dir_list
{
	char ** dirs;
	int count;
};

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
		}
	}
	return dir_entries;
}

map<string,struct stat *> list_dir()
{
	return list_dir(d_dot);
}

struct dir_list * display_present_directory()
{
	char * cwd = getcwd(NULL,100*sizeof(char));
	map<string,struct stat *> list = list_dir();
	int n = list.size();
	char ** dir_names  = (char **)malloc(n*sizeof(char *));
	int i=0;
	for(map<string,struct stat *>::iterator l_i = list.begin(); l_i!=list.end(); ++l_i)
	{
		string name = l_i->first;
		dir_names[i] = convert_string_to_char(name);
		struct stat * status = l_i->second; 
		cout<<left<<setw(25)<<name<<setw(25)<<convert_bytes_to_readable(status->st_size)<<setw(25)<<get_user_name(status->st_uid)<<setw(25)<<get_string_for_mode(status->st_mode)<<setw(25)<<convert_unix_time((status->st_ctime))<<endl;
		i++;
	}
	struct dir_list * dl = (struct dir_list *)malloc(sizeof(struct dir_list));
	dl->dirs = dir_names;
	dl->count = n;
	return dl;
}



int go_to(const char * name)
{
	int x = chdir(name);
	return x;	
}
