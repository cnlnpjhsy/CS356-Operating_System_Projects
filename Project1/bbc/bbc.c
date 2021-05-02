#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>

sem_t burger_empty, burger_full;
sem_t cashier_free, *cashier_order, *cashier_burger;
sem_t customer_presence;
int next_cashier;
sem_t next_cashier_mutex;

int customer_count, customer_end;
sem_t running_end;

#define MAKING_BURGER_MAX 5
#define ORDER_MAX 3
#define ARRAVING_MAX 30
#define SEED 0

struct members{
    pthread_t *cook_ptr;
    pthread_t *cashier_ptr;
    pthread_t *customer_ptr;
    int C;
    int M;
    int B;
};

void *cook(void *arg){
    int id = *(int *)arg + 1;
    while (1){
        sem_wait(&burger_empty);
        sleep(rand() % MAKING_BURGER_MAX);  // Making a burger
        printf("Cook [%d] make a burger.\n", id);
        sem_post(&burger_full);
    }
}

void *cashier(void *arg){
    int id = *(int *)arg + 1;
    while (1){
        int sema;
        sem_wait(&customer_presence); // Sleeping

        sem_wait(&next_cashier_mutex);  // Change the next cashier
        next_cashier = id;    
        sem_post(&cashier_free);    // Inform the customer to get ID
        sem_wait(&cashier_order[id - 1]);  // Waiting order           
        
        printf("Cashier [%d] accepts an order.\n", id);       
        sem_wait(&burger_full);     // Waiting burgers
        sem_post(&burger_empty);    
        sem_post(&cashier_burger[id - 1]);  // Presenting food
        printf("Cashier [%d] take a burger to customer.\n", id);
    }
}

void *customer(void *arg){
    int id = *(int *)arg + 1;
    int sema;
    sleep(rand() % ARRAVING_MAX); // Arriving
    printf("Customer [%d] come.\n", id);    
    sem_post(&customer_presence);

    sem_wait(&cashier_free);
    int cashierID = next_cashier;
    sem_post(&next_cashier_mutex);  // Got the cashier ID, mutex released

    sleep(rand() % ORDER_MAX);  // Ordering...
    sem_post(&cashier_order[cashierID - 1]);    // done.
    sem_wait(&cashier_burger[cashierID - 1]); // Waiting food
    /* No loop - customer leave */
    customer_count++;
    if(customer_count==customer_end)
        sem_post(&running_end);
}

void *global(void *arg){
    struct members *mem = (struct members *)arg;
    sem_wait(&running_end);
    int i;
    for (i = 0; i < mem->C; i++){
        pthread_cancel(mem->cook_ptr[i]);
    }
    for (i = 0; i < mem->M; i++){
        pthread_cancel(mem->cashier_ptr[i]);
    }
    for (i = 0; i < mem->B; i++){
        pthread_cancel(mem->customer_ptr[i]);
    }
}

int main(int argc, char **argv){
    if(argc != 5){
        printf("Error: wrong argument number\n");
        return -1;
    }
    /* Cook, Cashier, Customer, Reck */
    int C = strtol(argv[1], NULL, 10),
        M = strtol(argv[2], NULL, 10),
        B = strtol(argv[3], NULL, 10),
        R = strtol(argv[4], NULL, 10);
    int i = 0;
    customer_count = 0, customer_end = B;
    srand(SEED);    // Define a seed you like or time(NULL)

    printf("Cooks[%d], Cashiers[%d], Customers[%d]\n", C, M, B);

    /* Semaphore initialize */
    sem_init(&burger_empty, 0, R);
    sem_init(&burger_full, 0, 0);
    sem_init(&cashier_free, 0, 0);
    cashier_order = malloc(M * sizeof(sem_t));
    cashier_burger = malloc(M * sizeof(sem_t));    
    for (i = 0; i < M; i++){
        sem_init(&cashier_order[i], 0, 0);
        sem_init(&cashier_burger[i], 0, 0);
    }
    sem_init(&customer_presence, 0, 0);
    sem_init(&next_cashier_mutex, 0, 1);
    sem_init(&running_end, 0, 0);

    /* Thread initialize */
    pthread_t *cook_t = malloc(C * sizeof(pthread_t)),
              *cashier_t = malloc(M * sizeof(pthread_t)),
              *customer_t = malloc(B * sizeof(pthread_t)),
              global_t;
    int *cook_arg = malloc(C * sizeof(int)),
        *cashier_arg = malloc(M * sizeof(int)),
        *customer_arg = malloc(B * sizeof(int));
    for (i = 0; i < C; i++)
    {
        cook_arg[i] = i;
        pthread_create(&cook_t[i], NULL, cook, (void *)&cook_arg[i]);
    }
    for (i = 0; i < M; i++){
        cashier_arg[i] = i;
        pthread_create(&cashier_t[i], NULL, cashier, (void *)&cashier_arg[i]);
    }
    for (i = 0; i < B; i++){
        customer_arg[i] = i;
        pthread_create(&customer_t[i], NULL, customer, (void *)&customer_arg[i]);
    }

    /* Member management */
    struct members global_arg;
    global_arg.cook_ptr = cook_t;
    global_arg.cashier_ptr = cashier_t;
    global_arg.customer_ptr = customer_t;
    global_arg.C = C;
    global_arg.M = M;
    global_arg.B = B;
    pthread_create(&global_t, NULL, global, (void *)&global_arg);

    /* Now begin run! */
    printf("Begin run.\n");
    pthread_join(global_t, NULL);
    for (i = 0; i < C; i++){
        pthread_join(cook_t[i], NULL);
    }
    for (i = 0; i < M; i++){
        pthread_join(cashier_t[i], NULL);
    }
    for (i = 0; i < B; i++){
        pthread_join(customer_t[i], NULL);
    }

    /* ... ended. */
    printf("Running ended.\n");

    /* Post-work */
    sem_destroy(&burger_empty);
    sem_destroy(&burger_full);
    sem_destroy(&cashier_free);
    for (i = 0; i < M; i++){
        sem_destroy(&cashier_order[i]);
        sem_destroy(&cashier_burger[i]);
    }
    sem_destroy(&customer_presence);
    sem_destroy(&next_cashier_mutex);
    sem_destroy(&running_end);

    free(cashier_order);
    free(cashier_burger);
    free(cook_t);
    free(cashier_t);
    free(customer_t);
    free(cook_arg);
    free(cashier_arg);
    free(customer_arg);

    return 0;
}