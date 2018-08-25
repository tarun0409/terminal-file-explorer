#include "../headers/default_headers.h"
#include "../headers/syscalls.h"

struct dir_list
{
	char ** dirs;
	int count;
};
vector<string> split(string);
int  go_to(const char *);
char * convert_string_to_char(string);
struct dir_list * display_present_directory();
list<char *> trace;
list<char *>::iterator p_dir;
void setNonCanonicalMode();
void setCanonicalMode();
void clr_screen();
void move_cursor(char);

const char * dot_dot = "..";
const char * dot = ".";

struct dir_list * change_dir(const char * dir_name)
{
	int st = go_to(dir_name);
	struct dir_list * dl = NULL;
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
		clr_screen();
		//cout<<"Trace of the directory :"<<endl;
		//for(list<char *>::iterator itr = trace.begin(); itr!=trace.end(); ++itr)
		//{
			//cout<<*itr<<"-->";
		//}
		//cout<<endl;
		dl = display_present_directory();
	}
	return dl;
}

struct dir_list * traverse(char inp)
{
	struct dir_list * dl = NULL;
	if(!trace.empty())
	{
		list<char *>::iterator t;
		t = trace.end();
		--t;
		if(inp=='d' && p_dir!=t)
		{
			++p_dir;
			go_to(*p_dir);
			clr_screen();
			dl = display_present_directory();
		}
		else if(inp=='a' && p_dir!=trace.begin())
		{
			--p_dir;
			go_to(*p_dir);
			clr_screen();
			dl = display_present_directory();
		}
		else if(inp=='b')
		{
			if(t!=trace.begin())
			{
				go_to(dot_dot);
				clr_screen();
				dl = display_present_directory();
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
	return dl;
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

struct dir_list * command_mode()
{
	setCanonicalMode();
	string cmd = "";
	struct dir_list * dl = change_dir(dot);
	while(cmd.compare("e"))
	{
		cout<<"\n\nEnter the command:\n";
		getline(cin,cmd);
		if(cmd.compare("e"))
		{
			exec_command(cmd);
		}
		dl = change_dir(dot);
		dl = change_dir(dot);
	}
	setNonCanonicalMode();
	return dl;
}

void normal_mode()
{
	setNonCanonicalMode();
	char op = 'g';
	struct dir_list * dl = change_dir(dot);
	int n = dl->count;
	char ** dir_names = dl->dirs;
	printf("\033[%dA",n);
	int pos = 1;
	while(op!='q')
	{
		op = getchar();
		if(op == '\033')
		{
			getchar();
			switch(getchar()) 
			{
				case 'A':
					if(pos>1)
					{
						pos--;
						move_cursor('w');
					}
					break;
				case 'B':
					if(pos<n)
					{
						pos++;
						move_cursor('s');
					}
					break;
				case 'C':
					dl = traverse('d');
					n = dl->count;
					dir_names = dl->dirs;
					printf("\033[%dA",n);
					pos = 1;
					break;
				case 'D':
					dl = traverse('a');
					n = dl->count;
					dir_names = dl->dirs;
					printf("\033[%dA",n);
					pos = 1;
					break;
			}		
		}
		else if(op == 127)
		{
			dl = traverse('b');
			n = dl->count;
			dir_names = dl->dirs;
			printf("\033[%dA",n);
			pos = 1;
		}
		else if(op == 10)
		{
			char * dir_name = dir_names[pos-1];
			dl = change_dir(dir_name);
			n = dl->count;
			dir_names = dl->dirs;
			printf("\033[%dA",n);
			pos = 1;
		}
		else if(op=='c')
		{
			dl = command_mode();
			n = dl->count;
			dir_names = dl->dirs;
			printf("\033[%dA",n);
		}
	}
	setCanonicalMode();
	clr_screen();
}