#include "../headers/default_headers.h"
#include "../headers/syscalls.h"

#define clear() printf("\033[H\033[J")
#define COMMAND_MODE 1
#define NORMAL_MODE 0

char * convert_string_to_char(string);
vector<struct dir_info> get_dir_info();
void exec_command(string);
void change_dir(const char *);
void traverse(char);
string get_root_dir();

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
vector<struct dir_info> directories;
int win_left;
int win_right;
int rows;

void clr_screen()
{
	clear();
}

struct winsize get_window_size()
{
	struct winsize w;
    ioctl(0, TIOCGWINSZ, &w);
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

struct dir_info_max_sizes get_dir_info_max_sizes()
{
	struct dir_info_max_sizes dims;
	dims.name = 0;
	dims.mode = 0;
	dims.user_name = 0;
	dims.size = 0;
	dims.m_time = 0;
	for(int i=win_left; i<=win_right; i++)
	{
		struct dir_info t_d = directories[i];
		(dims.name) = ((t_d.name).length() > dims.name)?(t_d.name).length():(dims.name);
		(dims.mode) = ((t_d.mode).length() > dims.mode)?(t_d.mode).length():(dims.mode);
		(dims.user_name) = ((t_d.user_name).length() > dims.user_name)?(t_d.user_name).length():(dims.user_name);
		(dims.size) = ((t_d.size).length() > dims.size)?(t_d.size).length():(dims.size);
		(dims.m_time) = ((t_d.m_time).length() > dims.m_time)?(t_d.m_time).length():(dims.m_time);
	}
	return dims;
}

void list_directories(int mode)
{
	int n = win_right - win_left + 1;
	if(curr_pos>n)
	{
		if((win_right+1)<n_dir)
		{
			win_left++;
			win_right++;
		}
		curr_pos = n;
	}
	if(curr_pos<1)
	{
		if((win_left-1)>=0)
		{
			win_left--;
			win_right--;
		}
		curr_pos = 1;
	}
	clr_screen();
	struct dir_info_max_sizes dims = get_dir_info_max_sizes();
	for(int i=win_left; i<=win_right; i++)
	{
		struct dir_info t_d = directories[i];
		cout<<left<<setw((dims.mode) + 4)<<(t_d.mode)<<setw((dims.user_name) + 4)<<(t_d.user_name)<<setw((dims.size) + 4)<<(t_d.size)<<setw((dims.m_time) + 4)<<(t_d.m_time)<<setw((dims.name) + 4)<<(t_d.name)<<endl;
	}
	if(mode==NORMAL_MODE)
	{
			move_cursor((n-curr_pos+1),1);
	}
	else
	{
			move_cursor((rows-n-1),0);
	}
}

void change_directory_display(int mode)
{
	change_dir(".");
	directories = get_dir_info();
	int n = directories.size();
	n_dir = n;
	curr_pos = 1;
	struct winsize ws = get_window_size();
	rows = ws.ws_row;
	win_left = 0;
	if(n<rows)
	{
		win_right = (n-1);
	}
	else
	{
		win_right = (rows-3);
	}
	list_directories(mode);
}

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
	change_directory_display(COMMAND_MODE);
	int quit = 0;
	while(!quit)
	{
		char op = getchar();
		if(op=='\033')
		{
			quit = 1;
		}
		else
		{
			string cmd = "";
			string temp_cmd = "";
			cout<<op;
			cmd+=op;
			setCanonicalMode();
			getline(cin,temp_cmd);
			cmd.append(temp_cmd);
			exec_command(cmd);
			change_directory_display(COMMAND_MODE);
			setNonCanonicalMode();
		}
	}
}

int process_key_stroke(int ks)
{
	struct dir_info di;
	char * t;
	string rd;
	char * e_rd;
	switch(ks)
	{
		case 10:
			di = directories[curr_pos-1];
			t = convert_string_to_char(di.name);
			change_dir(t);
			change_directory_display(NORMAL_MODE);
			break;
		case 58:
			command_mode();
			change_directory_display(NORMAL_MODE);
			break;
		case 97:
			traverse('a');
			change_directory_display(NORMAL_MODE);
			break;
		case 98:
			traverse('b');
			change_directory_display(NORMAL_MODE);
			break;	
		case 100:
			traverse('d');
			change_directory_display(NORMAL_MODE);
			break;
		case 104:
			rd = get_root_dir();
			e_rd = convert_string_to_char(rd);
			change_dir(e_rd);
			change_directory_display(NORMAL_MODE);
			break;
		case 113:
			return 1;
		case 115:
			curr_pos++;
			list_directories(NORMAL_MODE);
			break;
		case 119:
			curr_pos--;
			list_directories(NORMAL_MODE);
			break;
	}
	return 0;
}
void normal_mode()
{
	setNonCanonicalMode();
	change_directory_display(NORMAL_MODE);
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
