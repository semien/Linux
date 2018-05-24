#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <string.h>
#include <signal.h>
#include "user_functions.h"

#define COMMAND_SIZE 1024

int daemonize()
{
  pid_t pid;
  pid = fork();
  if (pid < 0)
  {
    exit(EXIT_FAILURE);
  }
  if (pid > 0) {
    exit(EXIT_SUCCESS);
  }
  if( setsid() < 0 ) {
    exit(EXIT_FAILURE);
  }
  signal(SIGCHLD,SIG_IGN);
  signal(SIGHUP,SIG_IGN);
  
  pid = fork();
  if (pid < 0) {
    exit(EXIT_FAILURE);
  }
  if( pid > 0 ) {
    exit(EXIT_SUCCESS);
  }
  close(0);
  close(1);
  close(2);
  umask(0);
//  chdir("/");
  stdin=fopen("/dev/null","r");
  //stdout=fopen("/dev/null","w+");
  stderr=fopen("/dev/null","w+");
  return 0;
}


int main(int argc, char* argv[]){
  int listener, port;
  char fs_command[COMMAND_SIZE];
  char* util_name;
  struct message final_mes;
  
  daemonize();
  
  if (argc == 2 && !strcmp(argv[1],"-i")){
    mfs_install();
  }
  bzero((void*)fs_command, COMMAND_SIZE);
  bzero(&final_mes, sizeof(struct message));
  bzero(&internal_mes, sizeof(struct message));
  struct sockaddr_in serv_addr;
  if ((listener = socket(AF_INET, SOCK_STREAM, 0)) < 0){
    printf("ERROR opening socket\n");
    fclose(stdin);
    fclose(stderr);
    return 1;
  }
  bzero((char *) &serv_addr, sizeof(serv_addr));
  port = 3425;
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_addr.sin_port = htons(port);
  if (bind(listener, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0){
    printf("ERROR on binding\n");
    fclose(stdin);
    fclose(stderr);
    return 1;
  }
  setvbuf(stdout, final_mes.data, _IOFBF, MFS_BLOCK_SIZE);
  final_mes.is_final = 1;
  listen(listener,5);
  if((mfs = fopen(MFS_FILENAME, "r+b")) == NULL) {
    printf("Cannot open mfs file, maybe fs is not installed\n");
    close(listener);
    return 1;
  }
  while(1){
    if ((sock = accept(listener, NULL, NULL)) < 0){
      printf("ERROR accepting\n");
      break;
    }
    read_sb();
    internal_mes.is_final = 0;
    recv(sock, &sb.current_inode, sizeof(u32), 0);
    recv(sock, (void*)fs_command, COMMAND_SIZE, 0);
    util_name = strtok(fs_command," \n");
    bzero((void*)final_mes.data, MFS_BLOCK_SIZE);
    if (!check_sb()){
      printf("FS is not installed\n");
      close(sock);
      break;
    }
    read_group_desc_table();
    if (!util_name) {
      printf("wrong command\n");
    }
    if (!strcmp(util_name,"ls")){
      ls();
    }
    if (!strcmp(util_name,"cd")){
      cd();
    }
    if (!strcmp(util_name,"mkdir")){
      makedir();
    }
    if (!strcmp(util_name,"touch")){
      touch();
    }
    if (!strcmp(util_name,"put")){
      put();
    }
    if (!strcmp(util_name,"rm")){
      rm();
    }
    if (!strcmp(util_name,"cat")){
      cat();
    }
    if (!strcmp(util_name,"exit")){
      close(sock);
      util_name = NULL;
      break;
    }
    fflush(stdout);
    util_name = NULL;
    send(sock, &final_mes, sizeof(struct message), 0);
    send(sock, &sb.current_inode, sizeof(u32), 0);
    close(sock);
  }
  fclose(stdin);
  fclose(stderr);
  close(listener);
  fclose(mfs);
  return 0;
}
