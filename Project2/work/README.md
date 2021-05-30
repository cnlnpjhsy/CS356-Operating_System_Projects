519021911288 韩森宇
---
Basic - Files changed in Linux kernel
===
Contains:
- `arch/arm/configs/goldfish_armv7_defconfig` - New configuration added
- `include/linux/sched.h` - New defination & declaration
- `kernel/sched/`
  - `core.c` - Fixed for the new scheduler WRR
  - `Makefile` - `wrr.o` added
  - `rt.c` - Link the next schedule class to `wrr_sched_class`
  - `sched.h` - New defination & declaration
  - `wrr.c` - Major file. WRR scheduling implementation
- `Makefile` - Kernel cross compile
- `README.md` - This file

---
It's really troublesome to point out all changes here, so I will use Pull Request on Github to show the changes directly by updating the original kernel files first then making pull requests with the changed files. You can check the changes at https://github.com/cnlnpjhsy/CS356-Operating_System_Projects/pull/1/files. (This repo will remain private until June 4th, 2021.)
