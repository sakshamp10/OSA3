#include<math.h>
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

struct message{
    char* idx;
    char* msg;
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


void generate_strings(struct message** myMsg){
    srand(time(NULL));
    *myMsg = (struct message*) malloc(num*sizeof(struct message));
    int curr = 0;
    for(curr=0;curr<num;curr++)
    {
        (*myMsg)[curr].msg = (char*) malloc((len)*sizeof(char));
        (*myMsg)[curr].idx = (char*) malloc(idxsize(curr)*sizeof(char));
//        int_to_char(curr, &(*myMsg)[curr].idx);

        if(curr>=10){
            (*myMsg)[curr].idx[0] = '0'+curr/10;
            (*myMsg)[curr].idx[1] = '0'+curr%10;
            (*myMsg)[curr].idx[2] ='\0';
        }
        else{
            (*myMsg)[curr].idx[0] = '0'+curr;
            (*myMsg)[curr].idx[1] = '\0';
        }


        int i=0;
        while(i<len-1)
        {
            (*myMsg)[curr].msg[i] = 65 + rand()%26;
            i++;
        }
        (*myMsg)[curr].msg[len-1] = '\0';
//        curr++;
    }
}

void send(struct message* myMsg, int t, int* start){
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
        sendto(fd, myMsg[i].idx, (idxsize(i)), 0, (struct sockaddr*) &destination, sizeof(destination));
        sendto(fd, myMsg[i].msg, len, 0, (struct sockaddr*) &destination, sizeof(destination));
        // connection_fd is marked as connected
        // and it knows where the message should be directed
        printf("%s %s\n", myMsg[i].idx, myMsg[i].msg);
        i++;
    }

    close(fd);
}

int receive(struct message** myMsg, int *start){
    struct sockaddr_un address;
    int fd;
    fd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if(fd != -1){}
    else{
        printf("Socket cannot be initialized!\n");
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

    struct message *temp = (struct message*) malloc(sizeof(struct message));
    temp->idx = (char*) malloc(sizeof(char)*(idxsize(*start)));
    temp->msg = (char*) malloc(sizeof(char)*MAX_MESSAGE_SIZE);
    size_t size;


    // printf("Write a message: ");
    size = recvfrom(fd, temp->idx, (idxsize(*start)), 0, (struct sockaddr *) &emitter, &length);
    if(size != -1) {}
    else{
        if(errno == ECONNRESET) printf("ECONNRESET\n");
        close(fd);
        perror("Receiver"); exit(EXIT_FAILURE);
    }

    size = recvfrom(fd, temp->msg, len, 0, (struct sockaddr *) &emitter, &length);
    if(size != -1) {}
    else{
        if(errno == ECONNRESET) printf("ECONNRESET\n");
        close(fd);
        perror("Receiver"); exit(EXIT_FAILURE);
    }

    // connection_fd is marked as connected
    // and it knows where the message should be directed
    printf("%s\n", temp->idx);
    int ans=0;
    int i = 0;
    while(temp->idx[i]!='\0'){
        ans *= 10;
        ans += (int)((temp->idx[i]-'0'));
        i++;
    }
    int r_val=ans;
    free(temp);
    close(fd);

    return r_val;
}

int main(int argc, const char* argv[]){
    struct message* myMsg;

    generate_strings(&myMsg) ;

    int start = 0;
    struct timespec t1,t2;
    clock_gettime(CLOCK_REALTIME,&t1);
    while(start<num){
        printf("Sent data:\n");
        send(myMsg, 5, &start);
        printf("----------------\n");
        printf("Received data:\n");
        start = receive(&myMsg, &start);
        start++;
        printf("----------------\n");
        sleep(1);
    }
    clock_gettime(CLOCK_REALTIME,&t2);
    printf("time taken in socket mode= %lf\n", fabs(((t2.tv_sec-t1.tv_sec)+(t2.tv_nsec-t1.tv_nsec)/1e9)));

    return 0;
}