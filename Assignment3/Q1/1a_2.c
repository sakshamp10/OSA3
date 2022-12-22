#include<stdio.h>
#include<pthread.h>
#include<semaphore.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
int deadlock=0;
pthread_mutex_t forks[5];


void *thinking_start(void *philosopher){
    int st;
    int phil_pos=*(int*)philosopher;
    while(1){
        printf("Philosopher no. %d is Thinking right now\n",phil_pos);
        sleep(1);
        printf("Philosopher no. %d is Hungry right now\n",phil_pos);
        st=pthread_mutex_lock(&forks[(phil_pos-1)%5]);

        if(st){
            printf("Encountered error while in Lock()\n");
            exit(-1);
        }


        printf("Philosopher no. %d takes the Fork no. %d\n",phil_pos,((phil_pos-1)%5)+1);
        st=pthread_mutex_lock(&forks[(phil_pos)%5]);

        if(st){
            printf("Encountered in Lock()\n");
            exit(-1);
        }


        printf("Philosopher no. %d takes the Fork no. %d\n",phil_pos,(phil_pos%5)+1);
        printf("Philosopher no. %d is Eating right now\n",phil_pos);
        sleep(1);
        st=pthread_mutex_unlock(&forks[(phil_pos-1)%5]);

        if(st){
            printf("Encountered error in Unlock()\n");
            exit(-1);
        }

        printf("Philosoper no. %d puts the Fork no. %d\n",phil_pos,((phil_pos-1)%5)+1);
        st=pthread_mutex_unlock(&forks[(phil_pos)%5]);

        if(st){
            printf("Encountered error in Unlock()\n");
            exit(-1);
        }

        printf("Philosopher no. %d puts down Fork no. %d\n",phil_pos,((phil_pos)%5)+1);
    }

}
int main(){
    pthread_t philosophers[5];
    int phil_positions[]={1,2,3,4,5};
    int st;
    for(int i=0;i<5;i++){
        st=pthread_mutex_init(&forks[i],NULL);
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