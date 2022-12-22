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

void receive(struct myStruct** myData, int* start){
    struct sockaddr_un address;
    int fd;

    if((fd = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1){
        perror("Socket cannot be initialized!");
        exit(EXIT_FAILURE);
    }

    address.sun_family = AF_UNIX;
    memcpy(address.sun_path, LOCAL, strlen(LOCAL) + 1);

    unlink(LOCAL);
    if(bind(fd, (struct sockaddr*) &address, sizeof(address)) == -1){
        perror("Socket cannot be bound!");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_un emitter;
    socklen_t len;

    struct myStruct *temp = (struct myStruct*) malloc(sizeof(struct myStruct));
    temp->idx = (char*) malloc(sizeof(char)*(idxsize(*start)));
    temp->msg = (char*) malloc(sizeof(char)*MAX_MESSAGE_SIZE);
    size_t size;
    int ii = 0;
    while(ii!=5){
        // printf("Write a message: ");
        size = recvfrom(fd, temp->idx, (idxsize(*start)), 0, (struct sockaddr *) &emitter, &len);
        if(size == -1) {
            if(errno == ECONNRESET) printf("ECONNRESET\n");
            close(fd);
            perror("Receiver"); exit(EXIT_FAILURE);
        }
        if((*start)!=0 && strcmp(temp->idx, (*myData)[(*start)-1].idx)==0){
            break;
        }
        memcpy((*myData)[*start].idx, temp->idx, (idxsize(*start)));
        size = recvfrom(fd, temp->msg, l, 0, (struct sockaddr *) &emitter, &len);
        if(size == -1) {
            if(errno == ECONNRESET) printf("ECONNRESET\n");
            close(fd);
            perror("Receiver"); exit(EXIT_FAILURE);
        }
        memcpy((*myData)[*start].msg, temp->msg, l);

        printf("%s %s\n", (*myData)[*start].idx, (*myData)[*start].msg);
        *start=*start+1;
        if(*start<n){
        }
        else{
            break;
        }
        ii++;
    }
    free(temp);
    close(fd);
}

void send_str(struct myStruct* myData, int *start){
    struct sockaddr_un address;
    int fd;

    address.sun_family = AF_UNIX;
    memcpy(address.sun_path, LOCAL, strlen(LOCAL) + 1);

    if((fd = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1){
        perror("Socket cannot be initialized!");
        exit(EXIT_FAILURE);
    }

    unlink(LOCAL);
    if(bind(fd, (struct sockaddr*) &address, sizeof(address)) == -1){
        perror("Socket cannot be bound!");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_un destination;
    destination.sun_family = AF_UNIX;
    memcpy(destination.sun_path, DESTINATION, strlen(DESTINATION) + 1);

    sendto(fd, myData[(*start)-1].idx, (idxsize(*start-1)), 0, (struct sockaddr*) &destination, sizeof(destination));
    sendto(fd, myData[(*start)-1].msg, l, 0, (struct sockaddr*) &destination, sizeof(destination));

    printf("%s\n", myData[(*start)-1].idx);
    // printf("%s %s\n", myData[(*start)-1].idx, myData[(*start)-1].msg);

    close(fd);
}

int main(int argc, const char* argv[]){

    struct myStruct *myData = (struct myStruct*) malloc(n*sizeof(struct myStruct));

    for (int i = 0; i < n; i++){
        myData[i].msg = (char*) malloc((l)*sizeof(char));
        myData[i].idx = (char*) malloc((idxsize(i))*sizeof(char));
    }

    int start=0;

    while(start<n){
        printf("Received data:\n");
        receive(&myData, &start);
        printf("----------------\n");
        sleep(1);
        printf("Sent data:\n");
        send_str(myData ,&start);
        printf("----------------\n");
    }


    return 0;
}
