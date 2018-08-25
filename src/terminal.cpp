#include "../headers/default_headers.h"
#include "../headers/syscalls.h"

#define clear() printf("\033[H\033[J")

char * convert_string_to_char(string);
vector<struct dir_info> get_dir_info();
void exec_command(string);
void change_dir(const char *);
void traverse(char);

struct dir_info
{
	string name;
	string mode;
	string user_name;
	string size;
	string m_time;
};

struct dir_info_max_sizes
{
	int name;
	int mode;
	int user_name;
	int size;
	int m_time;
};

struct dir_list
{
	char ** dirs;
	int count;
};

int curr_pos;
int n_dir;
vector<string> dir_names;

void clr_screen()
{
	clear();
}

struct winsize * getSizeOfWindow()
{
	struct winsize * w = (struct winsize *)malloc(sizeof(struct winsize));
    ioctl(0, TIOCGWINSZ, w);
	return w;
}

void move_cursor(int n, int dir)
{
	if(dir)
	{
		printf("\033[%dA",n);
	}
	else
	{
		printf("\033[%dB",n);
	}
}



struct dir_info_max_sizes get_dir_info_max_sizes(vector<struct dir_info> d)
{
	struct dir_info_max_sizes dims;
	dims.name = 0;
	dims.mode = 0;
	dims.user_name = 0;
	dims.size = 0;
	dims.m_time = 0;
	for(int i=0; i<d.size(); i++)
	{
		struct dir_info t_d = d[i];
		(dims.name) = ((t_d.name).length() > dims.name)?(t_d.name).length():(dims.name);
		(dims.mode) = ((t_d.mode).length() > dims.mode)?(t_d.mode).length():(dims.mode);
		(dims.user_name) = ((t_d.user_name).length() > dims.user_name)?(t_d.user_name).length():(dims.user_name);
		(dims.size) = ((t_d.size).length() > dims.size)?(t_d.size).length():(dims.size);
		(dims.m_time) = ((t_d.m_time).length() > dims.m_time)?(t_d.m_time).length():(dims.m_time);
	}
	return dims;
}

void change_directory_display()
{
	change_dir(".");
	vector<struct dir_info> directories = get_dir_info();
	int n = directories.size();
	n_dir = n;
	//char ** dir_names  = (char **)malloc(n*sizeof(char *));
	struct dir_info_max_sizes dims = get_dir_info_max_sizes(directories);
	clr_screen();
	for(int i=0; i<n; i++)
	{
		struct dir_info t_d = directories[i];
		dir_names.push_back(t_d.name);
		//dir_names[i] = convert_string_to_char(t_d.name);
		cout<<left<<setw((dims.mode) + 4)<<(t_d.mode)<<setw((dims.user_name) + 4)<<(t_d.user_name)<<setw((dims.size) + 4)<<(t_d.size)<<setw((dims.m_time) + 4)<<(t_d.m_time)<<setw((dims.name) + 4)<<(t_d.name)<<endl;
	}
	curr_pos = 1;
	move_cursor(n_dir,1);
	//struct dir_list * dl = (struct dir_list *)malloc(sizeof(struct dir_list));
	//dl->dirs = dir_names;
	//dl->count = n;
	//return dl;
}


//struct dir_list * display_present_directory()
//{
//	vector<struct dir_info> directories = get_dir_info();
//	int n = directories.size();
//	char ** dir_names  = (char **)malloc(n*sizeof(char *));
//	struct dir_info_max_sizes dims = get_dir_info_max_sizes(directories);
//	for(int i=0; i<n; i++)
//	{
//		struct dir_info t_d = directories[i];
//		dir_names[i] = convert_string_to_char(t_d.name);
//		cout<<left<<setw((dims.mode) + 4)<<(t_d.mode)<<setw((dims.user_name) + 4)<<(t_d.user_name)<<setw((dims.size) + 4)<<(t_d.size)<<setw((dims.m_time) + 4)<<(t_d.m_time)<<setw((dims.name) + 4)<<(t_d.name)<<endl;
//	}
	//struct dir_list * dl = (struct dir_list *)malloc(sizeof(struct dir_list));
	//dl->dirs = dir_names;
	//dl->count = n;
	//return dl;
//}



void setNonCanonicalMode()
{
	struct termios info;
	tcgetattr(0,&info);
	info.c_lflag = info.c_lflag & ~ECHO;
	info.c_lflag = (info.c_lflag & ~ICANON);
	tcsetattr(0,TCSANOW,&info);
}

void setCanonicalMode()
{
	struct termios info;
	tcgetattr(0,&info);
	info.c_lflag = info.c_lflag | ECHO;
	info.c_lflag = (info.c_lflag | ICANON);
	tcsetattr(0,TCSANOW,&info);
}

void command_mode()
{
	setCanonicalMode();
	string cmd = "";
	while(cmd.compare("e"))
	{
		cout<<"\n\nEnter the command:\n";
		getline(cin,cmd);
		if(cmd.compare("e"))
		{
			exec_command(cmd);
		}
	}
	setNonCanonicalMode();
}

int process_key_stroke(int ks)
{
	switch(ks)
	{
		case 10:
			change_dir(convert_string_to_char(dir_names[curr_pos-1]));
			change_directory_display();
			break;
		case 97:
			traverse('a');
			change_directory_display();
			break;
		case 98:
			traverse('b');
			change_directory_display();
			break;
		case 99:
			command_mode();
			change_directory_display();
			move_cursor(n_dir,0);
			break;
		case 100:
			traverse('d');
			change_directory_display();
			break;
		case 113:
			return 1;
		case 115:
			if(curr_pos<n_dir)
			{
				curr_pos++;
				move_cursor(1,0);
			}
			break;
		case 119:
			if(curr_pos>1)
			{
				curr_pos--;
				move_cursor(1,1);
			}
			break;
	}
	return 0;
}
void normal_mode()
{
	setNonCanonicalMode();
	change_directory_display();
	int quit = 0;
	while(!quit)
	{
		char op = getchar();
		if(op == '\033')
		{
			getchar();
			switch(getchar())
			{
				case 'A':
					process_key_stroke(119);
					break;
				case 'B':
					process_key_stroke(115);
					break;
				case 'C':
					process_key_stroke(100);
					break;
				case 'D':
					process_key_stroke(97);
					break;
			}
		}
		else if(op == 127)
		{
			process_key_stroke(98);
		}
		else
		{
			quit = process_key_stroke(op);
		}
	}
	setCanonicalMode();
	clr_screen();
}
