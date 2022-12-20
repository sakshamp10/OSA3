
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


void generate_n_rand_str(struct myStruct** myData){
    srand(time(NULL));
    *myData = (struct myStruct*) malloc(num*sizeof(struct myStruct));
    int curr = 0;
    for(curr=0;curr<num;curr++)
    {
        (*myData)[curr].myStr = (char*) malloc((len)*sizeof(char));
        (*myData)[curr].myIdx = (char*) malloc(idxsize(curr)*sizeof(char));
//        int_to_char(curr, &(*myData)[curr].myIdx);

        if(curr>=10){
            (*myData)[curr].myIdx[0] = '0'+curr/10;
            (*myData)[curr].myIdx[1] = '0'+curr%10;
            (*myData)[curr].myIdx[2] ='\0';
        }
        else{
            (*myData)[curr].myIdx[0] = '0'+curr;
            (*myData)[curr].myIdx[1] = '\0';
        }


        int i=0;
        while(i<len-1)
        {
            (*myData)[curr].myStr[i] = 65 + rand()%26;
            i++;
        }
        (*myData)[curr].myStr[len-1] = '\0';
//        curr++;
    }
}

void send_t_rand_str(struct myStruct* myData, int t, int* start){
    struct sockaddr_un address;
    int fd;

    address.sun_family = AF_UNIX;
    memcpy(address.sun_path, LOCAL, strlen(LOCAL) + 1);

    if((fd = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1){
        perror("Socket cannot be initialized!1");
        exit(EXIT_FAILURE);
    }

    unlink(LOCAL);
    if(bind(fd, (struct sockaddr*) &address, sizeof(address)) == 0){}
    else{
        perror("Socket cannot be bound!");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_un destination;
    destination.sun_family = AF_UNIX;
    memcpy(destination.sun_path, DESTINATION, strlen(DESTINATION) + 1);
    int i=*start;
    while(i<min(*start+t,num))
    {
        // printf("Write a message: ");
        sendto(fd, myData[i].myIdx, (idxsize(i)), 0, (struct sockaddr*) &destination, sizeof(destination));
        sendto(fd, myData[i].myStr, len, 0, (struct sockaddr*) &destination, sizeof(destination));
        // connection_fd is marked as connected
        // and it knows where the message should be directed
        printf("%s %s\n", myData[i].myIdx, myData[i].myStr);
        i++;
    }

    close(fd);
}

int receive_last_rand_str(struct myStruct** myData, int *start){
    printf("%d",1);
    struct sockaddr_un address;
    int fd;
    fd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if(fd != -1){}
    else{
        printf("Socket cannot be initialized!\n");
        exit(EXIT_FAILURE);
    }
    printf("%d",2);
    address.sun_family = AF_UNIX;
    memcpy(address.sun_path, LOCAL, strlen(LOCAL) + 1);

    unlink(LOCAL);
    if(bind(fd, (struct sockaddr*) &address, sizeof(address)) == 0){}
    else{
        perror("Socket cannot be bound!");
        exit(EXIT_FAILURE);
    }

    printf("%d",3);


    struct sockaddr_un emitter;
    socklen_t length;

    struct myStruct *temp = (struct myStruct*) malloc(sizeof(struct myStruct));
    temp->myIdx = (char*) malloc(sizeof(char)*(idxsize(*start)));
    temp->myStr = (char*) malloc(sizeof(char)*MAX_MESSAGE_SIZE);
    size_t size;

    printf("%d",4);

    // printf("Write a message: ");
    size = recvfrom(fd, temp->myIdx, (idxsize(*start)), 0, (struct sockaddr *) &emitter, &length);
    if(size == -1) {
        if(errno == ECONNRESET) printf("ECONNRESET\n");
        close(fd);
        perror("Receiver"); exit(EXIT_FAILURE);
    }
    printf("%d",5);

    size = recvfrom(fd, temp->myStr, len, 0, (struct sockaddr *) &emitter, &length);
    if(size == -1) {
        if(errno == ECONNRESET) printf("ECONNRESET\n");
        close(fd);
        perror("Receiver"); exit(EXIT_FAILURE);
    }
    printf("%d",6);

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
    int r_val=ans;
    free(temp);
    close(fd);

    printf("%d",7);

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
        printf("Start\n");
        start = receive_last_rand_str(&myData, &start);
        start++;
        printf("----------------\n");
        sleep(1);
    }

    return 0;
}