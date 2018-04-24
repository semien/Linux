#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<time.h>
#include<math.h>
#include"structures.h"

#define READY 1
#define DIR 'd'
#define FIL '-'

const char MFS_FILENAME[10] = "miniFS";
const u32 MFS_RECS_PER_BLOCK = MFS_BLOCK_SIZE / sizeof(struct mfs_dir_file_rec);
const u32 MFS_BLOCKS_PER_BLOCK = MFS_BLOCK_SIZE / sizeof(u64);
struct mfs_super_block sb;
struct mfs_group_desc_table gdt;
void* buffer;
FILE* mfs = NULL;

int read_block(u64 offset, u32 lenght, void* buffer){
  if (fseeko(mfs, offset, SEEK_SET) != 0){
    printf("seek error, offset=%llu, len=%u\n", offset, lenght);
    return 1;
  };
  if (fread(buffer, lenght, 1, mfs) != 1){
    printf("reading error, offset=%llu, len=%u\n", offset, lenght);
    return 2;
  };
  return 0;
}

int write_block(u64 offset, u32 lenght, void* buffer){
  if (fseeko(mfs, offset, SEEK_SET) != 0){
    printf("seek error, offset=%llu, len=%u\n", offset, lenght);
    return 1;
  };
  if (fwrite(buffer, lenght, 1, mfs) != 1){
    printf("writing error, offset=%llu, len=%u\n", offset, lenght);
    return 2;
  };
  fflush(mfs);
  return 0;
}

int read_sb(){
  return read_block(0, (u64)sizeof(struct mfs_super_block), &sb);
}

int check_sb(){
  if (sb.state == READY)
    return 1;
  else
    return 0;
}

int rewrite_sb(){
  sb.wtime = time(NULL);
  return write_block(0, (u64)sizeof(struct mfs_super_block), &sb);
}

int read_group_desc_table(){
  u64 offset = (u64)sizeof(struct mfs_super_block);
  return read_block(offset, (u64)sizeof(struct mfs_group_desc_table), &gdt);
}

int rewrite_group_desc_table(){
  u64 offset = (u64)sizeof(struct mfs_super_block);
  return write_block(offset, (u64)sizeof(struct mfs_group_desc_table), &gdt);
}

void create_empty_inode(struct mfs_inode* inode, char type){
  inode->size = 0;
  inode->num_blocks = 0;
  memset(inode->blocks, 0, 14*sizeof(u64));
  inode->c_time = time(NULL);
  inode->a_time = inode->c_time;
  inode->d_time = 0;
  inode->type = type;
}

// int read_group_desc(u64 group_index, struct mfs_group_desc* desc){
//   *desc = gdt.desc[group_index]
//   if (memcpy(desc, &(gdt.desc[group_index]), (u64)sizeof(struct mfs_group_desc)) == NULL){
//     printf("Cannot read %llu group descriptor\n", group_index);
//     return 1;
//   }
//   return 0;
// }

void path_splitter(char* path, char* filename){
  size_t len = strlen(path);
  int i = len-1;
  while(path[i] != '/' && i >= 0) --i;
  if (i > 0){
    memcpy(filename, path+i+1, len-i);
    filename[len-i] = '\0';
    path[i] = '\0';
  }
  if (i == 0){
    memcpy(filename, path+i+1, len-i);
    filename[len-i] = '\0';
    path[0] = '/';
    path[1] = '\0';
  }
  if (i == -1){
    memcpy(filename, path, len);
    filename[len] = '\0';
    path[0] = '.';
    path[1] = '/';
    path[2] = '\0';
  }
}

int read_inode(u32 inode_index, struct mfs_inode* inode){
  struct mfs_group_desc desc;
  u64 group_index = inode_index / sb.inodes_per_group;
  u64 inode_group_index = inode_index % sb.inodes_per_group;
  printf("7.5\n");
  desc = gdt.desc[group_index];
  printf("8\n");
  u64 offset = desc.inode_table +
               inode_group_index*((u64)sizeof(struct mfs_inode));
  if (read_block(offset, (u64)sizeof(struct mfs_inode), inode)){
    printf("Cannot read %u inode\n", inode_index);
    return 2;
  };
  return 0;
}

int write_inode(u32 inode_index, struct mfs_inode* inode){
  struct mfs_group_desc desc;
  u64 group_index = inode_index / sb.inodes_per_group;
  u64 inode_group_index = inode_index % sb.inodes_per_group;
  desc = gdt.desc[group_index];
  u64 offset = desc.inode_table +
               inode_group_index*((u64)sizeof(struct mfs_inode));
  if (write_block(offset, (u64)sizeof(struct mfs_inode), inode)){
    printf("Cannot write %u inode\n", inode_index);
    return 2;
  };
  return 0;
}

int get_inode_index_from_dirblock(u64 data_block_offset, char* filename, struct mfs_inode* answer, int deep){
  u32 i = 0;
  int out;
  char data_block[MFS_BLOCK_SIZE];
  if (read_block(data_block_offset, MFS_BLOCK_SIZE, data_block)){
    return 2;
  }
  if (deep == 1){
    struct mfs_dir_file_rec* recs = (struct mfs_dir_file_rec*)data_block;
    for (i = 0; i < MFS_RECS_PER_BLOCK; ++i){
      if (recs[i].filename_len == 0){
        continue;
      } else {
        if (!strcmp(recs[i].filename, filename)){
          return read_inode(recs[i].inode, answer);
        }
      }
    }
    return 1;
  } else {
    u64* blocks = (u64*)data_block;
    for (i = 0; i < MFS_BLOCKS_PER_BLOCK; ++i){
      if (blocks[i] == 0)
        continue;
      out = get_inode_index_from_dirblock(blocks[i], filename, answer, deep-1);
      if (out == 1){
        continue;
      } else {
        return out;
      }
    }
    return 1;
  }
}

int get_inode_from_dir(struct mfs_inode* dir, char* filename, struct mfs_inode* answer){
  size_t filename_len = strlen(filename);
  u32 i = 0;
  if (!strcmp(filename, "..")){
    return read_inode(dir->parent_inode, answer);
  }
  if (!strcmp(filename, ".")){
    answer = dir;
    return 0;
  }
  if (dir->num_blocks <= 12){
    for (i = 0; i < dir->num_blocks; ++i){
      if (!get_inode_index_from_dirblock(dir->blocks[i],filename, answer, 1)){
        return 0;
      }
    }
    return 1;
  }
  if (dir->num_blocks > 12){
    for (i = 0; i < 12; ++i){
      if (!get_inode_index_from_dirblock(dir->blocks[i],filename, answer, 1)){
        return 0;
      }
    }
    if (!get_inode_index_from_dirblock(dir->blocks[12],filename, answer, 2)){
      return 0;
    }
  }
  if (dir->num_blocks > 12 + MFS_BLOCKS_PER_BLOCK){
    return get_inode_index_from_dirblock(dir->blocks[13],filename, answer, 3);
  }
  return 1;
}

int get_inode_index_from_relpath(u32 start_inode, char* path, u32* answer){
  int i,j=0;
  char tmp_buffer[MFS_FILENAME_LEN];
  u32 inode_index;
  struct mfs_inode current_inode;

  printf("3\n");
  if (read_inode(start_inode, &current_inode)){
    return 1;
  };

  printf("4\n");
  while(1){
    if (current_inode.type != DIR){
      path[j-1] = '\0';
      printf("%s not such directory\n", path);
    }

    printf("5\n");
    memset(tmp_buffer, 0, sizeof(tmp_buffer));
    for (i = 0; i < MFS_FILENAME_LEN; ++i,++j){
      tmp_buffer[i] = path[j];
      if (tmp_buffer[i] == '/' || tmp_buffer[i] == '\0')
        break;
    }

    printf("6\n");
    tmp_buffer[i] = '\0';
    get_inode_from_dir(&current_inode, tmp_buffer, &current_inode); // add if
    printf("7\n");
    if (read_inode(inode_index, &current_inode)){
      return 1;
    }
    if (path[j] == '\0')
      break;
    else
      ++j;
  }


  *answer = inode_index;
  return 0;
}

int get_inode_index_from_path(char* path, u32* answer){
  if (path[0] == '/'){
    if (path[1] == '\0'){
      *answer = MFS_ROOT_INODE;
      return 0;
    } else {
      return get_inode_index_from_relpath(MFS_ROOT_INODE, path+1, answer);
    }
  } else {
    return get_inode_index_from_relpath(sb.current_inode, path, answer);
  }
}

int print_file_from_block(u64 data_block_offset, int deep){
  u32 i = 0;
  int out;
  char data_block[MFS_BLOCK_SIZE];
  if (read_block(data_block_offset, MFS_BLOCK_SIZE, data_block)){
    return 1;
  }
  if (deep == 1){
    printf("%s\n", data_block);
  } else {
    u64* blocks = (u64*)data_block;
    for (i = 0; i < MFS_BLOCKS_PER_BLOCK; ++i){
      if (blocks[i] == 0)
        break;
      out = print_file_from_block(blocks[i], deep-1);
      if (out == 0){
        continue;
      } else {
        return out;
      }
    }
  }
  return 0;
}

int print_file_from_inode(u32 inode_index){
  struct mfs_inode inode;
  u32 i = 0;
  if (read_inode(inode_index, &inode) != 0){
    return 1;
  }
  if (inode.num_blocks <= 12){
    for (i = 0; i < inode.num_blocks; ++i){
      if (print_file_from_block(inode.blocks[i], 1) != 0){
        return 1;
      }
    }
    return 0;
  }
  if (inode.num_blocks > 12){
    for (i = 0; i < 12; ++i){
      if (print_file_from_block(inode.blocks[i], 1) != 0){
        return 1;
      }
    }
    if (print_file_from_block(inode.blocks[12], 2) != 0){
      return 1;
    }
  }
  if (inode.num_blocks > 12 + MFS_BLOCKS_PER_BLOCK){
    return print_file_from_block(inode.blocks[13], 3);
  }
  return 0;
}

int occupy_free_inode(u32* answer, u32 preferred_group){
  u32 i,j,k,group_index;
  u64 bits;
  struct mfs_inode_bitmap bm;
  if (sb.num_free_inodes == 0){
    return 1;
  }
  for (i = preferred_group; i < preferred_group + MFS_NUM_GROUPS; ++i){
    group_index = i % MFS_NUM_GROUPS;
    read_block(gdt.desc[group_index].inode_bitmap, sizeof(struct mfs_inode_bitmap), &bm);
    for (j = 0; j < MFS_INODEMAP_LL; ++j){
      bits = bm.bits[j];
      for (k = 0; k < 64; ++k){
        if (j*64 + k >= MFS_INODES_PER_GROUP) return 1;
        if (!(bits & 1)){
          bits |= 1;
          bits <<= k;
          bm.bits[j] |= bits;
          *answer = j*64 + k;
          --sb.num_free_inodes;
          return 0;
        } else {
          bits >>= 1;
        }
      }
    }
  }
  return 1;
}

int occupy_free_block(u64* offset, u32 preferred_group){
  u64 i,j,k,group_index,bits;
  struct mfs_block_bitmap bm;
  if (sb.num_free_blocks == 0){
    return 1;
  }
  for (i = preferred_group; i < preferred_group + MFS_NUM_GROUPS; ++i){
    group_index = i % MFS_NUM_GROUPS;
    read_block(gdt.desc[group_index].block_bitmap, sizeof(struct mfs_block_bitmap), &bm);
    for (j = 0; j < MFS_BLOCKMAP_LL; ++j){
      bits = bm.bits[j];
      for (k = 0; k < 64; ++k){
        if (j*64 + k >= MFS_BLOCKS_PER_GROUP) return 1;
        if (!(bits & 1)){
          *offset = (j*64 + k)*MFS_BLOCK_SIZE + gdt.desc[group_index].data_table;
          bits |= 1;
          bits <<= k;
          bm.bits[j] |= bits;
          --sb.num_free_blocks;
          return 0;
        } else {
          bits >>= 1;
        }
      }
    }
  }
  return 1;
}

int add_dir_rec_to_block(u64 offset, struct mfs_dir_file_rec* rec, int deep){
  u32 i = 0;
  int out;
  char data_block[MFS_BLOCK_SIZE];
  if (offset == 0){
    return 1;
  }
  if (read_block(offset, MFS_BLOCK_SIZE, data_block)){
    return 1;
  }
  if (deep == 1){
    struct mfs_dir_file_rec* recs = (struct mfs_dir_file_rec*)data_block;
    for (i = 0; i < MFS_RECS_PER_BLOCK; ++i){
      if (recs[i].filename_len == 0){
        recs[i] = *rec;
        write_block(offset, MFS_BLOCK_SIZE, data_block);
        return 0;
      }
    }
    return 1;
  } else {
    u64* blocks = (u64*)data_block;
    for (i = 0; i < MFS_BLOCKS_PER_BLOCK; ++i){
      if (!add_dir_rec_to_block(blocks[i], rec, deep-1)) return 0;
    }
    return 1;
  }
}

int add_dir_to_new_block(struct mfs_dir_file_rec* rec, struct mfs_inode* dir){
  u64 offset,offset1,offset2;
  u32 block_index = dir->num_blocks;
  char data_block[MFS_BLOCK_SIZE];
  if (occupy_free_block(&offset, rec->inode / MFS_INODES_PER_GROUP))
    return 1;
  struct mfs_dir_file_rec* recs = (struct mfs_dir_file_rec*)data_block;
  recs[0] = *rec;
  if (write_block(offset, MFS_BLOCK_SIZE, data_block)){
    return 1;
  }
  if (block_index < 12){
    dir->blocks[block_index] = offset;
    return 0;
  }
  /*if (block_index < 12 + MFS_BLOCKS_PER_BLOCK){
    if (dir->blocks[12] == 0){
      occupy_free_block(&offset1, rec->inode / MFS_INODES_PER_GROUP);
    } else {
      offset1 = dir->blocks[12];
    }
    u64* blocks = (u64*)data_block;
    blocks[0] = offset;
    if (write_block(offset1, MFS_BLOCK_SIZE, data_block)){
      return 1;
    }
    dir->blocks[12] = offset1;
    return 0;
  }

  if (dir->blocks[13] == 0){
    occupy_free_block(&offset2, rec->inode / MFS_INODES_PER_GROUP);
  } else {
    offset2 = dir->blocks[13];
  }
  */


  return 1;
}

int add_dir_rec(u32 dir_inode_index, char* filename, u32 inode_index){
  u32 i = 0;
  struct mfs_dir_file_rec rec;
  struct mfs_inode dir;
  if (read_inode(dir_inode_index, &dir)){
    return 1;
  }
  rec.inode = inode_index;
  rec.filename_len = strlen(filename);
  memcpy(filename, rec.filename, MFS_FILENAME_LEN);
  for(i = 0; i < 12; ++i){
    if (!add_dir_rec_to_block(dir.blocks[i], &rec, 1))
      return 0;
  }
  if (!add_dir_rec_to_block(dir.blocks[12], &rec, 2))
    return 0;
  if (!add_dir_rec_to_block(dir.blocks[13], &rec, 3))
    return 0;
  if (!add_dir_to_new_block(&rec, &dir)){
    return write_inode(dir_inode_index, &dir);
  }

  return 1;
}

int delete_dir_rec(struct mfs_inode* dir, char* filename){
  return 0;
}

int add_external_file(char* path_ext, char* dir){
  FILE* ext_file;
  if((ext_file = fopen(path_ext, "r+b")) == NULL) {
    printf("Cannot open mfs file\n");
    return 1;
  }
  return 0;
}

int create_file(char* path, char type){
  char filename[MFS_FILENAME_LEN];
  u32 dir_inode_index, group_index, new_node_index;
  struct mfs_inode dir_inode, new_node;
  printf("1\n");
  path_splitter(path, filename);
printf("2\n");
  if (get_inode_index_from_path(path, &dir_inode_index)){
    return 1;
  }
  printf("%s %s %u\n",path,filename,dir_inode_index);
  if (type == FIL)
    group_index = dir_inode_index / MFS_NUM_INODES;
  if (type == DIR){
    group_index = sb.last_dir_inode_index;
    sb.last_dir_inode_index = (sb.last_dir_inode_index+1)%MFS_NUM_GROUPS;
  }
  if (occupy_free_inode(&new_node_index, group_index)){
    return 1;
  }
  printf("%u\n", new_node_index);
  add_dir_rec(dir_inode_index, filename, new_node_index);
  create_empty_inode(&new_node, type);
  if (write_inode(new_node_index, &new_node)){
    return 1;
  }
  return 0;
}

int ls_from_block(u64 offset, int deep, int mode){
  u32 i = 0;
  int out;
  char data_block[MFS_BLOCK_SIZE];
  if (read_block(offset, MFS_BLOCK_SIZE, data_block)){
    return 1;
  }
  if (deep == 1){
    struct mfs_dir_file_rec* recs = (struct mfs_dir_file_rec*)data_block;
    for (i = 0; i < MFS_RECS_PER_BLOCK; ++i){
      switch (mode) {
        case 1:
           if (recs[i].filename_len != 0) printf("%s ", recs[i].filename);
           break;
        // add new modes
        default:
          return 2;
      }
    }
    return 1;
  } else {
    u64* blocks = (u64*)data_block;
    for (i = 0; i < MFS_BLOCKS_PER_BLOCK; ++i){
      if (blocks[i] == 0)
        break;
      out = ls_from_block(blocks[i], deep-1, mode);
      if (out == 0){
        continue;
      } else {
        return out;
      }
    }
  }
  return 0;
}

void ls(char* dir_path, int mode){
  u32 i = 0;
  struct mfs_inode dir_inode;
  u32 dir_inode_index;
  if (get_inode_index_from_path(dir_path, &dir_inode_index)) return;
  if (read_inode(dir_inode_index, &dir_inode)) return;
  if (dir_inode.num_blocks <= 12){
    for (i = 0; i < dir_inode.num_blocks; ++i){
      if (ls_from_block(dir_inode.blocks[i], 1, mode)){
        printf("ls error\n");
        return;
      }
    }
  }
  if (dir_inode.num_blocks > 12){
    for (i = 0; i < 12; ++i){
      if (ls_from_block(dir_inode.blocks[i], 1, mode)){
        printf("ls error\n");
        return;
      }
    }
    if (ls_from_block(dir_inode.blocks[12], 2, mode)){
      printf("ls error\n");
      return;
    }
  }
  if (dir_inode.num_blocks > 12 + MFS_BLOCKS_PER_BLOCK){
    if (ls_from_block(dir_inode.blocks[13], 3, mode)){
      printf("ls error\n");
      return;
    }
  }
}


int mfs_install(){
  if((mfs = fopen(MFS_FILENAME, "w+b")) == NULL) {
    printf("Cannot open mfs file\n");
    return 1;
  }
  u64 offset = sizeof(struct mfs_super_block) + sizeof(struct mfs_group_desc_table);
  u32 i,j;
  sb.blocks_per_group = MFS_BLOCKS_PER_GROUP;
  sb.inodes_per_group = MFS_INODES_PER_GROUP;
  sb.block_size = MFS_BLOCK_SIZE;
  sb.inode_size = sizeof(struct mfs_inode);
  sb.num_groups = MFS_NUM_GROUPS;
  sb.num_blocks = MFS_NUM_BLOCKS;
  sb.num_inodes = MFS_NUM_INODES;
  sb.num_free_blocks = MFS_NUM_BLOCKS;
  sb.num_free_inodes = MFS_NUM_INODES;
  sb.wtime = 0; // need to change
  sb.current_inode = 0;
  sb.state = READY;
  sb.last_dir_inode_index = 0;

  struct mfs_block_bitmap bbm;
  memset(bbm.bits, 0, MFS_BLOCKMAP_LL*sizeof(u64));
  struct mfs_inode_bitmap ibm;
  memset(ibm.bits, 0, MFS_INODEMAP_LL*sizeof(u64));
  struct mfs_inode in;
  create_empty_inode(&in, FIL);
  char tmp_block[MFS_BLOCK_SIZE];
  memset(tmp_block, 0, MFS_BLOCK_SIZE);

  for (i = 0; i < MFS_NUM_GROUPS; ++i){
    write_block(offset, sizeof(struct mfs_block_bitmap), &bbm);
    gdt.desc[i].block_bitmap = offset;
    offset += sizeof(struct mfs_block_bitmap);
    write_block(offset, sizeof(struct mfs_inode_bitmap), &ibm);
    gdt.desc[i].inode_bitmap = offset;
    offset += sizeof(struct mfs_inode_bitmap);
    gdt.desc[i].inode_table = offset;
    for (j = 0; j < MFS_INODES_PER_GROUP; ++j){
      write_block(offset, sizeof(struct mfs_inode), &in);
      offset += sizeof(struct mfs_inode);
    }
    gdt.desc[i].data_table = offset;
    for (j = 0; j < MFS_INODES_PER_GROUP; ++j){
      write_block(offset, MFS_BLOCK_SIZE, tmp_block);
      offset += MFS_BLOCK_SIZE;
    }
  }

  ibm.bits[0] = 1;
  write_block(gdt.desc[0].inode_bitmap, sizeof(struct mfs_inode_bitmap), &ibm);
  in.type = DIR;
  write_inode(0, &in);
  if (rewrite_sb() || rewrite_group_desc_table()){
    fclose(mfs);
    return 1;
  }
  fclose(mfs);
  return 0;
}
