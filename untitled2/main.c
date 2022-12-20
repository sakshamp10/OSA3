#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<fcntl.h>
#include<errno.h>
#include<unistd.h>

int msgSize=6;
#define FIFO1 "./fifo1"

struct message{
    char msg[10];
    char* idx;
};

int main(){
    struct message* myMsg[50];


    //code to generate strings
    srand(time(NULL));
    for(int i=0;i<50;i++){
        myMsg[i]=(struct message*)malloc(sizeof(struct message));
        for(int j=0;j<msgSize;j++){
            myMsg[i]->msg[j] = (char)(65 + rand()%26);
        }
        myMsg[i]->msg[msgSize]='\0';
        myMsg[i]->idx = (char*)malloc((3*sizeof(char)));
        itoa(i+1,(myMsg[i]->idx),10);
    }

//    for(int i=0;i<50;i++){
//        printf("%s ",myMsg[i]->idx);
//        printf("%s\n",myMsg[i]->msg);
//
//    }

    //code to send and receive the message using fifo

    if(mkfifo(FIFO1,0666)==-1){
        printf("\nError opening fifo!\n\n");
        exit(-1);
    }
    else{
        printf("FIFO created!\n");
    }

    int f1=open(FIFO1,O_WRONLY);
    int curr=0;
    while(curr<50){
        printf("----Sending message----\n");
        for(int i=curr;i<curr+5;i++){
            write(f1,myMsg[i]->idx,sizeof(myMsg[i]->idx));
            write(f1,myMsg[i]->msg,sizeof(myMsg[i]->msg));
        }
    }


    return 0;
}