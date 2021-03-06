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

vector<string> copy_file(string source_file, string dest_dir)
{
	struct stat sbuff;
	vector<string> errors;
	lstat(convert_string_to_char(source_file),&sbuff);
	mode_t mode = sbuff.st_mode;
	string dest_file = get_file_name_from_path(source_file);
	dest_dir+='/';
	dest_file = dest_dir.append(dest_file);
	char * sf = convert_string_to_char(source_file);
	char * df = convert_string_to_char(dest_file);
	if(access(df,F_OK)!=-1)
	{
		string err = dest_file.append(" already exists");
		errors.push_back(err);
		return errors;
	}
	int r_fd = open(sf,O_RDONLY);
	int w_fd = open(df,(O_WRONLY|O_CREAT),mode);
	char out_buff[1024];
	int r_size = (read(r_fd,out_buff,sizeof(out_buff)));
	while(r_size > 0)
	{
		write(w_fd,out_buff,r_size);
		r_size = (read(r_fd,out_buff,sizeof(out_buff)));
	}
	close(r_fd);
	close(w_fd);
	return errors;
}

vector<string> recursive_copy(string source_dir, string dest_dir, vector<string> errors)
{
	struct stat sbuff;
	lstat(convert_string_to_char(source_dir),&sbuff);
	mode_t mode = sbuff.st_mode;
	string dir_name = get_file_name_from_path(source_dir);
	dest_dir += '/';
	dir_name = dest_dir.append(dir_name);
	char * dd = convert_string_to_char(dir_name);
	if(access(dd,F_OK)!=-1)
	{
		string err = dir_name.append(" already exists");
		errors.push_back(err);
		return errors;
	}
	mkdir(convert_string_to_char(dir_name),mode);
	vector<struct dir_info> di = get_dir_info(convert_string_to_char(source_dir));
	for(int i=0; i<di.size(); i++)
	{
		string entry_name = di[i].name;
		if(!entry_name.compare(".") || !entry_name.compare(".."))
		{
			continue;
		}
		string sd = source_dir;
		sd += '/';
		entry_name = sd.append(entry_name);
		if(!is_directory(convert_string_to_char(entry_name)))
		{
			vector<string> errs = copy_file(entry_name,dir_name);
			for(int k = 0; k<errs.size(); k++)
			{
				errors.push_back(errs[k]);
			}
		}
		else
		{
			errors = recursive_copy(entry_name,dir_name,errors);
		}
	}
	return errors;
	
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

vector<string> search_file_in_dir(char * cd, string file_name, vector<string> search_results)
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
			if(!name.compare(file_name))
			{
					search_results.push_back(e_name);
			}
			if(is_directory(entry_name))
			{
				search_results = search_file_in_dir(entry_name,file_name,search_results);
			}
		}
	}
	return search_results;
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

vector<string> exec_command(string cmd)
{
	vector<string> errors;
	vector<string> cmd_split = split(cmd);
	string op = cmd_split[0];
	if(!op.compare("goto"))
	{
		string dir_name = cmd_split[1];
		dir_name = get_absolute_path(dir_name);
		char * d_name = convert_string_to_char(dir_name); 
		if(access(d_name,F_OK)==-1)
		{
			string err = dir_name.append(" does not exist");
			errors.push_back(err);
			return errors;
		}
		change_dir(d_name);
	}
	if(!op.compare("delete_file"))
	{
		string file_name = cmd_split[1];
		file_name = get_absolute_path(file_name);
		char * f_name = convert_string_to_char(file_name);
		if(access(f_name,F_OK)==-1)
		{
			string err = file_name.append(" does not exist");
			errors.push_back(err);
			return errors;
		}
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
		if(access(d_name,F_OK)==-1)
		{
			string err = dir_name.append(" does not exist");
			errors.push_back(err);
			return errors;
		}
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
			if(access(convert_string_to_char(dir_name),F_OK)==-1)
			{
				string err = dir_name.append(" does not exist");
				errors.push_back(err);
				return errors;
			}
			dir_name+='/';
			file_name = dir_name.append(file_name);
		}
		char * df = convert_string_to_char(file_name);
		if(access(df,F_OK)!=-1)
		{	
			string err = file_name.append(" already exists");
			errors.push_back(err);
			return errors;
		}
		FILE * f = fopen(df,"w+");
		fclose(f);
		return errors;
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
			if(access(convert_string_to_char(dir_name),F_OK)==-1)
			{
				string err = dir_name.append(" does not exist");
				errors.push_back(err);
				return errors;
			}
			dir_name+='/';
			fold_name = dir_name.append(fold_name);
		}
		char * dd = convert_string_to_char(fold_name);
		if(access(dd,F_OK)!=-1)
		{	
			string err = fold_name.append(" already exists");
			errors.push_back(err);
			return errors;
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
			if(access(convert_string_to_char(dir_name),F_OK)==-1)
			{	
				string err = dir_name.append(" does not exist");
				errors.push_back(err);
				return errors;
			}
			string entry_name = cmd_split[i];
			entry_name = get_absolute_path(entry_name);
			if(access(convert_string_to_char(entry_name),F_OK)==-1)
			{	
				string err = entry_name.append(" does not exist");
				errors.push_back(err);
				return errors;
			}
			if(!is_directory(convert_string_to_char(entry_name)))
			{
				errors = copy_file(entry_name,dir_name);
			}
			else
			{
				errors = recursive_copy(entry_name,dir_name,errors);
			}
		}
	}
	if(!op.compare("move"))
	{
		int n = cmd_split.size();
		for(int i=1; i<(n-1); i++)
		{	
			string dir_name = cmd_split[n-1];
			dir_name = get_absolute_path(dir_name);
			if(access(convert_string_to_char(dir_name),F_OK)==-1)
			{	
				string err = dir_name.append(" does not exist");
				errors.push_back(err);
				return errors;
			}
			string entry_name = cmd_split[i];
			entry_name = get_absolute_path(entry_name);
			if(access(convert_string_to_char(entry_name),F_OK)==-1)
			{	
				string err = entry_name.append(" does not exist");
				errors.push_back(err);
				return errors;
			}
			if(!is_directory(convert_string_to_char(entry_name)))
			{
				errors = copy_file(entry_name,dir_name);
				if(errors.empty())
				{
					unlink(convert_string_to_char(entry_name));
				}
			}
			else
			{
				errors = recursive_copy(entry_name,dir_name,errors);
				if(errors.empty())
				{
					remove_directory_contents(convert_string_to_char(entry_name));
					rmdir(convert_string_to_char(entry_name));
				}
			}
		}
	}
	if(!op.compare("rename"))
	{
		string root_dir = get_root_dir();
		string old_name = get_absolute_path(cmd_split[1]);
		if(access(convert_string_to_char(old_name),F_OK)==-1)
		{	
			string err = old_name.append(" does not exist");
			errors.push_back(err);
			return errors;
		}
		string new_name = get_absolute_path(cmd_split[2]);
		if(access(convert_string_to_char(new_name),F_OK)!=-1)
		{	
			string err = new_name.append(" already exists");
			errors.push_back(err);
			return errors;
		}
		rename(convert_string_to_char(old_name),convert_string_to_char(new_name));
	}
	if(!op.compare("search"))
	{
		string file_name = cmd_split[1];
		string cwd = getcwd(NULL,255*sizeof(char));
		vector<string> search_results;
		clr_screen();
		search_results = search_file_in_dir(convert_string_to_char(cwd),file_name,search_results);
		return search_results;
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
	return errors;
}
