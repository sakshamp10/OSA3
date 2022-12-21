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


void acquire(char** sem){
    while(strcmp(*sem,"wait")!=0){
    }
}

void release(char** sem){
    strcpy(*sem,"");
}

int min(int a , int b){
    if(a>b) return b;
    else return a;
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
            msg[i][2] =' ';
        }
        else {
            j=2;
            msg[i][0] = '0' + i;
            msg[i][1] = ' ';
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
        int j=i;
        while(j<min(i+5,num)){
            strcpy(send,msg[j]);
            acquire(&send);
            j++;
        }
        i=j;
        printf("MAX ID received by p1: %d\n",i-1);
    }

    return 0;
}
