#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<fcntl.h>
#include<errno.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/un.h>
#include <math.h>


#define MAX_MESSAGE_SIZE 6
#define l 6
#define n 50

char* LOCAL = "./sockP2";
char* DESTINATION = "./sockP1";

struct myStruct{
    char* idx;
    char* msg;
};

int idxsize(int i){
    if(i>=10){
        return 3;
    }
    else return 2;
}

void receive(struct myStruct** myData, int* st){
    struct sockaddr_un address;
    int f1;

    if((f1 = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1){
        perror("Socket cannot be initialized!");
        exit(EXIT_FAILURE);
    }

    address.sun_family = AF_UNIX;
    memcpy(address.sun_path, LOCAL, strlen(LOCAL) + 1);

    unlink(LOCAL);
    if(bind(f1, (struct sockaddr*) &address, sizeof(address)) == -1){
        perror("Socket cannot be bound!");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_un emitter;
    socklen_t len;

    struct myStruct *temp = (struct myStruct*) malloc(sizeof(struct myStruct));
    temp->idx = (char*) malloc(sizeof(char)*(idxsize(*st)));
    temp->msg = (char*) malloc(sizeof(char)*MAX_MESSAGE_SIZE);
    size_t size;
    int ii = 0;
    while(ii!=5){
        size = recvfrom(f1, temp->idx, (idxsize(*st)), 0, (struct sockaddr *) &emitter, &len);
        if(size != -1) {}
        else{
            if(errno == ECONNRESET) printf("ECONNRESET\n");
            close(f1);
            perror("Receiver"); exit(EXIT_FAILURE);
        }
        if((*st)!=0 && strcmp(temp->idx, (*myData)[(*st)-1].idx)==0){
            break;
        }
        memcpy((*myData)[*st].idx, temp->idx, (idxsize(*st)));
        size = recvfrom(f1, temp->msg, l, 0, (struct sockaddr *) &emitter, &len);
        if(size != -1) {}
        else{
            if(errno == ECONNRESET) printf("ECONNRESET\n");
            close(f1);
            perror("Receiver"); exit(EXIT_FAILURE);
        }
        memcpy((*myData)[*st].msg, temp->msg, l);

        printf("%s %s\n", (*myData)[*st].idx, (*myData)[*st].msg);
        *st=*st+1;
        if(*st<n){
        }
        else{
            break;
        }
        ii++;
    }
    free(temp);
    close(f1);
}

void send_str(struct myStruct* myData, int *st){
    struct sockaddr_un address;
    int f1;

    address.sun_family = AF_UNIX;
    memcpy(address.sun_path, LOCAL, strlen(LOCAL) + 1);

    if((f1 = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1){
        perror("Socket cannot be initialized!");
        exit(EXIT_FAILURE);
    }

    unlink(LOCAL);
    if(bind(f1, (struct sockaddr*) &address, sizeof(address)) == -1){
        perror("Socket cannot be bound!");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_un destination;
    destination.sun_family = AF_UNIX;
    memcpy(destination.sun_path, DESTINATION, strlen(DESTINATION) + 1);

    sendto(f1, myData[(*st)-1].idx, (idxsize(*st-1)), 0, (struct sockaddr*) &destination, sizeof(destination));
    sendto(f1, myData[(*st)-1].msg, l, 0, (struct sockaddr*) &destination, sizeof(destination));

    printf("%s\n", myData[(*st)-1].idx);

    close(f1);
}

int main(int argc, const char* argv[]){

    struct myStruct *myData = (struct myStruct*) malloc(n*sizeof(struct myStruct));

    for (int i = 0; i < n; i++){
        myData[i].msg = (char*) malloc((l)*sizeof(char));
        myData[i].idx = (char*) malloc((idxsize(i))*sizeof(char));
    }

    int st=0;

    while(st<n){
        printf("Received data:\n");
        receive(&myData, &st);
        printf("\n\n");
//        sleep(1);
        printf("Sent data:\n");
        send_str(myData ,&st);
        printf("\n\n");
    }


    return 0;
}
