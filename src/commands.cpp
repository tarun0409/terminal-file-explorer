#include "../headers/default_headers.h"
#include "../headers/syscalls.h"
#define COMMAND_MODE 1
#define NORMAL_MODE 0

vector<string> split(string);
char * convert_string_to_char(string);
void change_dir(const char *);
int is_directory(char *);
string get_root_dir();
vector<struct dir_info> get_dir_info();
vector<struct dir_info> get_dir_info(const char *);
void setNonCanonicalMode();
char * get_current_dir();
void clr_screen();
void change_directory_display(int);
string get_absolute_path(string);
string get_file_name_from_path(string);

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

void print_dump(char * cwd, FILE * fptr)
{
	vector<struct dir_info> dirs = get_dir_info(cwd);
	int n = dirs.size();
	fprintf(fptr,"%s\n\n",cwd);
	for(int i=0; i<n; i++)
	{
		struct dir_info dir = dirs[i];
		string name = dir.name;
		if(name.compare(".") && name.compare(".."))
		{
			fprintf(fptr,"%s\t",convert_string_to_char(name));
		}
	}
	fprintf(fptr,"\n\n\n");
	for(int i=0; i<n; i++)
	{
		struct dir_info dir = dirs[i];
		string name = dir.name;
		if(name.compare(".") && name.compare(".."))
		{
			string cd = cwd;
			cd+='/';
			string e_name = cd.append(name);
			char * entry_name = convert_string_to_char(e_name);
			if(is_directory(entry_name))
			{
				print_dump(entry_name,fptr);
			}
		}
	}
}

void search_file_in_dir(char * cd, string file_name)
{
	vector<struct dir_info> dirs = get_dir_info(cd);
	int n = dirs.size();
	for(int i=0; i<n; i++)
	{
		string curr_dir = cd;
		curr_dir+='/';
		struct dir_info dir = dirs[i];
		string name = dir.name;
		if(name.compare(".") && name.compare(".."))
		{
			string e_name = curr_dir.append(name);
			char * entry_name = convert_string_to_char(e_name);
			if(is_directory(entry_name))
			{
				search_file_in_dir(entry_name,file_name);
			}
			else
			{
				if(!name.compare(file_name))
				{
					cout<<e_name<<endl;
				}
			}
		}
		
	}
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
		dir_name = get_absolute_path(dir_name);
		char * d_name = convert_string_to_char(dir_name); 
		change_dir(d_name);
	}
	if(!op.compare("delete_file"))
	{
		string file_name = cmd_split[1];
		file_name = get_absolute_path(file_name);
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
		dir_name = get_absolute_path(dir_name);
		if(is_current_dir_in_delete_path(dir_name))
		{
			change_dir(convert_string_to_char(root_dir));
		}
		char * d_name = convert_string_to_char(dir_name);
		if(is_directory(d_name))
		{
			remove_directory_contents(d_name);
			rmdir(d_name);
		}
	}
	if(!op.compare("create_file"))
	{
		string file_name = cmd_split[1];
		string dir_name = cmd_split[2];
		if(!dir_name.compare("."))
		{
			string cwd = getcwd(NULL,100*sizeof(char));
			cwd+='/';
			file_name = cwd.append(file_name);
		}
		else
		{
			dir_name = get_absolute_path(dir_name);
			dir_name+='/';
			file_name = dir_name.append(file_name);
		}
		FILE * f = fopen(convert_string_to_char(file_name),"w+");
		fclose(f);
	}
	if(!op.compare("create_dir"))
	{
		string fold_name = cmd_split[1];
		string dir_name = cmd_split[2];
		if(!dir_name.compare("."))
		{
			string cwd = getcwd(NULL,100*sizeof(char));
			cwd+='/';
			fold_name = cwd.append(fold_name);
		}
		else
		{
			dir_name = get_absolute_path(dir_name);
			dir_name+='/';
			fold_name = dir_name.append(fold_name);
		}
		mode_t mode = 0777 & ~umask(0);
		mode = mode | S_IWUSR | S_IXUSR;
		mkdir(convert_string_to_char(fold_name),mode);
	}
	if(!op.compare("copy"))
	{
		int n = cmd_split.size();
		for(int i=1; i<(n-1); i++)
		{	
			string dir_name = cmd_split[n-1];
			dir_name = get_absolute_path(dir_name);
			dir_name+='/';
			string file_name = cmd_split[i];
			file_name = get_absolute_path(file_name);
			string new_file_name = get_file_name_from_path(file_name);
			new_file_name = dir_name.append(new_file_name);
			link(convert_string_to_char(file_name),convert_string_to_char(new_file_name));
		}
		
	}
	if(!op.compare("move"))
	{
		int n = cmd_split.size();
		for(int i=1; i<(n-1); i++)
		{
			string dir_name = cmd_split[n-1];
			dir_name = get_absolute_path(dir_name);
			dir_name+='/';
			string file_name = cmd_split[i];
			file_name = get_absolute_path(file_name);
			string new_file_name = get_file_name_from_path(file_name);
			new_file_name = dir_name.append(new_file_name);
			link(convert_string_to_char(file_name),convert_string_to_char(new_file_name));
			unlink(convert_string_to_char(file_name));
		}
		
	}
	if(!op.compare("rename"))
	{
		string root_dir = get_root_dir();
		string old_name = get_absolute_path(cmd_split[1]);
		string new_name = get_absolute_path(cmd_split[2]);
		rename(convert_string_to_char(old_name),convert_string_to_char(new_name));
	}
	if(!op.compare("search"))
	{
		string file_name = cmd_split[1];
		string cwd = getcwd(NULL,100*sizeof(char));
		clr_screen();
		search_file_in_dir(convert_string_to_char(cwd),file_name);
		setNonCanonicalMode();
		char op = getchar();
		if(op==10)
		{
			change_dir(get_current_dir());
			change_directory_display(NORMAL_MODE);
		}
		else if(op == '\033')
		{
			getchar();
			char c = getchar();
			if(c==100)
			{
				change_dir(get_current_dir());
				change_directory_display(NORMAL_MODE);
			}
		}
	}
	if(!op.compare("snapshot"))
	{
		string dir_name = cmd_split[1];
		dir_name = get_absolute_path(dir_name);
		string file_name = cmd_split[2];
		file_name = get_absolute_path(file_name);
		FILE *fptr = fopen(convert_string_to_char(file_name),"w+");
		print_dump(convert_string_to_char(dir_name), fptr);
		fclose(fptr);
	}
}
