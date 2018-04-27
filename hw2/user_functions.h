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
    printf("usage:\nls [-i | -l] [directory]\n");
  }
}

void cd(int argc, char* argv[]){
  char path[2] = "/";
  if (argc == 2){
    icd(path);
  }
  if (argc == 3){
    if (icd(argv[2])){
      printf("cd: wrong path\n");
    }
  }
  if (argc > 3){
    printf("usage:\ncd [directory]\n");
    return;
  }
  rewrite_sb();
}

void makedir(int argc, char* argv[]){
  int i = 0;
  if (argc == 2){
    printf("usage:\nmkdir directory ...\n");
    return;
  }
  for (i = 2; i < argc; ++i){
    if (create_file(argv[i], DIR)){
      printf("mkdir: wrong path %s\n", argv[i]);
    }
    rewrite_sb();
  }
}

void touch(int argc, char* argv[]){
  int i = 0;
  if (argc == 2){
    printf("usage:\ntouch file ...\n");
    return;
  }
  for (i = 2; i < argc; ++i){
    if (create_file(argv[i], FIL)){
      printf("touch: wrong path %s\n", argv[i]);
    }
    rewrite_sb();
  }
}

void rm(int argc, char* argv[]){
  int i = 0, error;
  if (argc == 2){
    printf("usage:\nrm [-r] file ...\n");
    return;
  }
  if (!strcmp(argv[2],"-r")){
    for (i = 3; i < argc; ++i){
      if (irm(argv[i], RECURSIVE)){
        printf("rm: wrong path\n");
      }
      rewrite_sb();
    }
  } else {
    for (i = 2; i < argc; ++i){
      error = irm(argv[i], SIMPLE);
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
    }
  }
}

void put(int argc, char* argv[]){
  if (argc != 4){
    printf("usage:\nput external_file internal\n");
    return;
  }
  if (argc == 4){
    if (iput(argv[2], argv[3])){
      printf("put: error\n");
    }
  }
  rewrite_sb();
}

void cat(int argc, char* argv[]){
  u32 i;
  if (argc == 2){
    printf("usage:\ncat file ...\n");
    return;
  }
  for (i = 2; i < argc; ++i){
    if (icat(argv[i])){
      printf("cat: wrong path %s\n", argv[2]);
    } else {
      printf("\n");
    }
  }
}
