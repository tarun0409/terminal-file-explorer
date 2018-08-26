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
			string root_dir = get_root_dir();
			dir_name = root_dir.append(dir_name);
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
			string root_dir = get_root_dir();
			dir_name = root_dir.append(dir_name);
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
			string root_dir = get_root_dir();
			dir_name = root_dir.append(dir_name);
			dir_name+='/';
			string file_name = cmd_split[i];
			char out_buff[1024];
			int r_fd = open(convert_string_to_char(file_name),O_RDONLY);
			string new_file_name = file_name;
			new_file_name = dir_name.append(new_file_name);
			mode_t mode = 0777 & ~umask(0);
			int w_fd = open(convert_string_to_char(new_file_name),(O_WRONLY|O_CREAT),mode);
			int r_size = (read(r_fd,out_buff,sizeof(out_buff)));
			while(r_size > 0)
			{
				write(w_fd,out_buff,r_size);
				r_size = (read(r_fd,out_buff,sizeof(out_buff)));
			}
			close(r_fd);
			close(w_fd);
		}
	}
	if(!op.compare("move"))
	{
		int n = cmd_split.size();
		for(int i=1; i<(n-1); i++)
		{
			string dir_name = cmd_split[n-1];
			string root_dir = get_root_dir();
			dir_name = root_dir.append(dir_name);
			dir_name+='/';
			string file_name = cmd_split[i];
			char out_buff[1024];
			int r_fd = open(convert_string_to_char(file_name),O_RDONLY);
			string new_file_name = file_name;
			new_file_name = dir_name.append(new_file_name);
			mode_t mode = 0777 & ~umask(0);
			int w_fd = open(convert_string_to_char(new_file_name),(O_WRONLY|O_CREAT),mode);
			int r_size = (read(r_fd,out_buff,sizeof(out_buff)));
			while(r_size > 0)
			{
				write(w_fd,out_buff,r_size);
				r_size = (read(r_fd,out_buff,sizeof(out_buff)));
			}
			close(r_fd);
			close(w_fd);
			unlink(convert_string_to_char(file_name));
		}
	}
	if(!op.compare("rename"))
	{
		string root_dir = get_root_dir();
		string old_name = cmd_split[1];
		string new_name = cmd_split[2];
		string temp1 = root_dir;
		old_name = temp1.append(old_name);
		string temp2 = root_dir;
		new_name = temp2.append(new_name);
		rename(convert_string_to_char(old_name),convert_string_to_char(new_name));
	}
}
