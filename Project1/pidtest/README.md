519021911288 韩森宇
---
Problem 3 - Child PID & Parent PID
===
Contains:
- `README.md` - This file
- `jni/pidtest.c` - Source code file
- `jni/Android.mk` - Makefile for `pidtest.c`
- `testscript.txt` - Sample output of executing `pidtest` in Android shell

Type shell command

`ndk-build`

in `jni/` folder to compile `pidtest.c`. The executable file will be located in `libs/armeabi/` as `pidtest`.

---
**How to execute `pidtest`**

Type

`
./pidtest`
`

in the proper folder of Android shell, in which `call356ARM` is also included. The program will then execute `./call356ARM 100`, and as the parent process of `call356ARM`, it will be shown in ptree that `pidtest` has a smaller PID than that of `call356ARM`.