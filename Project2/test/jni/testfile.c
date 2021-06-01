#include <stdlib.h>
#include <linux/sched.h>
#include <linux/time.h>
#include <sched.h>

/* 
 * Syscall table locates in:
 * /arch/x86/syscalls/syscall_32.tbl
 * Whoa, that's so deep!
 * 
 * #156: sched_setscheduler(pid_t pid, int policy, struct sched_param* param)
 * #157: sched_getscheduler(pid_t pid)
 * #159: sched_get_priority_max(int policy)
 * #160: sched_get_priority_min(int policy)
 * #161: sched_rr_get_interval(pid_t pid, struct timespec* interval)
 */

void isPolicy(int i){
    switch(i){
    case 0:
        printf("SCHED_NORMAL\n");
        break;
    case 1:
        printf("SCHED_FIFO\n");
        break;
    case 2:
        printf("SCHED_RR\n");
        break;
    case 6:
        printf("SCHED_WRR\n");
        break;
    default:
        printf("ERROR POLICY!\n");
        break;
    }
}

int main(int argc, char *argv[]){
    int pid = 0, loop = 1, policy = 0, prio = 1;
    struct sched_param param;
    struct timespec t;
    long timeslice = 0;

    printf("========== Scheduler Test Start ==========");
    while (loop)
    {
        printf("\nPlease input the process id of testprocess first:");
        scanf("%d", &pid);
        policy = syscall(157, pid);
        printf("Current scheduling policy is: ");
        isPolicy(policy);
        if (policy == 2 || policy == 6){
            printf("With timeslice: ");
            syscall(161, pid, &t);
            timeslice = t.tv_nsec / 1000000;
            printf("%ld ms.\n", timeslice);
        }

        printf("Change to which scheduling policy? ");
        printf("(0 = NORMAL, 1 = FIFO, 2 = RR, 6 = WRR):");
        scanf("%d", &policy);
        if (policy == 1 || policy == 2 || policy == 6){
            printf("Set process's priority (%d-%d):",
                   syscall(160, policy), syscall(159, policy));
            scanf("%d", &prio);
            param.sched_priority = prio;
        }
        else
            param.sched_priority = 0;

        printf("\nChanging...");
        syscall(156, pid, policy, &param);
        printf("Done.\n");
        policy = syscall(157, pid);
        printf("\nCurrent scheduling policy is: ");
        isPolicy(policy);
        if (policy == 2 || policy == 6){
            printf("With timeslice: ");
            syscall(161, pid, &t);
            timeslice = t.tv_nsec / 1000000;
            printf("%ld ms.\n", timeslice);
        }

        printf("\nTest again? (1/0): ");
        scanf("%d", &loop);
    }
    printf("\n========== Scheduler Test End ==========\n");
    return 0;
}

/* 5/22: testfile.c complete */
