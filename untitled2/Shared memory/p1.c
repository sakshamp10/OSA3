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

#define len 9
#define num 50
#define semfile "semaphorefile"

char* semName= "sem1";

struct message{
    char msg[10];
    char* idx;
};


void acquire(char** sem){
    while(strcmp(*sem,"wait")==0){
    }
    strcpy(*sem,"wait");
}

void release(char** sem){
    strcpy(*sem,"");
}


int main(){
    srand(time(NULL));
    char* msg[50];
    for(int i=0;i<num;i++){
        msg[i] = (char*)malloc((len)*sizeof(char));
    }
    //  generate_strings
    for(int i=0;i<num;i++){
        int j;
        if(i>=10){
            j=3;
            msg[i][0] = '0'+i/10;
            msg[i][1] = '0'+i%10;
            msg[i][2] ='\0';
        }
        else {
            j=2;
            msg[i][0] = '0' + i;
            msg[i][1] = '\0';
        }

        while( (j) <= len-2){
            msg[i][j] = 65 + rand()%26;
            j++;
        }
        msg[i][len-1]='\0';
    }


    char* send=(char*)malloc(sizeof(msg[0]));
    key_t key = ftok("shmfile",50);
    int shmid = shmget(key,1024,0666|IPC_CREAT);

    send = (char*) shmat(shmid,NULL,0);
    for(int i=0;i<num;){
        int j=0;
        while(j<5){
            acquire(&send);
            strcpy(send,msg[i]);
            j++;
        }
        i+=j;
        printf("MAX ID received: %d\n",i);

    }

    return 0;
}
