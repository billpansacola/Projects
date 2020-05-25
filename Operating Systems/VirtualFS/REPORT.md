# **PROJECT 4 REPORT**
*written by Bill Pansacola and Billy Guan*

---

#### **Phase 1 Development**
- When determining what data structures to store/represent `superblock, FAT,
and the root directory`, we identified what will enable us to efficiently grab
info from it to use. Thus we decided to represent `superblock` in a struct, and
`FAT and root directory` as arrays.
- We used the following typing: `uint8_t, uint16_t, and uint32_t` to hold the
exact sizes of each block's metadata. To hold the filenames and signatures for
certain blocks, we utilized a `char` array and specifying the exact size in
bytes, since a char represents 1 byte.
- We also made an entry struct called `root_entry` to accurately store
information for the root directory, thus making `root directory` an array of
root_entry structs (which will help us in phase 2).
- We made global variables for `superblock, FAT, and root directory` for us to
later utilize in the other `fs` functions and easier access.
- For fs_mount, to read in the various blocks in the file systems, we made
several buffers corresponding to the sizes of
`superblock, FAT, and root directory`, in order to have enough space. We also
used `block_disk_count()` and algebra to calculate all the information for the
superblock, and for `fs_info()`.
- For fs_unmount, before closing the virtual disk, we used wrote back to the
disk using `block_write()`.

### **Phase 2 Development**
- Since we made the `root directory` an array of `root_entry` structs, for all
phase 2 functions: `fs_create(), fs_delete(), and fs_ls()`, it was simple when
finding available space or a certain filename, since it was obtainable through
iterating through the array and checking the member variable of that
`root_entry`.

### **Phase 3 Development**
- In this phase, we implemented a `file_descriptor` struct to imitate an actual
file descriptor. This structs holds information, a file descriptor would have,
such as: its filename, file_size, offset, if it has been open or not, etc.
- Similarly to what we did for `root directory`, we made a global array of
file descriptor structs to easily iterate through and access information when
implementing the phase 3 functions.
- The `int open` member of the file_descriptor struct enabled us to make changes
to a certain file descriptor when using `fs_open()` and `fs_close()`.
- To find a certain file and its descriptor, we used a `for` loop to iterate
through our array. This process is visible in all of the Phase 3 functions.
- Since `fs_stat()` returns the size of a file, and our `file_descriptor` struct
does not hold information but our size, but instead in its `root entry`, therefore we found the file in our root directory in order to access its size.

### **Phase 4 Development**
