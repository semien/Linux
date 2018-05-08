#include"internal_operations.h"


//part = strtok(command," ");
//
//// Выделение последующих частей
//while (part != NULL)
//{
//  // Вывод очередной выделенной части
//  printf("%s\n",part);
//  // Выделение очередной части строки
//  part = strtok(NULL," ");
//}

void ls(){
  char path[2] = ".";
  char* arg2;
  char* arg3;
  char* arg4;
  arg2 = strtok(NULL, " \n");
  if (arg2 == NULL){
    ils(path, 1);
    return;
  } else {
    arg3 = strtok(NULL, " \n");
    if (arg3 == NULL){
      if (!strcmp(arg2,"-l")){
        ils(path, 2);
        return;
      }
      if (!strcmp(arg2,"-i")){
        ils(path, 3);
        return ;
      }
      ils(arg2, 1);
      return;
    } else {
      arg4 = strtok(NULL, " \n");
      if (arg4 == NULL){
        if (!strcmp(arg2,"-l")){
          ils(arg3, 2);
          return;
        }
        if (!strcmp(arg2,"-i")){
          ils(arg3, 3);
          return ;
        }
      }
    }
  }
  printf("usage:\nls [-i | -l] [directory]\n");
}

void cd(){
  char path[2] = "/";
  char* arg2;
  char* arg3;
  arg2 = strtok(NULL, " \n");
  if (arg2 == NULL){
    icd(path);
    rewrite_sb();
    return;
  } else {
    arg3 = strtok(NULL, " \n");
    if (arg3 == NULL){
      if (icd(arg2)) printf("cd: wrong path\n");
      rewrite_sb();
      return;
    }
  }
  printf("usage:\ncd [directory]\n");
}

void makedir(){
  char* arg;
  arg = strtok(NULL," \n");
  if (arg == NULL) printf("usage:\nmkdir directory ...\n");
  while (arg){
    if (create_file(arg, DIR)){
      printf("mkdir: wrong path %s\n", arg);
    }
    rewrite_sb();
    arg = strtok(NULL, " \n");
  }
}

void touch(){
  char* arg;
  arg = strtok(NULL," \n");
  if (arg == NULL) printf("usage:\ntouch file ...\n");
  while (arg){
    if (create_file(arg, FIL)){
      printf("mkdir: wrong path %s\n", arg);
    }
    rewrite_sb();
    arg = strtok(NULL, " \n");
  }
}

void rm(){
  char* arg;
  int error;
  arg = strtok(NULL, " \n");
  if (arg == NULL){
    printf("usage:\nrm [-r] file ...\n");
    return;
  }
  if (!strcmp(arg,"-r")){
    arg = strtok(NULL, " \n");
    while (arg){
      if (irm(arg, RECURSIVE)) printf("rm: wrong path\n");
      rewrite_sb();
      arg = strtok(NULL, " \n");
    }
  } else {
    while (arg) {
      error = irm(arg, SIMPLE);
      switch (error) {
        case 1:
          printf("rm: wrong path \n");
          break;
        case 2:
          printf("rm: cannot remove directory, try \"rm -r\"\n");
          break;
        default:
          break;
      }
      rewrite_sb();
      arg = strtok(NULL, " \n");
    }
  }
}

void put(){
  char* arg2;
  char* arg3;
  arg2 = strtok(NULL, " \n");
  if(arg2){
    arg3 = strtok(NULL, " \n");
    if(arg3){
      if (iput(arg2, arg3)) printf("put: error\n");
      rewrite_sb();
      return;
    }
  }
  printf("usage:\nput external_file internal\n");
}

void cat(){
  char* arg;
  arg = strtok(NULL," \n");
  if (arg == NULL) printf("usage:\ncat file ...\n");
  while (arg){
    if (icat(arg)){
      printf("cat: wrong path %s\n", arg);
    } else {
      printf("\n");
    }
    arg = strtok(NULL, " \n");
  }
}
