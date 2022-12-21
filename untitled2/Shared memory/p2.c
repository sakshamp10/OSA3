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


void acquire(int sf){
    char* a = "1";
    while(strcmp(a,"1")==0 || a==NULL){
        lseek(sf,0,SEEK_SET);
        read(sf,&a,1);
    }
    write(sf,"0",1);
}

void release(int sf){
    lseek(sf,0,SEEK_SET);
    write(sf,"1",1);
}



int main(){
    char* send=(char*)malloc((len)*sizeof(char));
    key_t key = ftok("shmfile",50);
    int shmid = shmget(key,1024,0666|IPC_CREAT);

    send = (char*)shmat(shmid,NULL,0);

    int sf = open(semfile,O_RDWR|O_CREAT,0666);

    int curr=0;
    while(curr<num){
        for(int i=curr;i<curr+5;i++){
            acquire(sf);
            printf("received : %s\n",send);
            release(sf);
        }
        printf("max id sent: %d\n",curr+4);
    }

    return 0;
}
