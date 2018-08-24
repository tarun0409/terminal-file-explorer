#include "../headers/default_headers.h"
#include "../headers/syscalls.h"

#define clear() printf("\033[H\033[J")

void clr_screen()
{
	clear();
}


void move_cursor(char p)
{
	if(p=='w')
	{
		printf("\033[1A");
	}
	if(p=='s')
	{
		printf("\033[1B");
	}
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

