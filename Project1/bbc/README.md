519021911288 韩森宇
---
Problem 4 - Burger Buddies Problem
===
Contains:
- `README.md` - This file
- `bbc.c` - Source code file
- `testscript.txt` - Sample output of BBC

To compile `bbc.c`, use shell command

`
gcc -o BBC bbc.c -lpthread
`

under the current directory. The executable file `BBC` will be located in the same directory.

---
**How to execute `BBC`**

Type

`
./BBC #Cooks #Cashiers #Customers #RackSize
`

in the proper folder of Linux shell. The program will simulate a burger shop as a simple model of multi-thread synchronization algorithm. Program will be terminated after all customers have been served.