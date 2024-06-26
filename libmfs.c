#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define UFS_DIRECTORY (0)
#define UFS_REGULAR_FILE (1)

#define UFS_BLOCK_SIZE (4096)

#define DIRECT_PTRS (30)

typedef struct {
    int type;   // MFS_DIRECTORY or MFS_REGULAR
    int size;   // bytes
    unsigned int direct[DIRECT_PTRS];
} inode_t;

typedef struct {
    char name[28];  // up to 28 bytes of name in directory (including \0)
    int  inum;      // inode number of entry (-1 means entry not used)
} dir_ent_t;

// presumed: block 0 is the super block
typedef struct __super {
    int inode_bitmap_addr; // block address (in blocks)
    int inode_bitmap_len;  // in blocks
    int data_bitmap_addr;  // block address (in blocks)
    int data_bitmap_len;   // in blocks
    int inode_region_addr; // block address (in blocks)
    int inode_region_len;  // in blocks
    int data_region_addr;  // block address (in blocks)
    int data_region_len;   // in blocks
    int num_inodes;        // just the number of inodes
    int num_data;          // and data blocks...
} super_t;

typedef struct __bitmap {
    unsigned int bitmap;
} bitmap_t;

typedef struct {
    dir_ent_t entries[128];
} dir_block_t;

typedef struct __ufs {
    int fd; // file descriptor
    super_t s; 
    bitmap_t inode_bp; // inode bitmap
    int inode_bp_sz; // size of inode bitmap
    bitmap_t data_bp; // data bitmap
    int data_bp_sz; // size of data bitmap
    inode_t *inodes; // pointer to array of inodes
    bitmap_t dirty_inode_bp; 
    bitmap_t dirty_data_bp;
} ufs ;


int UFS_INIT(super_t *SuperBlock){ // pread in the superblock, the inodes and the bitmaps (data, inode), store it in memory
    printf("hello1");
    int fd = open("filesystem", O_RDONLY);
    if (fd < 0 ){
        printf("rintf opening the file system");
        return -1;
        exit(1);
    }
    printf("2");
    int reader = pread(fd, &SuperBlock, sizeof(super_t), 0);
    if (reader != sizeof(super_t)){
        printf("rintf reading superblock");
        return -1;
        exit(1);
    }
    return 0;
}

inode_t findInode(int inum, int fd){
        printf("3");
    super_t SuperBlock;
    UFS_INIT(&SuperBlock);
    printf("4");
    inode_t Inode;
    int blk = SuperBlock.inode_region_addr * UFS_BLOCK_SIZE + inum * sizeof(inode_t);
    int reader = pread(fd, &Inode, sizeof(inode_t), blk);
        printf("5");
    if (reader != sizeof(inode_t)){
      printf("rintf");
      exit(1);
    }
    return Inode;
}

int UFS_Lookup(int pinum, char *name){// takes the parent inode number and looks up the entry name
        printf("hida");
    int fd = open("filesystem", O_RDONLY);
    printf("hello");
    if (fd < 0){
        printf("rintf opening file system");
        return -1;
        exit(1);
    }
        printf("lmao");
    inode_t parentInode = findInode(pinum,fd);
    printf("okay");
    for(int i = 0; i < DIRECT_PTRS; i++){
            printf("whatt");
        if(parentInode.direct[i] != -1){
            dir_block_t dir_block;

            int blk = parentInode.direct[i] * UFS_BLOCK_SIZE;
            int reader = pread(fd, &dir_block, sizeof(dir_block_t), blk);
    printf("ohkay");
            if (reader != sizeof(dir_block_t)){
                printf("rintf reading directory entries");
                return -1;
                exit(1);
            }

            for(int j = 0; j < 128; j++){
                printf("haha");
                if (strcmp(dir_block.entries[j].name, name) == 0){
                    return dir_block.entries[j].inum;
                }
            }
        } else {
            return -1;
        }
    }
    close(fd);
    return 0;
}

int readSuperBlock(){
    int fd = open("image_file", O_RDONLY);
    if (fd < 0 ){
        printf("open");
        exit(1);
    }

    super_t SuperBlock;
    int reader = pread(fd, &SuperBlock, sizeof(super_t), 0);
    if (reader != sizeof(super_t)){
        perror("read");
        exit(1);
    }
    printf("inode bitmap address : %d \n" , SuperBlock.inode_bitmap_addr);
    printf("inode bitmap length : %d \n" , SuperBlock.inode_bitmap_len);
    printf("data bitmap address : %d \n" , SuperBlock.data_bitmap_addr);
    printf("data bitmap length : %d \n" , SuperBlock.data_bitmap_len);
    printf("inode region address : %d \n" , SuperBlock.inode_region_addr);
    printf("inode region length : %d \n" , SuperBlock.inode_region_len);
    printf("data region address : %d \n" , SuperBlock.data_region_addr);
    printf("data region length : %d \n" , SuperBlock.data_region_len);
    printf("number of inode blocks : %d \n" , SuperBlock.num_inodes);
    printf("number of data blocks : %d \n" , SuperBlock.num_data);
    return 0;
}

int main(){
    char* name= ".";
    UFS_Lookup(0,name);
}