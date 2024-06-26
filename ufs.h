#ifndef __ufs_h__
#define __ufs_h__

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

#endif // __ufs_h__