#include "../headers/default_headers.h"
#include "../headers/syscalls.h"

void display_present_directory();
void traverse(char);
void exec_command(string);

void command_mode()
{
	string cmd = "";
	while(cmd.compare("e"))
	{
		cout<<"\n\nEnter the command:\n";
		cin>>cmd;
		if(cmd.compare("e"))
		{
			exec_command(cmd);
		}
		display_present_directory();
	}
}

void normal_mode()
{
	char inp = 's';
	while(inp!='e')
	{
		display_present_directory();
		cout<<endl<<"Enter any one of following:"<<endl<<"Switch to command mode (c)"<<endl<<"Go to previous directory (b)"<<endl<<"Logical left (l)"<<endl<<"Logical right (r)"<<endl<<"Exit (e)"<<endl;
		cin>>inp;
		if(inp=='c')
		{
			command_mode();
		}
		if(inp!='e')
		{
			traverse(inp);
		}
	}
}


int main()
{
	normal_mode();
	return 0;
}