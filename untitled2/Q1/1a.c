#include<stdio.h>
#include<pthread.h>
#include<semaphore.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>

#define NO_OF_PHILS 5
#define NO_OF_FORKS 5
#define NO_OF_SAUCE_BOWLS 4
#define TIME_T 0.5
#define DEADLOCK 0

sem_t lock;
sem_t forks[NO_OF_FORKS];
sem_t bowls;

int nums[NO_OF_PHILS] = {0,1,2,3,4};

void check_error_sem(int ret_val, int flag){
    if(ret_val){
        if(flag==0){
            printf("Encountered some error in sem_init()");
        }
        else if(flag==1){
            printf("Encountered some error in sem_wait()");
        }
        else if(flag==2){
            printf("Encountered some error in sem_post()");
        }
        else if(flag==3){
            printf("Encountered some error in sem_getvalue()");
        }
        else {
            printf("Encountered some error");
        }
        exit(1);
    }
}

void* start_thinking(void* phil){
    int phil_no = (*(int*)phil);
    int bowl_no;
    if(DEADLOCK || phil_no+1<NO_OF_PHILS){
        while(1){
            printf("Philosopher %d is thinking\n", phil_no+1);
            sleep(TIME_T);
            printf("Philosopher %d is hungry\n", phil_no+1);

            check_error_sem(sem_wait(&forks[(phil_no)%NO_OF_FORKS]), 1);
            printf("Philosopher %d takes fork %d\n", phil_no+1, ((phil_no)%NO_OF_FORKS)+1);

            check_error_sem(sem_wait(&forks[(phil_no+1)%NO_OF_FORKS]), 1);
            printf("Philosopher %d takes fork %d\n", phil_no+1, ((phil_no+1)%NO_OF_FORKS)+1);

            check_error_sem(sem_wait(&lock), 1);
            check_error_sem(sem_wait(&bowls), 1);
            check_error_sem(sem_getvalue(&bowls, &bowl_no), 3);
            printf("Philosopher %d takes sauce bowl [Available sauce bowls: %d]\n", phil_no+1, bowl_no);
            check_error_sem(sem_post(&lock), 2);

            printf("Philosopher %d is eating\n", phil_no+1);
            sleep(TIME_T);

            check_error_sem(sem_wait(&lock), 1);
            check_error_sem(sem_post(&bowls), 2);
            check_error_sem(sem_getvalue(&bowls, &bowl_no), 3);
            printf("Philosopher %d puts sauce bowl [Available sauce bowls: %d]\n", phil_no+1, bowl_no);
            check_error_sem(sem_post(&lock), 2);

            check_error_sem(sem_post(&forks[(phil_no)%NO_OF_FORKS]), 2);
            printf("Philosopher %d puts fork %d\n", phil_no+1, ((phil_no)%NO_OF_FORKS)+1);

            check_error_sem(sem_post(&forks[(phil_no+1)%NO_OF_FORKS]), 2);
            printf("Philosopher %d puts fork %d\n", phil_no+1, ((phil_no+1)%NO_OF_FORKS)+1);
        }
    }
    else{
        while(1){
            printf("Philosopher %d is thinking\n", phil_no+1);
            sleep(TIME_T);
            printf("Philosopher %d is hungry\n", phil_no+1);

            check_error_sem(sem_wait(&forks[(phil_no+1)%NO_OF_FORKS]), 1);
            printf("Philosopher %d takes fork %d\n", phil_no+1, ((phil_no+1)%NO_OF_FORKS)+1);

            check_error_sem(sem_wait(&forks[(phil_no)%NO_OF_FORKS]), 1);
            printf("Philosopher %d takes fork %d\n", phil_no+1, ((phil_no)%NO_OF_FORKS)+1);

            check_error_sem(sem_wait(&lock), 1);
            check_error_sem(sem_wait(&bowls), 1);
            check_error_sem(sem_getvalue(&bowls, &bowl_no), 3);
            printf("Philosopher %d takes sauce bowl [Available sauce bowls: %d]\n", phil_no+1, bowl_no);
            check_error_sem(sem_post(&lock), 2);

            printf("Philosopher %d is eating\n", phil_no+1);
            sleep(TIME_T);

            check_error_sem(sem_wait(&lock), 1);
            check_error_sem(sem_post(&bowls),2);
            check_error_sem(sem_getvalue(&bowls, &bowl_no), 3);
            printf("Philosopher %d puts sauce bowl [Available sauce bowls: %d]\n", phil_no+1, bowl_no);
            check_error_sem(sem_post(&lock), 2);

            check_error_sem(sem_post(&forks[(phil_no+1)%NO_OF_FORKS]), 2);
            printf("Philosopher %d puts fork %d\n", phil_no+1, ((phil_no+1)%NO_OF_FORKS)+1);

            check_error_sem(sem_post(&forks[(phil_no)%NO_OF_FORKS]), 2);
            printf("Philosopher %d puts fork %d\n", phil_no+1, ((phil_no)%NO_OF_FORKS)+1);
        }

    }
}

int main(){
    pthread_t phil_id[NO_OF_PHILS];

    sem_init(&lock, 0, 1);
    for (int i = 0; i < NO_OF_FORKS; i++){
        check_error_sem(sem_init(&forks[i], 0, 1), 0);
    }
    check_error_sem(sem_init(&bowls, 0, NO_OF_SAUCE_BOWLS), 0);

    printf("----------------------------------\n");
    printf("# of Philosophers: %d\n", NO_OF_PHILS);
    printf("# of Forks: %d\n", NO_OF_FORKS);
    printf("# of Sauce Bowls: %d\n", NO_OF_SAUCE_BOWLS);
    printf("----------------------------------\n");

    int status;
    for (int i = 0; i < NO_OF_PHILS; i++){
        status = pthread_create(&phil_id[i], NULL, start_thinking, &nums[i]);
        if(status){
            fprintf(stderr, "pthread_create: %s\n", strerror(status));
            return EXIT_FAILURE;
        }
    }

    for (int i = 0; i < NO_OF_PHILS; i++){
        status = pthread_join(phil_id[i], NULL);
        if(status){
            fprintf(stderr, "pthread_join: %s\n", strerror(status));
            return EXIT_FAILURE;
        }
    }

    return 0;
}