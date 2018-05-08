# Task 2. Mini File System
Clumsy implementation of the ext2-like file system.
### Launch:
```sh
gcc minifs.c -lm -o mfs
./mfs [-i]
```
-i: initialize empty fs with root directory
### Commands:
+ make directory
```sh
mkdir directory ...
```
+ create file (although the original function "touch" works a little differently)
```sh
touch file ...
```
+ change directory
```sh
cd [directory]
```
+ list files
```sh
ls [-l | -r] [directory]
```
+ import external files
```sh
put external_path internal_file_path
```
+ delete files
```sh
rm [-r] file ...
```
+ write files to standard output sequentially 
```sh
cat file ...
```
+ exit
```sh
exit
```
