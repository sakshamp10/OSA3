
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

#define MAX_MESSAGE_SIZE 6
#define LOCAL       "./sockP1"
#define DESTINATION "./sockP2"
#define len 6
#define num 50


int min(int a , int b){
    if(a>b) return b;
    else return a;
}

struct myStruct{
    char* myIdx;
    char* myStr;
};

void int_to_char(int curr, char **ptr){
    if(curr>=10){
        (*ptr)[0] = '0'+curr/10;
        (*ptr)[1] = '0'+curr%10;
        (*ptr)[2] ='\0';
    }
    else{
        (*ptr)[0] = '0'+curr;
        (*ptr)[1] = '\0';
    }
}

//int char_to_int(char** ptr){
//    int ans=0;
//    int i = 0;
//    while((*ptr)[i]!='\0'){
//        ans *= 10;
//        ans += (int)((*ptr)[i]-'0');
//        i++;
//    }
//    return ans;
//}

void generate_n_rand_str(struct myStruct** myData){
    srand(time(NULL));
    *myData = (struct myStruct*) malloc(num*sizeof(struct myStruct));
    int curr = 0;
    while(curr<num){
        (*myData)[curr].myStr = (char*) malloc((len)*sizeof(char));
        (*myData)[curr].myIdx = (char*) malloc((curr<10?2:3)*sizeof(char));
        int_to_char(curr, &(*myData)[curr].myIdx);

        for(int i = 0; i <= len-2; i++){
            (*myData)[curr].myStr[i] = 65 + rand()%26;
        }
        (*myData)[curr].myStr[len-1] = '\0';
        curr++;
    }
}

void send_t_rand_str(struct myStruct* myData, int t, int* start){
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

    for(int i=*start; i<min(*start+t,num); i++){
        // printf("Write a message: ");
        sendto(fd, myData[i].myIdx, (i<10?2:3), 0, (struct sockaddr*) &destination, sizeof(destination));
        sendto(fd, myData[i].myStr, len, 0, (struct sockaddr*) &destination, sizeof(destination));
        // connection_fd is marked as connected
        // and it knows where the message should be directed
        printf("%s %s\n", myData[i].myIdx, myData[i].myStr);
    }

    close(fd);
}

int receive_last_rand_str(struct myStruct** myData, int *start){
    struct sockaddr_un address;
    int fd;

    if((fd = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1){
        perror("Socket cannot be initialized!");
        exit(EXIT_FAILURE);
    }

    address.sun_family = AF_UNIX;
    memcpy(address.sun_path, LOCAL, strlen(LOCAL) + 1);

    unlink(LOCAL);
    if(bind(fd, (struct sockaddr*) &address, sizeof(address)) == 0){}
    else{
        perror("Socket cannot be bound!");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_un emitter;
    socklen_t length;

    struct myStruct *temp = (struct myStruct*) malloc(sizeof(struct myStruct));
    temp->myIdx = (char*) malloc(sizeof(char)*((*start)<10?2:3));
    temp->myStr = (char*) malloc(sizeof(char)*MAX_MESSAGE_SIZE);
    size_t size;
    // printf("Write a message: ");
    size = recvfrom(fd, temp->myIdx, ((*start)<10?2:3), 0, (struct sockaddr *) &emitter, &length);
    if(size == -1) {
        if(errno == ECONNRESET) printf("ECONNRESET\n");
        close(fd);
        perror("Receiver"); exit(EXIT_FAILURE);
    }
    size = recvfrom(fd, temp->myStr, len, 0, (struct sockaddr *) &emitter, &length);
    if(size == -1) {
        if(errno == ECONNRESET) printf("ECONNRESET\n");
        close(fd);
        perror("Receiver"); exit(EXIT_FAILURE);
    }
    // connection_fd is marked as connected
    // and it knows where the message should be directed
    printf("%s\n", temp->myIdx);
    int ans=0;
    int i = 0;
    while(temp->myIdx[i]!='\0'){
        ans *= 10;
        ans += (int)((temp->myIdx[i]-'0'));
        i++;
    }
    int r_val = ans;

    free(temp);
    close(fd);
    return r_val;
}

int main(int argc, const char* argv[]){
    struct myStruct* myData;

    generate_n_rand_str(&myData) ;

    int start = 0;
    while(start<num){
        printf("Sent data:\n");
        send_t_rand_str(myData, 5, &start);
        printf("----------------\n");
        printf("Received data:\n");
        start = receive_last_rand_str(&myData, &start);
        start++;
        printf("----------------\n");
        sleep(1);
    }

    return 0;
}