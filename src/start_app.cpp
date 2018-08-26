#include "../headers/default_headers.h"
#include "../headers/syscalls.h"

void normal_mode();

char * app_root;

string get_root_dir()
{
	string root_dir = app_root;
	return root_dir;
}

int main()
{
	app_root = getcwd(NULL,100*sizeof(char));
	normal_mode();
	return 0;
}