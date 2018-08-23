#include "../headers/default_headers.h"
#include "../headers/syscalls.h"
string convert_unix_time(time_t t)
{
	struct tm *tim = localtime(&t);
	char date[25];
	strftime(date,sizeof(date),"%d-%m-%Y %I:%M%p",tim);
	return date;
}

string get_user_name(uid_t user_id)
{
	struct passwd *pw = getpwuid (user_id);
	char * name = pw->pw_name;
	return name;
}

string get_string_for_mode(mode_t md)
{
	string perm;
	S_ISDIR(md)?perm.push_back('d'):perm.push_back('-');
	(md & S_IRUSR)?perm.push_back('r'):perm.push_back('-');
	(md & S_IWUSR)?perm.push_back('w'):perm.push_back('-');
	(md & S_IXUSR)?perm.push_back('x'):perm.push_back('-');
	(md & S_IRGRP)?perm.push_back('r'):perm.push_back('-');
	(md & S_IWGRP)?perm.push_back('w'):perm.push_back('-');
	(md & S_IXGRP)?perm.push_back('x'):perm.push_back('-');
	(md & S_IROTH)?perm.push_back('r'):perm.push_back('-');
	(md & S_IWOTH)?perm.push_back('w'):perm.push_back('-');
	(md & S_IXOTH)?perm.push_back('x'):perm.push_back('-');
	return perm;
	
}

char * convert_string_to_char(string str)
{
	int n = str.length();
	char * chars = (char *)malloc((n+1)*sizeof(char));
	int i;
	for(i=0; i<n; i++)
	{
		chars[i] = str[i];
	}
	chars[i] = '\0';
}

vector<string> split(string str)
{
	vector<string> ret_str_vec;
	string my_string = "NS";
	for(int i=0; i<str.length(); i++)
	{
		cout<<"\n\nMY_STRING: "<<my_string<<endl;
		if(str[i]==' ')
		{
				ret_str_vec.push_back(my_string);
				my_string = "NS";
		}
		else
		{
			if(!my_string.compare("NS"))
			{
				my_string = "";
			}
			my_string+=str[i];
		}
	}
	if(my_string.compare("NS"))
	{
		ret_str_vec.push_back(my_string);
	}
	return ret_str_vec;
}

string convert_bytes_to_readable(size_t bytes)
{
	double output = (double)bytes;
	double temp_output = output;
	int order = 0;
	while(output>=1024)
	{
		output = output/1024;
		order++;
	}
	char buff[10];
	switch(order)
	{
		case 0:
			sprintf(buff,"%0.2fB",output);
			break;
		case 1:
			sprintf(buff,"%0.2fKB",output);
			break;
		case 3:
			sprintf(buff,"%0.2fMB",output);
			break;
		case 4:
			sprintf(buff,"%0.2fGB",output);
			break;
		case 5:
			sprintf(buff,"%0.2fTB",output);
			break;
		case 6:
			sprintf(buff,"%0.2fPB",output);
			break;
		case 7:
			sprintf(buff,"%0.2fEB",output);
			break;
		case 8:
			sprintf(buff,"%0.2fZB",output);
			break;
		case 9:
			sprintf(buff,"%0.2fYB",output);
			break;
		default:
			sprintf(buff,"%0.2fB",temp_output);
	}
	return buff;
}