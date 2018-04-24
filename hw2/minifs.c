#include<stdio.h>
#include"internal_operations.h"

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

  // if ((mfs=fopen("file", "r+b"))==NULL) {
  //     printf("Cannot open file.\n");
  // }
  //
  // struct mfs_group_desc h ={
  //   .block_bitmap = 4,
  //   .data_table = 34
  // };
  // struct mfs_group_desc m;
  // struct mfs_group_desc m[2];
  //
  // write_block(0, sizeof(struct mfs_group_desc), &h);
  // read_block(0, 2*sizeof(struct mfs_group_desc), &m);
  //
  // printf("%llu %llu\n", h.block_bitmap, h.data_table);
  // printf("%llu %llu\n", m.block_bitmap, m.data_table);
  //
  //
  // mfs_install();
  if((mfs = fopen(MFS_FILENAME, "r+b")) == NULL) {
    printf("Cannot open mfs file\n");
    return 1;
  }
  char path[MFS_FILENAME_LEN] = "kek";
  char path2[MFS_FILENAME_LEN] = "/";
  read_sb();
  read_group_desc_table();
  // ls(path2,1);
  create_file(path, FIL);
  rewrite_sb();
  rewrite_group_desc_table();
  fclose(mfs);

  return 0;
}
