#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/sched.h>
#include <sys/time.h>
#include <sched.h>
#include <time.h>

void multMat(int n, float *A, float *B, float *C){
    int i,j,k;
	float r;
    /* This is ikj loop order, which is most cache-friendly */
    for( i = 0; i < n; i++ )
        for( k = 0; k < n; k++ ){
			r = A[i*n+k];
            for( j = 0; j < n; j++ )
                  C[i*n+j] += r*B[k*n+j];
		}
}

int main(int argc, char **argv){
    if (argc != 3){
        printf("Error: argument number incorrect\n");
        return -1;
    }
    int *nmax = malloc(sizeof(int)), *childs = malloc(sizeof(int));
    *nmax = strtol(argv[1], NULL, 10);
    *childs = strtol(argv[2], NULL, 10);

    void (*orderings)(int, float *, float *, float *) = &multMat;
    char *schedulers[] = {"SCHED_NORMAL", "SCHED_FIFO", "SCHED_RR", "", "", "", "SCHED_WRR"};

    float *A = (float *)malloc(*nmax * *nmax * sizeof(float));
    float *B = (float *)malloc(*nmax * *nmax * sizeof(float));
    float *C = (float *)malloc(*nmax * *nmax * sizeof(float));

    pid_t *childPID = malloc(sizeof(pid_t) * *childs);
    pid_t itsPID;
    struct timeval start, end;
    struct sched_param param;
    struct timespec t;
    param.sched_priority = 50;

    int i, j;
    /* fill matrices with random numbers */
    for (i = 0; i < *nmax * *nmax; i++)
        A[i] = drand48() * 2 - 1;
    for (i = 0; i < *nmax * *nmax; i++)
        B[i] = drand48() * 2 - 1;
    for (i = 0; i < *nmax * *nmax; i++)
        C[i] = drand48() * 2 - 1;

    for (i = 0; i <= 6; i++){
        if (i == 3 || i == 4 || i == 5)
            continue;
        
        gettimeofday(&start, NULL);
        /* fork begin */
        for (j = 0; j < *childs; j++){
            itsPID = fork();
            if (itsPID > 0)
                childPID[j] = itsPID;
            else if (itsPID == 0){
                pid_t myPID = getpid();
                /* set scheduler */
                syscall(156, myPID, i, &param);                
                /* multiply matrices */
                (*orderings)(*nmax, A, B, C);
                exit(1);
            }
        }
        
        for (j = 0; j < *childs; j++)
            waitpid(childPID[j], NULL, 0);
        /* all children done */
        gettimeofday(&end, NULL);

        double seconds = (end.tv_sec - start.tv_sec) +
            1.0e-6 * (end.tv_usec - start.tv_usec);
        /* convert time to Mflop/s */
        double Mflops = 2e-6 * *nmax * *nmax * *nmax * *childs / seconds;

        printf("%s:\tn = %d, %.3f Mflop/s (%.3f seconds)\n",
               schedulers[i], *nmax, Mflops, seconds);
    }

    free(A);
    free(B);
    free(C);
    free(nmax);
    free(childs);

    printf("\n");

    return 0;
}

/* 5/30: bench.c complete */
