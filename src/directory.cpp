#include "../headers/default_headers.h"
#include "../headers/syscalls.h"

string convert_unix_time(time_t);
string convert_bytes_to_readable(size_t);
string get_user_name(uid_t);
string get_string_for_mode(mode_t);
char * convert_string_to_char(string);

list<char *> trace;
list<char *>::iterator p_dir;

const char * dot = ".";
const char * dot_dot = "..";

struct dir_info
{
	string name;
	string mode;
	string user_name;
	string size;
	string m_time;
};


int is_directory(char * d)
{
	if(d==NULL)
	{
		return 0;
	}
	struct stat statbuf;
	lstat(d,&statbuf);
	if(S_ISDIR(statbuf.st_mode))
	{
		return 1;
	}
	return 0;
}

char * get_current_dir()
{
	if(!trace.empty())
	{
		return *p_dir;
	}
	return NULL;
}

vector<struct dir_info> get_dir_info(const char * dir_name)
{
	vector<struct dir_info> directories;
	DIR * currentDir = opendir(dir_name);
	if(currentDir!=NULL)
	{
		struct dirent * dirEntry;
		set<string> sorted_dir_names;
		while((dirEntry = readdir(currentDir))!=NULL)
		{
			string entryName = dirEntry->d_name;
			sorted_dir_names.insert(entryName);
		}
		for(set<string>::iterator itr = sorted_dir_names.begin(); itr!=sorted_dir_names.end(); ++itr)
		{
			char * entryName = convert_string_to_char(*itr);
			struct stat statbuf;
			lstat(entryName,&statbuf);
			struct dir_info d;
			d.name = entryName;
			d.mode = get_string_for_mode(statbuf.st_mode);
			d.user_name = get_user_name(statbuf.st_uid);
			d.size = convert_bytes_to_readable(statbuf.st_size);
			d.m_time = convert_unix_time(statbuf.st_ctime);
			directories.push_back(d);
		}
	}
	return directories;
}

vector<struct dir_info> get_dir_info()
{
	return get_dir_info(dot);
}

int go_to(const char * name)
{
	if(name==NULL)
	{
		return 1;
	}
	int x = chdir(name);
	return x;	
}

void traverse(char inp)
{
	if(!trace.empty())
	{
		list<char *>::iterator t;
		t = trace.end();
		--t;
		if(inp=='d' && p_dir!=t)
		{
			++p_dir;
			go_to(*p_dir);
		}
		else if(inp=='a' && p_dir!=trace.begin())
		{
			--p_dir;
			go_to(*p_dir);
		}
		else if(inp=='b')
		{
			if(t!=trace.begin())
			{
				go_to(dot_dot);
				char * cwd = getcwd(NULL,100*sizeof(char));
				t = p_dir;
				--t;
				string dir1 = *t;
				string dir2 = cwd;
				if(dir1.compare(dir2))
				{
					trace.push_back(cwd);
				}
				else
				{
					--p_dir;
				}
			}
		}
		
	}
}


void change_dir(const char * dir_name)
{
	int st = go_to(dir_name);
	if(!st)
	{
		char * cwd = getcwd(NULL,255*sizeof(char));
		if(trace.empty())
		{
			trace.push_back(cwd);
			p_dir = trace.begin();
		}
		else
		{
			string c1 = cwd;
			string c2 = *p_dir;
			if(c1.compare(c2))
			{
				list<char *>::iterator t = p_dir;
				++t;
				if(t!=trace.end())
				{
					c2 = *t;
					if(!c1.compare(c2))
					{
						++p_dir;
					}
					else
					{
							list<char *> temp_list;
							temp_list.splice(temp_list.begin(),trace,t,trace.end());
							trace.push_back(cwd);
							++p_dir;
					}
				}
				else
				{
					trace.push_back(cwd);
					++p_dir;
				}
			}
		}
		cout<<endl;
	}
}