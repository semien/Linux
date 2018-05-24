#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <math.h>

struct message{
  char data[4096];
  int is_final;
};

int main(int argc, char* argv[]){
  int sock, port, ans, i, num_blocks;
  unsigned int cur_inode_num = 0, len;
  unsigned int size;
  char fs_command[1024];
  char* util_name;
  char* put_path; // for 'put'
  char external_path[1024];
  struct sockaddr_in serv_addr;
  struct message mes;
  FILE* file;
  bzero(&mes, sizeof(struct message));
  bzero(fs_command, 1024);
  bzero(external_path, 1024);
  bzero((char *) &serv_addr, sizeof(serv_addr));
  port = 3425;
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_addr.sin_port = htons(port);
  while(1){
    printf(">");
    if(!fgets(fs_command, 1024, stdin)){
      printf("command reading error\n");
      continue;
    } else {
      if (fs_command[0]=='\0' || fs_command[0]=='\n') continue;
      if (!strncmp(fs_command, "quit", 4)){
        return 0;
      }
      sock = socket(AF_INET, SOCK_STREAM, 0);
      if (connect(sock,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0){
        printf("ERROR connecting\n");
        close(sock);
        break;
      }
      send(sock, &cur_inode_num, sizeof(unsigned int), 0);
      if (!strncmp(fs_command, "put", 3)){
        util_name = strtok(fs_command, " \n");
        put_path = strtok(NULL, " \n");
        if (put_path){
          strcpy(external_path, put_path);
        } else {
          printf("usage:\nput external_file internal\n");
          continue;
        }
        put_path = strtok(NULL, " \n");
        if (put_path){
          strcat(util_name, " ");
          strcat(util_name, put_path);
        } else {
          printf("usage:\nput external_file internal\n");
          continue;
        }
        if((file = fopen(external_path, "r+b")) == NULL) {
          printf("wrong path\n");
          continue;
        }
        send(sock, &fs_command, 1024, 0);
        fseeko(file, 0, SEEK_END);
        size = (unsigned int)ftell(file) + 1;
        num_blocks = ceil((double)size/ 4095);
        send(sock, &size, sizeof(unsigned int), 0);
        recv(sock, &ans, sizeof(int), 0);
        rewind(file);
        if (ans == 1){
          for (i = 0; i<num_blocks; ++i){
            len = fread(mes.data, 1, 4095, file);
            if (len != 4095) mes.data[len] = '\0';
            mes.is_final = 0;
            if (i == num_blocks - 1) mes.is_final = 1;
            send(sock, &mes, sizeof(struct message), 0);
          }
        }
        fclose(file);
      } else {
        send(sock, fs_command, 1024, 0);
      }
      while(1){
        bzero(mes.data, 4096);
        recv(sock, (void*)&mes, sizeof(struct message), 0);
        printf("%s", mes.data);
        if (mes.is_final)
          break;
      }
      util_name = NULL;
      put_path = NULL;
      recv(sock, &cur_inode_num, sizeof(unsigned int), 0);
      close(sock);
    }
  }
  
  return 0;
  
}
