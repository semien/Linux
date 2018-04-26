#include<stdio.h>
#include"user_functions.h"

u32 func(struct mfs_inode_bitmap* bm){
  u32 i,j,k,answer = 1000;
  u64 bits;
  for (j = 0; j < 2; ++j){
    bits = bm->bits[j];
    for (k = 0; k < 64; ++k){
      printf("%u %llu\n", j*64 + k, bits );
      if (j*64 + k >= 66) return 1;
      if (!(bits & 1)){
        answer = j*64 + k;
        bits |= 1;
        bits <<= k;
        bm->bits[j] |= bits;
        return answer;
      } else {
        bits >>= 1;
      }
    }
  }
  return answer;
}

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
  read_group_desc_table();
  if (!strcmp(argv[1],"ls")){
    ls(argc,argv);
  }
  if (!strcmp(argv[1],"cd")){
    cd(argc,argv);
  }
  if (!strcmp(argv[1],"mkdir")){
    mkdir(argc, argv);
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

  fclose(mfs);
  return 0;
}
