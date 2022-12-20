#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<fcntl.h>
#include<errno.h>
#include<unistd.h>
#include <sys/un.h>

int msgSize=6;
char* FIFO1 = "fifo1";
char* FIFO2 = "fifo2";

struct message{
    char msg[6];
    char* idx;
};

int idxsize(int i){
    if(i>=9){
        return 3;
    }
    else{
        return 2;
    }
}

int main(){
    struct message* myMsg[50];
    struct message* reader= (struct message*)malloc(sizeof(struct message));
    reader->idx= (char*)malloc(sizeof(char)*4);


    //code to generate strings
    srand(time(NULL));
    for(int i=0;i<50;i++){
        myMsg[i]=(struct message*)malloc(sizeof(struct message));
        for(int j=0;j<msgSize;j++){
            myMsg[i]->msg[j] = (char)(65 + rand()%26);
        }
        myMsg[i]->msg[msgSize-1]='\0';
        myMsg[i]->idx = (char*)malloc((4*sizeof(char)));

        int a = i+1;
//        while(a>0){
            if(a>=10){
                myMsg[i]->idx[1] = (char)(48+a%10);
                myMsg[i]->idx[0] = (char)(48+a/10);
                myMsg[i]->idx[2] = '\0';
            }
            else{
                myMsg[i]->idx[1] = '\0';
                myMsg[i]->idx[0] = (char)(48+a);
            }

//        }
    }

    for(int i=0;i<50;i++){
        printf("%s %s\n",myMsg[i]->idx,myMsg[i]->msg);
    }

    //code to send and receive the message using fifo

    if(mkfifo(FIFO1,0666)==-1){
        printf("\nError opening fifo1!\n\n");
        exit(-1);
    }
    else{
        printf("FIFO1 created!\n");
    }
    if(mkfifo(FIFO2,0666)==-1){
        printf("\nError opening fifo2!\n\n");
        exit(-1);
    }
    else{
        printf("FIFO2 created!\n");
    }

    int f1=open(FIFO1,O_WRONLY);
    int f2=open(FIFO2,O_RDONLY);
    int curr=0;
    int i=0;
    while(curr<50){
        printf("----Sending message----\n");
        for(i=curr;i<curr+5;i++){
            write(f1,myMsg[i]->idx,sizeof(idxsize(i)));
            write(f1,myMsg[i]->msg,sizeof(myMsg[i]->msg));
        }
        sleep(1);
        read(f2,reader->idx,sizeof(idxsize(i)));
        read(f2,reader->msg,sizeof(reader->msg));
        close(f2);

        printf("\n\n------Max index received: %s------\n",reader->idx);
        curr+=atoi(reader->idx);
        sleep(1);
    }


    return 0;
}