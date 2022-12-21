#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<fcntl.h>
#include<errno.h>
#include<unistd.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include <semaphore.h>

#define len 6
#define num 50

char* semName= "sem1";

struct message{
    char msg[10];
    char* idx;
};

int idxsize(int i){
    if(i>=10){
        return 3;
    }
    else{
        return 2;
    }
}


int main(){

    struct message* sender ,*receiver;
    sender=(struct message*)malloc(sizeof(struct message));
    sender->idx=(char*)malloc(3*sizeof(char));
    receiver=(struct message*)malloc(sizeof(struct message));
    receiver->idx=(char*)malloc(3*sizeof(char));

    sem_t *sem = sem_open(semName,0,0666,0);


    key_t key = ftok("shmfile",50);
    int shmid = shmget(key,1024,0666|IPC_CREAT);

    sender = (struct message*)shmat(shmid,NULL,0);


    for(int i=0;i<5;i++){
        sem_wait(sem);
        receiver = sender;
        printf("received p2: %s %s\n",sender->msg,sender->idx);
        sem_post(sem);
    }
    printf("sent p2: %s\n",receiver->idx);


    return 0;
}
