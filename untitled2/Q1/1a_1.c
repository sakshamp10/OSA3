#include<stdio.h>
#include<pthread.h>
#include<semaphore.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>


sem_t forks[5];

void *thinking_start(void *philosopher){
    int philpos=*(int*)philosopher;

    while(1){
        printf("Philosopher at Position %d is Thinking right now\n",philpos);
        sleep(1);
        printf("Philosopher at Position %d is Hungry right now\n",philpos);
        int st;
        st=sem_wait(&forks[(philpos-1)%5]);
        if(st){
            printf("Encountered error while executing sem_wait()\n");
            exit(-1);
        }

        if((philpos-1)%2==0)
            printf("Philosoper at Position %d takes the Fork at Position %d\n",philpos,((philpos-1)%5)+1);
        st=sem_wait(&forks[(philpos)%5]);
        if(st){
            printf("Encountered error while executing sem_wait()\n");
            exit(-1);
        }

        printf("Philosoper at Position %d takes the Fork at Position %d\n",philpos,(philpos%5)+1);
        printf("Philosopher at Position %d is Eating right now\n",philpos);
        sleep(1);
        st=sem_post(&forks[(philpos-1)%5]);
        if(st){
            printf("Encountered error while executing sem_post()\n");
            exit(-1);
        }
        printf("Philosoper at Position %d puts the Fork at Position %d\n",philpos,((philpos-1)%5)+1);
        st=sem_post(&forks[(philpos)%5]);
        if(st){
            printf("Encountered error while executing sem_post()\n");
            exit(-1);
        }
        printf("Philosoper at Position %d puts the Fork at Position %d\n",philpos,((philpos)%5)+1);
    }

}
int main(){
    pthread_t philosophers[5];
    int sequencing[]={1,2,3,4,5};
    int st;
    for(int i=0;i<5;i++){
        st=sem_init(&forks[i],0,1);
        if(st){
            printf("Encountered error while executing sem_init()\n");
            exit(-1);
        }
    }

    printf("Number of Philosophers: %d\n",5);
    printf("Number of Forks: %d\n\n\n",5);

    for(int i=0;i<5;i++){
        st=pthread_create(&philosophers[i],NULL,thinking_start,&sequencing[i]);
        if(st){
            printf("Error! Error Encountered while creating the threads\n");
            return 1;
        }
    }
    for(int i=0;i<5;i++){
        st=pthread_join(philosophers[i],NULL);
        if(st){
            printf("Error! Error Encountered while joining the threads\n");
            return 1;
        }
    }
    return 0;
}