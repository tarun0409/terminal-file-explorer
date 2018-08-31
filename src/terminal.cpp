#include "../headers/default_headers.h"
#include "../headers/syscalls.h"

#define clr_scr() printf("\033[H\033[J")
#define SEARCH_MODE 2
#define COMMAND_MODE 1
#define NORMAL_MODE 0

char * convert_string_to_char(string);
vector<struct dir_info> get_dir_info();
vector<string> exec_command(string);
void change_dir(const char *);
void traverse(char);
string get_root_dir();
int is_directory(char *);
int process_key_stroke(int,int);

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
vector<int> entry_extra_lines;
int curr_index;
int curr_pos_buff;
int win_left;
int win_right;
int rows;
string cmd_buffer = "";
vector<string> search_results;

void clr_screen()
{
	clr_scr();
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
void set_entry_sizes(int mode)
{
	struct winsize ws = get_window_size();
	int col_size = ws.ws_col;
	int l = 0;
	entry_extra_lines.clear();
	if(mode==SEARCH_MODE)
	{
		for(int i=0; i<n_dir; i++)
		{
			string entry_name = search_results[i];
			int s = entry_name.length();
			int extra_lines = s/col_size;
			entry_extra_lines.push_back(extra_lines);
		}
	}
	else
	{
		for(int i=0; i<n_dir; i++)
		{
			struct dir_info t_d = directories[i];
			int size = 0;
			size = (t_d.name).length() + (t_d.mode).length() + (t_d.user_name).length() + (t_d.size).length() + (t_d.m_time).length() + 16;
			int extra_lines = size/col_size;
			entry_extra_lines.push_back(extra_lines);
		}
	}
}
int get_no_of_lines()
{
	int l = 0;
	for(int i=win_left; i<=win_right; i++)
	{
		l+=(entry_extra_lines[i] + 1);
	}
	return l;
}
void list_directories(int mode)
{
	int no_of_lines = get_no_of_lines();
	if(curr_pos>no_of_lines)
	{
		if((win_right+1)<n_dir)
		{
			win_left++;
			win_right++;
		}
		curr_pos = no_of_lines;
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
	if(mode==SEARCH_MODE)
	{
		for(int i=win_left; i<=win_right; i++)
		{
			cout<<search_results[i]<<endl;
		}
	}
	else
	{
			struct dir_info_max_sizes dims = get_dir_info_max_sizes();
		for(int i=win_left; i<=win_right; i++)
		{
			struct dir_info t_d = directories[i];
			cout<<left<<setw((dims.mode) + 4)<<(t_d.mode)<<setw((dims.user_name) + 4)<<(t_d.user_name)<<setw((dims.size) + 4)<<(t_d.size)<<setw((dims.m_time) + 4)<<(t_d.m_time)<<(t_d.name)<<endl;
		}
	}
	no_of_lines = get_no_of_lines();
	if(mode==NORMAL_MODE || mode == SEARCH_MODE)
	{
			move_cursor((no_of_lines-curr_pos+1),1);
	}
	else
	{
			move_cursor((rows-no_of_lines-1),0);
			cout<<cmd_buffer;
	}
}

void change_directory_display(int mode)
{
	if(mode==SEARCH_MODE)
	{
		n_dir = search_results.size();
		curr_pos = 1;
		curr_index = 0;
	}
	else
	{
		change_dir(".");
		directories = get_dir_info();
		n_dir = directories.size();
		curr_pos = 1;
		curr_index = 0;
	}
	set_entry_sizes(mode);
	struct winsize ws = get_window_size();
	rows = ws.ws_row;
	win_left = 0;
	if(n_dir<rows)
	{
		win_right = (n_dir-1);
	}
	else
	{
		if(mode==NORMAL_MODE)
		{
			win_right = (rows-2);
		}
		else if(mode==COMMAND_MODE)
		{
			win_right = (rows-4);
		}
	}
	list_directories(mode);
	curr_pos_buff = entry_extra_lines[0];
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

int command_mode()
{
	change_directory_display(COMMAND_MODE);
	int quit = 0;
	int search_av = 0;
	while(!quit)
	{
		char op = getchar();
		if(op=='\033')
		{
			quit = 1;
		}
		else if(op==10)
		{
			vector<string> res = exec_command(cmd_buffer);
			if(!res.empty())
			{
				search_results = res;
				change_directory_display(SEARCH_MODE);
				if(!search_results.empty())
				{
					char sop = 'g';
					int q = 0;
					while(!q)
					{
						sop = getchar();
						if(sop == '\033')
						{
							getchar();
							switch(getchar())
							{
								case 'A':
									process_key_stroke(119,SEARCH_MODE);
									break;
								case 'B':
									process_key_stroke(115,SEARCH_MODE);
									break;
								case 'D':
									q = 1;
									quit = 1;
									break;
							}
						}
						else if(sop == 127)
						{
							q = 1;
							quit = 1;
						}
						else if(sop==10)
						{
							process_key_stroke(10,SEARCH_MODE);
							search_av = 1;
							q = 1;
							quit = 1;
						}
					}
					
				}
			}
			else
			{
				cmd_buffer = "";
				change_directory_display(COMMAND_MODE);
			}
		}
		else if(op==127)
		{
			int cl = cmd_buffer.length();
			if(cl>0)
			{
				cmd_buffer = cmd_buffer.substr(0,(cl-1));
				change_directory_display(COMMAND_MODE);
			}
		}
		else
		{
			cmd_buffer+=op;
			change_directory_display(COMMAND_MODE);
		}
	}
	cmd_buffer = "";
	return search_av;
}

int process_key_stroke(int ks,int mode)
{
	struct dir_info di;
	char * t;
	string rd;
	char * e_rd;
	int limit;
	int prev_ci;
	int search_av;
	switch(ks)
	{
		case 10:
			if(mode==NORMAL_MODE)
			{
				di = directories[curr_index];
				t = convert_string_to_char(di.name);
			}
			else if(mode==SEARCH_MODE)
			{
				t = convert_string_to_char(search_results[curr_index]);
			}
			if(is_directory(t))
			{
				change_dir(t);
			}
			else
			{
				if(fork()==0)
				{
					execl("/usr/bin/xdg-open", "xdg-open", t, (char *)0);
					exit(1);
				}
			}
			change_directory_display(NORMAL_MODE);
			break;
		case 58:
			search_av = command_mode();
			if(!search_av)
			{
				change_directory_display(mode);
			}
			break;
		case 97:
			traverse('a');
			change_directory_display(mode);
			break;
		case 98:
			traverse('b');
			change_directory_display(mode);
			break;	
		case 100:
			traverse('d');
			change_directory_display(mode);
			break;
		case 104:
			rd = get_root_dir();
			e_rd = convert_string_to_char(rd);
			change_dir(e_rd);
			change_directory_display(mode);
			break;
		case 113:
			return 1;
		case 115:
			curr_pos++;
			if(curr_index!=(n_dir-1) && curr_pos_buff<=0)
			{
				curr_index++;
				if(curr_index>=n_dir)
				{
					curr_index = (n_dir-1);
				}
				curr_pos_buff = entry_extra_lines[curr_index];
			}
			else if(curr_pos_buff>0)
			{
				curr_pos_buff--;
			}
			list_directories(mode);
			break;
		case 119:
			curr_pos--;
			if((curr_index!=0) && (curr_pos_buff>=entry_extra_lines[curr_index]))
			{
				curr_index--;
				curr_pos_buff = 0;
			}
			else if(curr_pos_buff<entry_extra_lines[curr_index])
			{
				curr_pos_buff++;
			}
			list_directories(mode);
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
					process_key_stroke(119,NORMAL_MODE);
					break;
				case 'B':
					process_key_stroke(115,NORMAL_MODE);
					break;
				case 'C':
					process_key_stroke(100,NORMAL_MODE);
					break;
				case 'D':
					process_key_stroke(97,NORMAL_MODE);
					break;
			}
		}
		else if(op == 127)
		{
			process_key_stroke(98,NORMAL_MODE);
		}
		else
		{
			quit = process_key_stroke(op,NORMAL_MODE);
		}
	}
	setCanonicalMode();
	clr_screen();
}
