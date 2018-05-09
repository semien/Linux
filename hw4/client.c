#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

struct message{
  char data[4096];
  int is_final;
};

int main(int argc, char* argv[]){
  int sock, port;
  char command[1024];
  struct sockaddr_in serv_addr;
  struct message mes;
  bzero(&mes, sizeof(struct message));
  bzero(command, 1024);
  bzero((char *) &serv_addr, sizeof(serv_addr));
  port = 3425;
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_addr.sin_port = htons(port);
  while(1){
    printf(">");
    if(!fgets(command, 1024, stdin)){
      printf("command reading error\n");
      continue;
    } else {
      sock = socket(AF_INET, SOCK_STREAM, 0);
      if (connect(sock,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0){
        printf("ERROR connecting\n");
        close(sock);
        break;
      }
      send(sock, command, 1024, 0);
      while(1){
        recv(sock, (void*)&mes, sizeof(struct message), 0);
        printf("%s", mes.data);
        if (mes.is_final)
          break;
      }
      close(sock);
    }
  }
  
  return 0;
  
}
