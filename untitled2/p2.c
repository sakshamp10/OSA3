#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<fcntl.h>
#include<errno.h>
#include<unistd.h>

#define FIFO1 "./fifo1"
#define FIFO2 "./fifo2"

struct message{
    char msg[10];
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

    for(int i=0;i<50;i++){
        myMsg[i]= (struct message*)malloc(sizeof(struct message));
        myMsg[i]->idx=(char*)malloc(4*sizeof(char));
    }


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
    }else{
        printf("FIFO2 created!\n");
    }
    int curr=0;
    int i=0;
    while(curr<50){


        int f1;
        for(;i<curr+5;i++){
            f1= open(FIFO1,O_RDONLY);
            int ret = read(f1,myMsg[i]->idx,sizeof(idxsize(i)));
            if(ret==-1){
                printf("Error in reading in p2!\n");
                exit(-1);
            }
            read(f1,myMsg[i]->msg,sizeof(myMsg[i]->msg));
            printf("%s %s\n",myMsg[i]->idx,myMsg[i]->msg);
        }
        close(f1);

        sleep(1);

        curr=i;
        int f2 = open(FIFO2,O_WRONLY);
        write(f2,myMsg[curr-1]->idx,sizeof(idxsize(i)));
        write(f2,myMsg[curr-1]->msg,sizeof(myMsg[curr-1]->msg));
        printf("index(p2): %s\n",myMsg[curr-1]->idx);

    }
    return 0;
}