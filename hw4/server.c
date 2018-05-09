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
  
  umask(0);
  //chdir("/");
  stdin=fopen("/dev/null","r");
  stdout=fopen("/dev/null","w+");
  stderr=fopen("/dev/null","w+");
  return 0;
}


int main(int argc, char* argv[]){
  int listener, port;
  char command[1024];
  char* name;
  char bufferout[1024];
  struct message final_mes;
  
  daemonize();
  
  if (argc == 2 && !strcmp(argv[1],"-i")){
    mfs_install();
  }
  if((mfs = fopen(MFS_FILENAME, "r+b")) == NULL) {
    printf("Cannot open mfs file, maybe fs is not installed\n");
    return 1;
  }
  bzero((void*)command, 1024);
  bzero((void*)bufferout, 1024);
  bzero(&final_mes, sizeof(struct message));
  bzero(&internal_mes, sizeof(struct message));
  struct sockaddr_in serv_addr;
  if ((listener = socket(AF_INET, SOCK_STREAM, 0)) < 0){
    printf("ERROR opening socket\n");
    return 1;
  }
  bzero((char *) &serv_addr, sizeof(serv_addr));
  port = 3425;
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_addr.sin_port = htons(port);
  if (bind(listener, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0){
    printf("ERROR on binding\n");
    return 1;
  }
  setvbuf(stdout, final_mes.data, _IOFBF, MFS_BLOCK_SIZE);
  final_mes.is_final = 1;
  internal_mes.is_final = 0;
  listen(listener,5);
  while(1){
    if ((sock = accept(listener, NULL, NULL)) < 0){
      printf("ERROR accepting\n");
      break;
    }
    recv(sock, (void*)command, 1024, 0);
    name = strtok(command," \n");
    bzero((void*)final_mes.data, MFS_BLOCK_SIZE);
    read_sb();
    if (!check_sb()){
      printf("FS is not installed\n");
      close(sock);
      break;
    }
    read_group_desc_table();
    if (!name) {
      printf("wrong command\n");
    }
    if (!strcmp(name,"ls")){
      ls();
    }
    if (!strcmp(name,"cd")){
      cd();
    }
    if (!strcmp(name,"mkdir")){
      makedir();
    }
    if (!strcmp(name,"touch")){
      touch();
    }
    if (!strcmp(name,"put")){
      put();
    }
    if (!strcmp(name,"rm")){
      rm();
    }
    if (!strcmp(name,"cat")){
      cat();
    }
    if (!strcmp(name,"exit")){
      close(sock);
      break;
    }
    fflush(stdout);
    send(sock, &final_mes, sizeof(struct message), 0);
    close(sock);
  }
  close(listener);
  fclose(mfs);
  return 0;
}
