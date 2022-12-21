#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<fcntl.h>
#include<errno.h>
#include<unistd.h>
//#include<sys/ipc.h>

#define len 6
#define num 50

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
void generate_strings(struct message** myMsg, int curr){

    *myMsg = (struct message*) malloc(num*sizeof(struct message));
        (*myMsg)->idx = (char*) malloc(idxsize(curr)*sizeof(char));

        if(curr>=10){
            (*myMsg)->idx[0] = '0'+curr/10;
            (*myMsg)->idx[1] = '0'+curr%10;
            (*myMsg)->idx[2] ='\0';
        }
        else{
            (*myMsg)->idx[0] = '0'+curr;
            (*myMsg)->idx[1] = '\0';
        }

        int i=0;
        while( (i) <= len-2){
            (*myMsg)->msg[i] = 65 + rand()%26;
            i++;
        }
        (*myMsg)->msg[len-1] = '\0';
}

int main(){
    srand(time(NULL));
    struct message* myMsg[50];
    struct message* sender,*receiver;

    sender=(struct message*)malloc(sizeof(struct message));
    sender->idx=(char*)malloc(3*sizeof(char));
    receiver=(struct message*)malloc(sizeof(struct message));
    receiver->idx=(char*)malloc(3*sizeof(char));

    for(int i=0;i<50;i++){
        myMsg[i]= (struct message*)malloc(sizeof(struct message));

    }



    for(int i=0;i<num;i++){
//        generate_strings(&myMsg[i],i);
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
        else{
            myMsg[i]->idx[0] = '0'+i;
            myMsg[i]->idx[1] = '\0';

        printf("%s %s\n",myMsg[i]->msg,myMsg[i]->idx);
    }

//    key_t key = ftok("shmfile",50);
//    int shmid = shmget(key,1024,0666|IPC_CREAT);

    return 0;
}
