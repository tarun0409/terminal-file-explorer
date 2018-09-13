#include <string>
#include <vector>
#include <stdlib.h>
using namespace std;

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
	return chars;
}


string get_file_name(string full_name)
{
  vector<string> parts;
  string curr_string = "";
  int i = 0;
  if(full_name.length() > 0)
  {
    char d = full_name[0];
    if(d == '.')
    {
      curr_string+='.';
      i = 1;
    }
  }
  for(; i<full_name.length(); i++)
  {
    char c = full_name[i];
    if(c=='.')
    {
      parts.push_back(curr_string);
      curr_string = "";
    }
    else
    {
      curr_string+=c;
    }
  }
  string file_name = "";
  parts.push_back(curr_string);
  if(parts.size()==1)
  {
    file_name=parts[0];
  }
  else
  {
    for(int j=0; j<(parts.size()-1); j++)
    {
      if(j!=0)
      {
        file_name+='.';
      }
      file_name = file_name.append(parts[j]);
    }
  }
  return file_name;

}
