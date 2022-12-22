#include<pthread.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<semaphore.h>


sem_t forks[5];

void *thinking_start(void *philosopher){
    int philpos=*(int*)philosopher;

    while(1){
        printf("Philosopher no. %d is Thinking right now\n",philpos);
        sleep(1);
        printf("Philosopher no. %d is Hungry right now\n",philpos);
        int st;
        st=sem_wait(&forks[(philpos-1)%5]);
        if(st){
            printf("Encountered error in sem_wait()\n");
            exit(-1);
        }

        if((philpos-1)%2==0)
            printf("Philosoper no. %d takes the Fork no. %d\n",philpos,((philpos-1)%5)+1);
        st=sem_wait(&forks[(philpos)%5]);
        if(st){
            printf("Encountered error in sem_wait()\n");
            exit(-1);
        }

        printf("Philosoper no. %d takes the Fork no. %d\n",philpos,(philpos%5)+1);
        printf("Philosopher no. %d is Eating right now\n",philpos);
        sleep(1);
        st=sem_post(&forks[(philpos-1)%5]);
        if(st){
            printf("Encountered error in sem_post()\n");
            exit(-1);
        }
        printf("Philosoper no. %d puts the Fork no. %d\n",philpos,((philpos-1)%5)+1);
        st=sem_post(&forks[(philpos)%5]);
        if(st){
            printf("Encountered error in sem_post()\n");
            exit(-1);
        }
        printf("Philosoper no. %d puts the Fork no. %d\n",philpos,((philpos)%5)+1);
    }

}
int main(){
    pthread_t philosophers[5];
    int phil_positions[]={1,2,3,4,5};
    int st;
    for(int i=0;i<5;i++){
        st=sem_init(&forks[i],0,1);
        if(st){
            printf("Encountered error in sem_init()\n");
            exit(-1);
        }
    }

    printf("Number of Philosophers: %d\n",5);
    printf("Number of Forks: %d\n\n\n",5);

    for(int i=0;i<5;i++){
        st=pthread_create(&philosophers[i],NULL,thinking_start,&phil_positions[i]);
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