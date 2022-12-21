#include<stdio.h>
#include<pthread.h>
#include<semaphore.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
int deadlock=0;

sem_t forks[5],bowl1,bowl2;
int bowl1un,bowl2un;


void *thinking_start(void *philosopher){
    int flag=0;
    int phil_pos=*(int*)philosopher;
    phil_pos++;
    while(100){
        int b1,b2;
        int st;
        if(flag==0){
            printf("Philosopher at Position %d is Thinking right now\n",phil_pos);
            sleep(1);
            printf("Philosopher at Position %d is Hungry right now\n",phil_pos);
            st=sem_wait(&forks[(phil_pos-1)%5]);
            if(st){
                printf("Encountered error while executing sem_wait()\n");
            }
            printf("Philosoper at Position %d takes the Fork at Position %d\n",phil_pos,((phil_pos-1)%5)+1);
            st=sem_wait(&forks[(phil_pos)%5]);
            if(st){
                printf("Encountered error while executing sem_wait()\n");
            }
            printf("Philosoper at Position %d takes the Fork at Position %d\n",phil_pos,(phil_pos%5)+1);
            flag=1;
            sem_getvalue(&bowl1,&b1);
            sem_getvalue(&bowl2,&b2);
        }
        if(bowl1un==b1){
            st=sem_wait(&bowl1);
            if(st){
                printf("Encountered error while executing sem_wait()\n");
            }

            printf("Philosopher at Position %d is Eating right now from Bowl1\n",phil_pos);
            sleep(1);
            st=sem_post(&bowl1);
            if(st){
                printf("Encountered error while executing sem_post()\n");
            }

            st=sem_post(&forks[(phil_pos-1)%5]);
            if(st){
                printf("Encountered error while executing sem_post()\n");
            }

            printf("Philosoper at Position %d puts the Fork at Position %d\n",phil_pos,((phil_pos-1)%5)+1);
            st=sem_post(&forks[(phil_pos)%5]);
            if(st){
                printf("Encountered error while executing sem_post()\n");
            }
            printf("Philosoper at Position %d puts the Fork at Position %d\n",phil_pos,((phil_pos)%5)+1);
            flag=0;
        }
        else if(bowl2un==b2){
            st=sem_wait(&bowl2);

            if(st){
                printf("Encountered error while executing sem_wait()\n");
            }
            printf("Philosopher at Position %d is Eating right now from Bowl2\n",phil_pos);
            sleep(1);
            st=sem_post(&bowl2);
            if(st){
                printf("Encountered error while executing sem_post()\n");
            }

            st=sem_post(&forks[(phil_pos-1)%5]);
            if(st){
                printf("Encountered error while executing sem_post()\n");
            }
            printf("Philosoper at Position %d puts the Fork at Position %d\n",phil_pos,((phil_pos-1)%5)+1);
            st=sem_post(&forks[(phil_pos)%5]);
            if(st){
                printf("Encountered error while executing sem_post()\n");
            }
            printf("Philosoper at Position %d puts the Fork at Position %d\n",phil_pos,((phil_pos)%5)+1);
            flag=0;
        }
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
        }
    }
    int s_b1=sem_init(&bowl1,0,1);
    if(s_b1){
        printf("Encountered error while executing sem_init()\n");
    }
    int s_b2=sem_init(&bowl2,0,1);
    if(s_b2){
        printf("Encountered error while executing sem_init()\n");
    }
    printf("*************************************\n");
    printf("Number of Philosophers: %d\n",5);
    printf("Number of Forks: %d\n",5);
    printf("*************************************\n");
    sem_getvalue(&bowl1,&bowl1un);
    sem_getvalue(&bowl2,&bowl2un);
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