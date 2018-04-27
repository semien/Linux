# Task 2. Mini File System
Clumsy implementation of the ext2-like file system.
### Launch:
```sh
gcc minifs.c -o mfs
./mfs command
```
### Commands:
+ initialize fs with root directory
```sh
./mfs install
```
+ make directory
```sh
./mfs mkdir directory ...
```
+ create file (although the original function "touch" works a little differently)
```sh
./mfs touch file ...
```
+ change directory
```sh
./mfs cd [directory]
```
+ list files
```sh
./mfs ls [-l | -r] [directory]
```
+ import external files
```sh
./mfs put external_path internal_file_path
```
+ delete files
```sh
./mfs rm [-r] file ...
```
+ write files to standard output sequentially 
```sh
./mfs cat file ...
```
