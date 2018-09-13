#include <unistd.h>
#include <iostream>
#include <sys/types.h>
#include <openssl/sha.h>
#include <string>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#define PIECE_SIZE 524288

using namespace std;

string get_file_name(string);
char * convert_string_to_char(string);


struct torrent_info
{
  
};

int create_torrent_file(char * file_name)
{
  char buffer[PIECE_SIZE];
  int r_fd = open(file_name,O_RDONLY);
  if(r_fd < 0)
  {
    cout<<"\nFile does not exist/No permissions on file\n";
    return r_fd;
  }
  struct stat statbuf;
	lstat(file_name,&statbuf);
  size_t file_size_t = statbuf.st_size;
  string fs = to_string(file_size_t);
  int file_size_len = fs.length();
  char * file_size = convert_string_to_char(fs);
  string fname = file_name;
  int file_name_len = fname.length();
  string tfname = get_file_name(fname);
  tfname = tfname.append(".mtorrent");
  char * torrent_file_name = convert_string_to_char(tfname);
  int w_fd = open(torrent_file_name,(O_WRONLY | O_CREAT | O_TRUNC),(S_IRUSR | S_IWUSR | S_IXUSR));
  if(w_fd < 0)
  {
    cout<<"\nCould not create torrent file\n";
    return w_fd;
  }
  write(w_fd,"127.0.0.1:9090,127.0.0.1:9091\n",30);
  fname=fname+'\n';
  char * temp_buff = convert_string_to_char(fname);
  write(w_fd,temp_buff,(file_name_len+1));
  write(w_fd,file_size,file_size_len);
  write(w_fd,"\n",1);
  SHA_CTX ctx;
  SHA1_Init(&ctx);
  SHA1_Update(&ctx, file_name, file_name_len);
  while( read(r_fd, buffer, PIECE_SIZE) > 0 )
  {
    string temp_buff = buffer;
    int len = temp_buff.length();
    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1((unsigned const char *)buffer, len, hash);
    SHA1_Update(&ctx, buffer, len);
    write(w_fd, hash, SHA_DIGEST_LENGTH);
  }
  unsigned char full_hash[SHA_DIGEST_LENGTH];
  SHA1_Final(full_hash, &ctx);
  write(w_fd,"\n",1);
  write(w_fd, full_hash, SHA_DIGEST_LENGTH);
  close(r_fd);
  close(w_fd);
  return 0;
}
