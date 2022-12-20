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

#define PENDING_QUEUE_SIZE 1
#define MAX_MESSAGE_SIZE 6
#define len 6
#define num 50

char* LOCAL = "./sockP1";
char* DESTINATION = "./sockP2";

int min(int a , int b){
    if(a>b) return b;
    else return a;
}
struct myStruct{
    char* myIdx;
    char* myStr;
};

int idxsize(int i){
    if(i>=10){
        return 3;
    }
    else return 2;
}

void receive_t_rand_str(struct myStruct** myData, int* start){
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
    socklen_t length;

    struct myStruct *temp = (struct myStruct*) malloc(sizeof(struct myStruct));
    temp->myIdx = (char*) malloc(sizeof(char)*((*start)<10?2:3));
    temp->myStr = (char*) malloc(sizeof(char)*MAX_MESSAGE_SIZE);
    size_t size;
    int j = 0;
    while(j!=5){
        // printf("Write a message: ");
        size = recvfrom(fd, temp->myIdx, ((*start)<10?2:3), 0, (struct sockaddr *) &emitter, &length);
        if(size == -1) {
            if(errno == ECONNRESET) printf("ECONNRESET\n");
            close(fd);
            perror("Receiver"); exit(EXIT_FAILURE);
        }
        if((*start)!=0 && strcmp(temp->myIdx, (*myData)[(*start)-1].myIdx)==0){
            break;
        }
        memcpy((*myData)[*start].myIdx, temp->myIdx, ((*start)<10?2:3));
        size = recvfrom(fd, temp->myStr, len, 0, (struct sockaddr *) &emitter, &length);
        if(size == -1) {
            if(errno == ECONNRESET) printf("ECONNRESET\n");
            close(fd);
            perror("Receiver"); exit(EXIT_FAILURE);
        }
        memcpy((*myData)[*start].myStr, temp->myStr, len);

        printf("%s %s\n", (*myData)[*start].myIdx, (*myData)[*start].myStr);
        *start=*start+1;
        if(*start<num){}
        else{
            break;
        }
        j++;
    }
    free(temp);
    close(fd);
}

void send_last_rand_str(struct myStruct* myData, int *start){
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

    sendto(fd, myData[(*start)-1].myIdx, ((*start)-1<10?2:3), 0, (struct sockaddr*) &destination, sizeof(destination));
    sendto(fd, myData[(*start)-1].myStr, len, 0, (struct sockaddr*) &destination, sizeof(destination));

    printf("%s\n", myData[(*start)-1].myIdx);
    close(fd);
}

int main(int argc, const char* argv[]){
    int num_of_rand_str = 50;
    int len_of_rand_str = 6;

    struct myStruct *myData = (struct myStruct*) malloc(num_of_rand_str*sizeof(struct myStruct));

    for (int i = 0; i < num_of_rand_str; i++){
        myData[i].myStr = (char*) malloc((len_of_rand_str)*sizeof(char));
        myData[i].myIdx = (char*) malloc((i<10?2:3)*sizeof(char));
    }

    int start=0;
    while(start<num_of_rand_str){
        printf("Received data:\n");
        receive_t_rand_str(&myData, &start);
        printf("----------------\n");
        sleep(1);
        printf("Sent data:\n");
        send_last_rand_str(myData, &start);
        printf("----------------\n");
    }

    return 0;
}
