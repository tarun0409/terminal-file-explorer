#include "../headers/default_headers.h"
#include "../headers/syscalls.h"

vector<string> split(string);
char * convert_string_to_char(string);
void change_dir(const char *);

void exec_command(string cmd)
{
	vector<string> cmd_split = split(cmd);
	string op = cmd_split[0];
	if(!op.compare("goto"))
	{
		string dir_name = cmd_split[1];
		char * d_name = convert_string_to_char(dir_name); 
		change_dir(d_name);
	}
}
