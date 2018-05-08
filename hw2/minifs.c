#include<stdio.h>
#include"user_functions.h"

int main(int argc, char* argv[]){
  char command[1024];
  char* name;
  if (argc > 2){
    printf("usage:\n./mfs [-i]");
    return 1;
  }
  if (argc == 2 && !strcmp(argv[1],"-i")){
      mfs_install();
  }
  if((mfs = fopen(MFS_FILENAME, "r+b")) == NULL) {
    printf("Cannot open mfs file, maybe fs is not installed\n");
    return 1;
  }
  while(1){
    printf(">");
    if(!fgets(command, 1024, stdin)){
      printf("command reading error\n");
      break;
    }
    name = strtok(command," \n");
    read_sb();
    if (!check_sb()){
      printf("FS is not installed\n");
    }
    read_group_desc_table();
    if (!name) break;
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
      break;
    }
  }
  fclose(mfs);
  return 0;
}
