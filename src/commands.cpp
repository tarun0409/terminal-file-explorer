#include "../headers/default_headers.h"
#include "../headers/syscalls.h"

vector<string> split(string);
int  go_to(const char *);
char * convert_string_to_char(string);
void display_present_directory();
list<char *> trace;
list<char *>::iterator p_dir;
void display_dir_info(list<char *>);

const char * dot_dot = "..";
const char * dot = ".";

void change_dir(const char * dir_name)
{
	int st = go_to(dir_name);
	if(!st)
	{
		char * cwd = getcwd(NULL,100*sizeof(char));
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
	}
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

void command_mode()
{
	string cmd = "";
	while(cmd.compare("e"))
	{
		cout<<"\n\nEnter the command:\n";
		getline(cin,cmd);
		if(cmd.compare("e"))
		{
			exec_command(cmd);
		}
		display_dir_info(trace);
	}
}

void normal_mode()
{
	string inp = "s";
	change_dir(dot);
	while(inp.compare("e"))
	{
		display_dir_info(trace);
		cout<<endl<<"Enter any one of following:"<<endl<<"Switch to command mode (c)"<<endl<<"Go to previous directory (b)"<<endl<<"Logical left (a)"<<endl<<"Logical right (d)"<<endl<<"Exit (e)"<<endl;
		cout<<endl;
		cout<<endl;
		
		getline(cin,inp);
		if(!inp.compare("c"))
		{
			command_mode();
		}
		else if(inp.compare("e"))
		{
			traverse(inp[0]);
		}
	}
}