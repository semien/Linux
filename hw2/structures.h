#include<time.h>
#include<sys/types.h>

typedef unsigned long long u64;
typedef unsigned int u32;

#define MFS_FILENAME_LEN 255
#define MFS_BLOCK_SIZE 4096
#define MFS_BLOCKS_PER_GROUP 16
#define MFS_INODES_PER_GROUP 8 //
#define MFS_NUM_GROUPS 8
#define MFS_NUM_BLOCKS (MFS_NUM_GROUPS*MFS_BLOCKS_PER_GROUP)
#define MFS_NUM_INODES (MFS_NUM_GROUPS*MFS_INODES_PER_GROUP)
#define MFS_ROOT_INODE 0
#define MFS_INODEMAP_LL MFS_INODES_PER_GROUP / 64 + 1
#define MFS_BLOCKMAP_LL MFS_BLOCKS_PER_GROUP / 64 + 1

struct mfs_super_block {
  u32 num_blocks; // blocks in fs
  u32 num_inodes; // inodes in fs
  u32 num_free_blocks; // free blocks in fs
  u32 num_free_inodes; // free inodes in fs
  u32 num_groups;
  u32 blocks_per_group;
  u32 inodes_per_group; // inodes in group
  u32 block_size;
  u32 inode_size;
  u32 last_dir_group_index; // for uniform distribution of dirs between groups
  time_t wtime; // last writing
  u32 current_inode;
  int state; // READY - 1 or others
};

// addresses for random access
struct mfs_group_desc {
  u64 block_bitmap;
  u64 inode_bitmap;
  u64 inode_table;
  u64 data_table;
};

struct mfs_group_desc_table{
  struct mfs_group_desc desc[MFS_NUM_GROUPS];
};

struct mfs_inode {
  u32 size; // in bytes
  //time_t a_time; // access time
  //time_t m_time; // modification time
  time_t c_time; // creating time
  char type; // only two types
  u32 num_blocks; // number of blocks occupied by this file
  u32 parent_inode; // number of parent directory inode
  u64 blocks[14]; // addresses of data blocks
};

struct mfs_dir_file_rec { // structure that describe files in directory
  u32 inode;
  u32 filename_len;
  char filename[MFS_FILENAME_LEN];
};

struct mfs_inode_bitmap {
  u64 bits[MFS_INODEMAP_LL];
};

struct mfs_block_bitmap {
  u64 bits[MFS_BLOCKMAP_LL];
};
