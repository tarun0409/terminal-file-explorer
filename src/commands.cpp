#include "../headers/default_headers.h"
#include "../headers/syscalls.h"

vector<string> split(string);
char * convert_string_to_char(string);
void change_dir(const char *);
int is_directory(char *);
string get_root_dir();
vector<struct dir_info> get_dir_info();
vector<struct dir_info> get_dir_info(const char *);

struct dir_info
{
	string name;
	string mode;
	string user_name;
	string size;
	string m_time;
};

int is_current_dir_in_delete_path(string dir_name)
{
	char * cwd = getcwd(NULL,100*sizeof(char));
	string curr_dir = cwd;
	if(curr_dir.find(dir_name) != string::npos)
	{
		return 1;
	}
	return 0;
}

string prev_dir(string curr_dir)
{
	string path = "";
	int n = curr_dir.length();
	string curr_word = "";
	for(int i=0; i<n; i++)
	{
		char s = curr_dir[i];
		if(s=='/')
		{
			path+='/';
			path.append(curr_word);
			curr_word = "";
		}
		else
		{
			curr_word+=s;
		}
	}
	return path;
	
}

void remove_directory_contents(char * dir_name)
{
	vector<struct dir_info> dirs = get_dir_info(dir_name);
	int n = dirs.size();
	for(int i=0; i<n; i++)
	{
		string curr_dir = dir_name;
		curr_dir+='/';
		struct dir_info dir = dirs[i];
		string name = dir.name;
		string e_name = curr_dir.append(name);
		char * entry_name = convert_string_to_char(e_name);
		if(!is_directory(entry_name))
		{
			unlink(entry_name);
		}
		else
		{
			if(name.compare(".") && name.compare(".."))
			{
					remove_directory_contents(entry_name);
					rmdir(entry_name);
			}
		}
	}
}

void exec_command(string cmd)
{
	vector<string> cmd_split = split(cmd);
	string op = cmd_split[0];
	if(!op.compare("goto"))
	{
		string dir_name = cmd_split[1];
		string root_dir = get_root_dir();
		dir_name = root_dir.append(dir_name);
		char * d_name = convert_string_to_char(dir_name); 
		change_dir(d_name);
	}
	if(!op.compare("delete_file"))
	{
		string file_name = cmd_split[1];
		string root_dir = get_root_dir();
		file_name = root_dir.append(file_name);
		char * f_name = convert_string_to_char(file_name);
		if(!is_directory(f_name))
		{
			unlink(f_name);
		}
	}
	if(!op.compare("delete_dir"))
	{
		string dir_name = cmd_split[1];
		string root_dir = get_root_dir();
		string temp_root_dir = root_dir;
		dir_name = root_dir.append(dir_name);
		if(is_current_dir_in_delete_path(dir_name))
		{
			change_dir(convert_string_to_char(temp_root_dir));
		}
		char * d_name = convert_string_to_char(dir_name);
		if(is_directory(d_name))
		{
			remove_directory_contents(d_name);
			rmdir(d_name);
		}
	}
}
