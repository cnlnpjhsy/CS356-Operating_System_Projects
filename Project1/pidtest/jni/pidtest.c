#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(){
    pid_t pid;
    pid = fork();

    if(pid > 0){
        pid_t parent = getpid();
        printf("519021911288 - Parent PID = %d\n", parent);
        wait(NULL);
    }
    else if(pid == 0){
        pid_t child = getpid();
        printf("519021911288 - Child PID = %d\n", child);
        execl("./call356ARM", "call356ARM", "100", NULL);
        exit(1);
    }

    return 0;
}