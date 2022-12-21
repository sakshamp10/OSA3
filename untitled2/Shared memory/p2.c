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

#define len 8
#define num 50
#define semfile "semaphorefile"


void acquire(char** sem){
    while(strcmp(*sem,"wait")==0){
    }
}

void release(char** sem){
    strcpy(*sem,"wait");
}



int main(){
    char* send=(char*)malloc((len)*sizeof(char));
    key_t key = ftok("shmfile",50);
    int shmid = shmget(key,1024,0666|IPC_CREAT);

    send = (char*)shmat(shmid,NULL,0);

    int curr=0;
    while(curr<num){
        for(int i=curr;i<curr+5;i++){
            acquire(&send);
            printf("received : %s\n",send);
            release(&send);
        }
        printf("max id sent: %d\n",curr+4);
    }
    return 0;
}
