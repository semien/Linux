#include<stdio.h>
#include"user_functions.h"

int main(int argc, char* argv[]){
  if (argc < 2){
    printf("mfs: wrong number of arguments\n");
    return 1;
  }
  if (!strcmp(argv[1],"install")){
    mfs_install();
  }
  if((mfs = fopen(MFS_FILENAME, "r+b")) == NULL) {
    printf("Cannot open mfs file\n");
    return 1;
  }

  read_sb();
  if (!check_sb()){
    printf("FS is not installed\n");
  }
  read_group_desc_table();
  if (!strcmp(argv[1],"ls")){
    ls(argc,argv);
  }
  if (!strcmp(argv[1],"cd")){
    cd(argc,argv);
  }
  if (!strcmp(argv[1],"mkdir")){
    makedir(argc, argv);
  }
  if (!strcmp(argv[1],"touch")){
    touch(argc, argv);
  }
  if (!strcmp(argv[1],"put")){
    put(argc, argv);
  }
  if (!strcmp(argv[1],"rm")){
    rm(argc, argv);
  }
  if (!strcmp(argv[1],"cat")){
    cat(argc, argv);
  }
  fclose(mfs);
  return 0;
}
