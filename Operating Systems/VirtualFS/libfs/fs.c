#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h> // for open

#include "disk.h"
#include "fs.h"

/* TODO: Phase 1 */
/*
 *  Struct implementation of superblock, FAT, and root directory
 */

 typedef struct
 {
    char signature[8];
    uint16_t total_blcks_of_virtual_disk;
    uint16_t root_dir_blck_index;
    uint16_t data_blck_starti;
    uint16_t amt_of_data_blcks;
    uint8_t number_blocks_FAT;
    char padding[4079];
 }__attribute__((packed)) superblock;

typedef struct
{
    char filename[16];
    uint32_t file_size;
    uint16_t first_datablck_index;
    char padding[10];
}__attribute__((packed)) root_entry;

typedef struct
{
	char filename[16];
	int file_size;
	int offset;
	int fd;
	int open;
} file_descriptor;

static superblock sb;
static uint16_t* FAT;
static root_entry* rdir;
static int virtual_disk_open = 0;
int fat_free = 0;
int rdir_free = 0;
static file_descriptor fds[32];
static int first_open = 1;

int fs_mount(const char *diskname)
{
	/* TODO: Phase 1 */

	/*
	 * Open the Virtual Disk
	 * Read the metadata (superblock, fat, root directory)
	 */

	 if(block_disk_open(diskname) < 0)
	 {
	    return -1;
	 }

	 virtual_disk_open = 1;

	 // include other error checking such as checking filesystem signature
	 char* buffer = malloc(sizeof(superblock));
	 if(block_read(0, buffer) < 0)
	 {
	    return -1;
	 }

	 if(strncmp(buffer, "ECS150FS", 8) != 0)
	 {
	    return -1;
	 }

     // Suggest we take get the superblock before getting the count, so block_read beforehand?
	 int block_count = block_disk_count();
	 sb.total_blcks_of_virtual_disk = block_count;
	 sb.number_blocks_FAT = (block_count * 2) / 4096;
	 sb.root_dir_blck_index = sb.number_blocks_FAT + 1;
	 sb.data_blck_starti = sb.root_dir_blck_index + 1;
	 sb.amt_of_data_blcks = block_count - sb.number_blocks_FAT - 1 - 1;

	 FAT = malloc(sizeof(uint16_t) * sb.amt_of_data_blcks);
	 uint16_t new_buffer[2048];
	 for(int i = 1; i < sb.root_dir_blck_index; i++)
	 {
	    if(block_read(i, new_buffer) < 0)
	    {
            return -1;
	    }
	    FAT[i-1] = new_buffer[0];
	 }
	 for(int i = 0; i < sb.amt_of_data_blcks; i++)
	 {
	    if(FAT[i] == 0)
	    {
	        fat_free++;
	    }
	 }

	 rdir = malloc(sizeof(root_entry) * 128);
	 if(block_read(sb.root_dir_blck_index, rdir) < 0)
	 {
	    return -1;
	 }
	 for(int i = 0; i < 128; i++)
	 {
	    if(rdir[i].first_datablck_index != 65535)
	    {
            rdir_free++;
	    }
	 }

	 return 0;
}

int fs_umount(void)
{
	/* TODO: Phase 1 */

	/*
	 * close virtual disk (make sure the virtual disk is up-to-date)
	 */
	 for(int i = 1; i < sb.root_dir_blck_index; i++)
	 {
        if(block_write(i, FAT) < 0)
     	{
            return -1;
     	}
     }

     if(block_write(sb.root_dir_blck_index, rdir) < 0)
     {
        return -1;
     }

     if(block_disk_close() < 0)
     {
        return -1;
     }

     virtual_disk_open = 0;
	 free(FAT);
	 free(rdir);
	 return 0;
}

int fs_info(void)
{
	/* TODO: Phase 1 */

	/*
	 * show information about volume
	 */

	 // error check

	 fprintf(stdout, "FS Info:\n");
	 fprintf(stdout, "total_blk_count=%d\n", sb.total_blcks_of_virtual_disk);
	 fprintf(stdout, "fat_blk_count=%d\n", sb.number_blocks_FAT);
	 fprintf(stdout, "rdir_blk=%d\n", sb.root_dir_blck_index);
	 fprintf(stdout, "data_blk=%d\n", sb.data_blck_starti);
	 fprintf(stdout, "data_blk_count=%d\n", sb.amt_of_data_blcks);
	 fprintf(stdout, "fat_free_ratio=%d/%d\n", fat_free - sb.number_blocks_FAT,sb.amt_of_data_blcks);
	 fprintf(stdout, "rdir_free_ratio=%d/128\n", rdir_free);

	 return 0;
}

int fs_create(const char *filename)
{
    /* TODO: Phase 2 */

	//find index of first entry with first char of filename == "\0"
    if(filename == NULL || strlen(filename) > 16)
    {
        return -1;
    }

    for(int j = 0; j < 128; j++)
    {
        if(strcmp(rdir[j].filename, filename) == 0)
        {
            return -1;
        }
    }

    int i;
    for(int index = 0; index < 128; index++)
    {
    	i = index;

    	if (strncmp(rdir[i].filename, "\0", 1) == 0)
    	{
    		break;
    	}
    }

    //check if out of bounds
    if(i == 128)
    {
        return -1;
    }

    strcpy(rdir[i].filename, filename);
    rdir[i].file_size = 0;
    rdir[i].first_datablck_index = 65535;
	return 0;
}

int fs_delete(const char *filename)
{
	/* TODO: Phase 2 */

	/*
	 * Delete existing file and
	 * free allocated data blocks, if any
	 */
	 if(filename == NULL)
	 {
	    return -1;
	 }

	 for(int j = 0; j < 32; j++)
	 {
	    if(strcmp(fds[j].filename, filename) == 0)
	    {
	        if(fds[j].open == 1)
	        {
	            return -1;
	        }
	    }
	 }

     int i;

     for(int index = 0; index < 128; index++)
     {
     	i = index;

        if (strcmp(rdir[i].filename, filename) == 0)
        {
            break;
        }
     }

     if(i == 129)
     {
        return -1;
     }

     rdir[i].filename[0] = '\0';
	 return 0;
}

int fs_ls(void)
{
	/* TODO: Phase 2 */

	/*
	 * List all existing files
	 */
	 if(!virtual_disk_open)
	 {
	    return -1;
	 }
	 fprintf(stdout, "FS Ls:\n");

	 for(int i = 0; i < 128; i++)
     {
        if(strncmp(rdir[i].filename, "\0", 1) != 0)
     	{
     	    //need to check that output matches reference program
     	    fprintf(stdout, "file: %s, size: %d, data_blk: %d\n", rdir[i].filename, rdir[i].file_size, rdir[i].first_datablck_index);
     	}
     }

	 return 0;
}

int fs_open(const char *filename)
{
	/* TODO: Phase 3 */

	/*
	 * Initialize and return file descriptor
	 * 32 file descriptors max
	 * can open same file multiple times
	 * contain file's offset (initially 0)
	 */

	 if(filename == NULL)
	 {
	    return -1;
	 }

	 int open_count = 0;
	 for(int j = 0; j < 32; j++)
	 {
        if(fds[j].open == 1)
        {
            open_count++;
        }
	 }

     if(open_count == FS_OPEN_MAX_COUNT)
     {
        return -1;
     }

	 int index = 0;

     //if first time calling open, initialize fds, the list of 32 file descriptors
     if(first_open == 1)
     {
     	for(int i = 0; i < 32; i++)
     	{
     		fds[i].open = 0;
     		fds[i].fd = 1;
     	}

     	first_open = 0;
     }

     //find index of first fd with bool open == false
     for(int i = 0; i < 32; i++)
     {
     	if(fds[i].open == 0)
     	{
     		index = i;
     		break;
     	}
     	//not found
     	if(i == 32)
     	{
     		return -1;
     	}
     }

     //check valid fd
     if(fds[index].fd < 0)
     {
     	return -1;
     }

     strcpy(fds[index].filename, filename);
     fds[index].offset = 0;
     fds[index].open = 1;

     return fds[index].fd;
}

int fs_close(int fd)
{
	/* TODO: Phase 3 */

    /*
     * close file descriptor
     */
     int index = 0;

     for(int i = 0; i < 32; i++)
     {
     	if(fds[i].fd == fd)
     	{
     		index = i;
     		break;
     	}
     	//not found
     	if(i == 32)
     	{
     		return -1;
     	}
     }

     fds[index].open = 0;
     fds[index].fd = -1;

     return 0;
}

int fs_stat(int fd)
{
    /* TODO: Phase 3 */

	/*
	 * return file size
	 */
	 int index = 0;
	 int rindex;

     for(int i = 0; i < 32; i++)
     {
        if(fds[i].fd == fd)
        {
            index = i;
            break;
        }
        //not found
        if(i == 32)
        {
            return -1;
        }
     }

     for(int i = 0; index < 129; index++)
     {

        if (strcmp(rdir[i].filename, fds[index].filename) == 0)
        {
            rindex = i;
        }
     }

    return rdir[rindex].file_size;
}

int fs_lseek(int fd, size_t offset)
{
	/* TODO: Phase 3 */

	/*
	 * move file's offset
	 */
	 int index = 0;

     for(int i = 0; i < 32; i++)
     {
        if(fds[i].fd == fd)
        {
            index = i;
            break;
        }
        //not found
        if(i == 31)
        {
            return -1;
        }
     }

     fds[index].offset = offset;

	 return 0;
}


// file_offset implicity set wheneva we do read or write, or explicitly with fs_lseek()
int fs_write(int fd, void *buf, size_t count)
{
	/* TODO: Phase 4 */

	/*
	 * write a certain number of bytes to a file
	 * extend file if necessary
	 */

	 return 0;
}

int fs_read(int fd, void *buf, size_t count)
{
        /* TODO: Phase 4 */

        //first find fd in fds
         int index = 0;

	for(int i = 0; i < 32; i++)
	{
		if(fds[i].fd == fd)
		{
			index = i;
			break;
		}
		//not found
		if(i == 31)
		{
			return -1;
		}
	}

        int block_num = (fds[index].offset/4096 + 6);
	int curr_count = count;
	char bounce[4096];

        if(((fds[index].offset%4096) + curr_count) > 4096)
	{
		//memcpy(bounce, block[block_num], 4096);
		strcat(buf, bounce);
		fds[index].offset += (4096 - (fds[index].offset%4096));
		curr_count -= (4096 - (fds[index].offset%4096));
        	block_num++;
	}


	while((curr_count) > 4096)
	{
		//memcpy(bounce, block[block_num], 4096);
		strcat(buf, bounce);
	 	fds[index].offset += 4096;
	 	curr_count -= 4096;
	 	block_num++;
	}

	//memcpy(bounce, block[block_num], 4096);
	strncat(buf, bounce, curr_count);
	fds[index].offset += curr_count;


        /*
         * read a certain number of bytes to a file
         */

         return 0;
}

