#include "../headers/default_headers.h"
#include "../headers/syscalls.h"
map<string,struct stat *> list_dir();
string convert_unix_time(time_t);
string convert_bytes_to_readable(size_t);
string get_user_name(uid_t);
string get_string_for_mode(mode_t);
int main()
{
	map<string,struct stat *> list = list_dir();
	for(map<string,struct stat *>::iterator l_i = list.begin(); l_i!=list.end(); ++l_i)
	{
		string name = l_i->first;
		struct stat * status = l_i->second; 
		cout<<left<<setw(25)<<name<<setw(25)<<convert_bytes_to_readable(status->st_size)<<setw(25)<<get_user_name(status->st_uid)<<setw(25)<<get_string_for_mode(status->st_mode)<<setw(25)<<convert_unix_time((status->st_ctime))<<endl;
	}
	return 0;
}