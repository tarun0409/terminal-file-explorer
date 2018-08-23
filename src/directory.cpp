#include "../headers/default_headers.h"
#include "../headers/syscalls.h"

string convert_unix_time(time_t);
string convert_bytes_to_readable(size_t);
string get_user_name(uid_t);
string get_string_for_mode(mode_t);

const char * d_dot = ".";

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

void display_present_directory()
{
	cout<<endl<<endl<<"You are over here: "<<endl;
	char * cwd = getcwd(NULL,100*sizeof(char));
	cout<<cwd<<endl<<endl;
	map<string,struct stat *> list = list_dir();
	for(map<string,struct stat *>::iterator l_i = list.begin(); l_i!=list.end(); ++l_i)
	{
		string name = l_i->first;
		struct stat * status = l_i->second; 
		cout<<left<<setw(25)<<name<<setw(25)<<convert_bytes_to_readable(status->st_size)<<setw(25)<<get_user_name(status->st_uid)<<setw(25)<<get_string_for_mode(status->st_mode)<<setw(25)<<convert_unix_time((status->st_ctime))<<endl;
	}
}

void display_dir_info(list<char *> trace)
{
	cout<<endl<<"Trace of directories"<<endl;
	for(list<char *>::iterator itr = trace.begin(); itr!=trace.end(); ++itr)
	{
		cout<<*itr<<"-->";
	}
	cout<<endl;
	display_present_directory();
}


int go_to(const char * name)
{
	int x = chdir(name);
	return x;	
}
