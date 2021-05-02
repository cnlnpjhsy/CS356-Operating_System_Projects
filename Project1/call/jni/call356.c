#include <stdlib.h>

struct prinfo
{
    pid_t parent_pid;       /* parent pid */
    pid_t pid;              /* process id */
    pid_t first_child_pid;  /* pif of youngest child */
    pid_t next_sibling_pid; /* pid of older sbling */
    long state;             /* current state of process */
    long uid;               /* user id of process owner */
    char comm[64];          /* name of progres executed */
};

typedef struct level{
    int pid;
    int sibling;
};

void printTab(int i){
    int j;
    for (j = 0; j < i; j++)
        printf("\t");
}
void printInfo(struct prinfo p){
    printf("%s,%d,%ld,%d,%d,%d,%d\n",
           p.comm, p.pid, p.state, p.parent_pid,
           p.first_child_pid, p.next_sibling_pid,
           p.uid);
}

void call(int *nr){
    struct prinfo *buffer = malloc(*nr * sizeof(struct prinfo));
    printf("%d processes assigned in calling.\n", *nr);
    syscall(356, buffer, nr);
    printf("%d processes returned in calling.\n", *nr);

    struct level *allLevel = malloc(*nr * sizeof(struct level));
    /* Simulate a stack */
    int *level_last_process = malloc(*nr * sizeof(int));
    /* Print the ptree */
    int tabs = 0;
    int i;
    for (i = 0; i < *nr; i++)
    {
        (allLevel + i)->pid = (buffer + i)->pid;
        (allLevel + i)->sibling = (buffer + i)->next_sibling_pid;

        if(level_last_process + tabs == NULL){
            *(level_last_process + tabs) = i;
        }
        /* Top of the stack */
        struct level *top = allLevel + *(level_last_process + tabs);
        while ((buffer + i)->parent_pid != top->pid &&
               (buffer + i)->pid != top->sibling){
            tabs--;
            top = allLevel + *(level_last_process + tabs);
        }
        if((buffer + i)->pid==top->sibling){
            printTab(tabs);
            printInfo(*(buffer + i));
            *(level_last_process + tabs) = i;
        }
        else if((buffer + i)->parent_pid==top->pid){
            printTab(++tabs);
            printInfo(*(buffer + i));
            *(level_last_process + tabs) = i;
        }
    }
    free(level_last_process);
    free(allLevel);
    free(buffer);

    return;
}

int main(int argc, char *argv[]){
    if(argc!=2){
        printf("Error: argument number incorrect\n");
        return -1;
    }
    int *nr = malloc(sizeof(int));
    *nr = strtol(argv[1], NULL, 10);
    call(nr);
    printf("%d processes printed.\n", *nr);
    free(nr);
    return 0;
}

/* 3/27: call356.c finished */

/**
 * 3/22: Try to use list to simulate the stack, but list is
 * only available in kernel mode. Using array instead.
 */

/* 3/21: call356.c completed */