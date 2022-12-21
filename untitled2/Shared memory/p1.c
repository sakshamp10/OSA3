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
    srand(time(NULL));
    struct message* myMsg[50];
    struct message* sender,*receiver;

    sem_t *sem = sem_open(semName,O_CREAT,0666,1);

    sender=(struct message*)malloc(sizeof(struct message));
    sender->idx=(char*)malloc(3*sizeof(char));
    receiver=(struct message*)malloc(sizeof(struct message));
    receiver->idx=(char*)malloc(3*sizeof(char));

    for(int i=0;i<50;i++){
        myMsg[i]= (struct message*)malloc(sizeof(struct message));

    }


//  generate_strings
    for(int i=0;i<num;i++){
        myMsg[i]->idx=(char*)malloc(idxsize(i)*sizeof(char));
        int j=0;
        while( (j) <= len-2){
            myMsg[i]->msg[j] = 65 + rand()%26;
            j++;
        }
        myMsg[i]->msg[len-1]='\0';

        if(i>=10){
            myMsg[i]->idx[0] = '0'+i/10;
            myMsg[i]->idx[1] = '0'+i%10;
            myMsg[i]->idx[2] ='\0';
        }
        else {
            myMsg[i]->idx[0] = '0' + i;
            myMsg[i]->idx[1] = '\0';
        }
//        printf("%s %s\n",myMsg[i]->msg,myMsg[i]->idx);
    }

    // open shared memory

    key_t key = ftok("shmfile",50);
    int shmid = shmget(key,1024,0666|IPC_CREAT);

    sender = (struct message*) shmat(shmid,NULL,0);

    int curr=0;
    while(curr<num){
        int i;
        for(i=curr;i<curr+5;i++){
            sem_wait(sem);
            sender = myMsg[i];
            sem_post(sem);
        }
        curr=i;

        sem_wait(sem);
        int ans=0;
        int j = 0;
        while(sender->idx[j]!='\0'){
            ans *= 10;
            ans += (int)((sender->idx[j]-'0'));
            i++;
        }
        if(ans==curr){
            //received back from p2
            printf("received p1: %s\n",sender->idx);
        }
        sem_post(sem);
    }


    return 0;
}
