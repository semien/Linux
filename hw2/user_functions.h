#include"internal_operations.h"

void ls(int argc, char* argv[]){
  char path[2] = ".";
  if (argc == 2){
    ils(path, 1);
  }
  if (argc == 3){
    if (!strcmp(argv[2],"-l")){
      ils(path, 2);
      return;
    }
    if (!strcmp(argv[2],"-i")){
      ils(path, 3);
      return ;
    }
    ils(argv[2], 1);
  }
  if (argc == 4){
    if (!strcmp(argv[3],"-l")){
      ils(argv[2], 2);
      return;
    }
    if (!strcmp(argv[3],"-i")){
      ils(argv[2], 3);
      return ;
    }
    if (!strcmp(argv[2],"-l")){
      ils(argv[3], 2);
      return;
    }
    if (!strcmp(argv[2],"-i")){
      ils(argv[3], 3);
      return ;
    }
  }
  if (argc > 4){
    printf("ls: wrong number of arguments\n");
  }
}

void cd(int argc, char* argv[]){
  char path[2] = "/";
  if (argc == 2){
    icd(path);
  }
  if (argc == 3){
    if (icd(argv[2])){
      printf("wrong path\n");
    };
  }
  if (argc > 3){
    printf("cd: wrong number of arguments\n");
  }
  rewrite_sb();
}

void mkdir(int argc, char* argv[]){
  int i = 0;
  for (i = 2; i < argc; ++i){
    if (create_file(argv[i], DIR)){
      printf("mkdir: wrong path %s", argv[2]);
      rewrite_sb();
    }
  }
}

void touch(int argc, char* argv[]){
  int i = 0;
  for (i = 2; i < argc; ++i){
    if (create_file(argv[i], FIL)){
      printf("mkdir: wrong path %s", argv[2]);
      rewrite_sb();
    }
  }
}

void rm(int argc, char* argv[]){
}

void put(int argc, char* argv[]){
}
