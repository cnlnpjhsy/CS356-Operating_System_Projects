519021911288 韩森宇
---
Problem 2 - Calling system call `ptree`
===
Contains:
- `README.md` - This file
- `jni/call356.c` - Source code file
- `jni/Android.mk` - Makefile for `call356.c`
- `testscript.txt` - Sample output of calling `ptree` in Android shell

Type shell command

`ndk-build`

in `jni/` folder to compile `call356.c`. The executable file will be located in `libs/armeabi/` as `call356ARM`.

---
**How to execute `call356ARM`**

Type

`
./call356ARM #MAX_PROCESS_NUMBER
`

in the proper folder of Android shell. The program will return a process tree, which contains MAX_PROCESS_NUMBER processes at most. 

