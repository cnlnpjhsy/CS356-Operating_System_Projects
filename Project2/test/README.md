519021911288 韩森宇
---
Basic - Scheduler-changing testfile
===
Contains:
- `README.md` - This file
- `jni/testfile.c` - Source code file
- `jni/Android.mk` - Makefile for `testfile.c`
- `testscript.txt` - Sample output of using the `testfile` to change schedulers

Type shell command

`ndk-build`

in `jni/` folder to compile `testfile.c`. The executable file will be located in `libs/armeabi/` as `testfile`.

---
**How to execute `testfile`**

Type

`
./testfile
`

in the proper folder of Android shell.

The program will ask the pid of the process whose scheduler you wish to change (and it's recommended to run `ps -P` in the shell first to check the process's pid). After getting the pid, the program will show the corresponding process's current scheduler (and its time slice, if there exists). Then you can change its scheduler by following the instruction.