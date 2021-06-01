519021911288 韩森宇
---
Bonus - Scheduler performance benchmark
===
Contains:
- `README.md` - This file
- `jni/bench.c` - Source code file
- `jni/Android.mk` - Makefile for `bench.c`
- `testscript.txt` - Sample output of running `benchmark` in Android shell

Type shell command

`ndk-build`

in `jni/` folder to compile `bench.c`. The executable file will be located in `libs/armeabi/` as `benchmark`.

---
**How to execute `benchmark`**

Type

`
./benchmark #MATRIX_SIZE #CHILD_PROCESS_NUMBER
`

in the proper folder of Android shell.

For each scheduling policy (`SCHED_NORMAL`, `SCHED_FIFO`, `SCHED_RR`, and `SCHED_WRR`), the program will fork CHILD_PROCESS_NUMBER child processes to calculate the matrix multiplication. Each child process will calculate MATRIX_SIZE * MATRIX_SIZE matrix multiplication independently under the given scheduling policy. A timer records how long each scheduling policy takes, from forking children to finishing all children's calculating. To make the benchmark more generally and visually, **Mflops/s** is used here to judge the scheduler's efficiency of using the CPU. Both time and Mflops/s will be printed on the shell for comparing the performance.