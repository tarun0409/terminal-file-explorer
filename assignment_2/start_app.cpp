#include <iostream>

int create_torrent_file(char *);

int main(int argc, char const *argv[]) {

  char file_name[10] = "file1.txt";
  create_torrent_file(file_name);
  return 0;
}
