#include "../headers/default_headers.h"

vector<string> split(string);
void go_to(char *);
char * convert_string_to_char(string);

void exec_command(string cmd)
{
	vector<string> cmd_split = split(cmd);
	string op = cmd_split[0];
	if(!op.compare("goto"))
	{
		string dir_name = cmd_split[1];
		cout<<"\n\ndir_name :"<<dir_name<<endl<<endl;
		go_to(convert_string_to_char(dir_name));
	}
}