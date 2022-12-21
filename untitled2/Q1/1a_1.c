#include<stdio.h>
#include<pthread.h>
#include<semaphore.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
int deadlock=0;
int sequencing[]={0,1,2,3,4};
sem_t forks[5];
void init_error_checker(int status){
    if(status){
        printf("Encountered error while executing sem_init()\n");
    }
}
void wait_error_checker(int status){
    if(status){
        printf("Encountered error while executing sem_wait()\n");
    }
}
void post_error_checker(int status){
    if(status){
        printf("Encountered error while executing sem_post()\n");
    }
}
void main_error_checker(int status, int type){
    if(status){
        if(type==-1){
            init_error_checker(status);
        }
        else if(type==0){
            wait_error_checker(status);
        }
        else if(type==1){
            post_error_checker(status);
        }
        else{
            printf("Unkown Error has occured\n");
        }
        exit(1);
    }
}
void *thinking_start(void *philosophers){
    int philosopher_pos=*(int*)philosophers;
    philosopher_pos++;
    while(100){
        int temp_status;
        printf("Philosopher at Position %d is Thinking right now\n",philosopher_pos);
        sleep(1);
        printf("Philosopher at Position %d is Hungry right now\n",philosopher_pos);
        temp_status=sem_wait(&forks[(philosopher_pos-1)%5]);
        main_error_checker(temp_status,0);
        if((philosopher_pos-1)%2==0)
            printf("Philosoper at Position %d takes the Fork at Position %d\n",philosopher_pos,((philosopher_pos-1)%5)+1);
        temp_status=sem_wait(&forks[(philosopher_pos)%5]);
        main_error_checker(temp_status,0);
        printf("Philosoper at Position %d takes the Fork at Position %d\n",philosopher_pos,(philosopher_pos%5)+1);
        printf("Philosopher at Position %d is Eating right now\n",philosopher_pos);
        sleep(1);
        temp_status=sem_post(&forks[(philosopher_pos-1)%5]);
        main_error_checker(temp_status,1);
        printf("Philosoper at Position %d puts the Fork at Position %d\n",philosopher_pos,((philosopher_pos-1)%5)+1);
        temp_status=sem_post(&forks[(philosopher_pos)%5]);
        main_error_checker(temp_status,1);
        printf("Philosoper at Position %d puts the Fork at Position %d\n",philosopher_pos,((philosopher_pos)%5)+1);
    }

}
int main(){
    pthread_t philosophers[5];
    int status;
    for(int i=0;i<5;i++){
        status=sem_init(&forks[i],0,1);
        main_error_checker(status,-1);
    }
    printf("*************************************\n");
    printf("Number of Philosophers: %d\n",5);
    printf("Number of Forks: %d\n",5);
    printf("*************************************\n");
    for(int i=0;i<5;i++){
        status=pthread_create(&philosophers[i],NULL,thinking_start,&sequencing[i]);
        if(status){
            printf("Error! Error Encountered while creating the threads\n");
            return 1;
        }
    }
    for(int i=0;i<5;i++){
        status=pthread_join(philosophers[i],NULL);
        if(status){
            printf("Error! Error Encountered while joining the threads\n");
            return 1;
        }
    }
    return 0;
}