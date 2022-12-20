#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<fcntl.h>
#include<errno.h>
#include<unistd.h>

#define PIPE_NAME "./myFifo"

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

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

int char_to_int(char** ptr){
    int ans=0;
    int i = 0;
    while((*ptr)[i]!='\0'){
        ans *= 10;
        ans += (int)((*ptr)[i]-'0');
        i++;
    }
    return ans;
}

void generate_n_rand_str(struct myStruct** myData, int n, int l){
    srand(time(NULL));
    *myData = (struct myStruct*) malloc(n*sizeof(struct myStruct));
    int curr = 0;
    while(curr<n){
        (*myData)[curr].myStr = (char*) malloc((l)*sizeof(char));
        (*myData)[curr].myIdx = (char*) malloc((curr<10?2:3)*sizeof(char));
        int_to_char(curr, &(*myData)[curr].myIdx);

        for(int i = 0; i <= l-2; i++){
            (*myData)[curr].myStr[i] = 33 + rand()%62;
        }
        (*myData)[curr].myStr[l-1] = '\0';
        curr++;
    }
}

void send_t_rand_str(struct myStruct* myData, int n, int l, int t, int* start){
    // Creating FIFO
    umask(0);
    if(mknod(PIPE_NAME, S_IFIFO | 0660, 0) == -1){
        if(errno != EEXIST){
            perror("Cannot create FIFO");
            exit(EXIT_FAILURE);
        }
        // else{
        // 	printf("Using existing FIFO\n");
        // }
    }
    else{
        printf("FIFO created successfully\n");
    }
    // printf("%d %d \n", *start, t);
    // Writing
    int fd = open(PIPE_NAME, O_WRONLY);
    for(int i=*start; i<MIN(*start+t,n); i++){
        printf("%s %s\n", myData[i].myIdx, myData[i].myStr);
        write(fd, myData[i].myIdx, (i<10?2:3));
        write(fd, myData[i].myStr, l);
    }
    close(fd);
}

int receive_last_rand_str(struct myStruct** myData, int n, int l, int *start){
    struct myStruct* temp = (struct myStruct*) malloc(sizeof(struct myStruct));
    temp->myIdx = (char*) malloc(((*start-1)<10?2:3)*sizeof(char));
    temp->myStr = (char*) malloc(l*sizeof(char));
    // Creating FIFO
    umask(0);
    if(mknod(PIPE_NAME, S_IFIFO | 0660, 0) == -1){
        if(errno != EEXIST){
            perror("Cannot create FIFO");
            exit(EXIT_FAILURE);
        }
        // else{
        // 	printf("Using existing FIFO\n");
        // }
    }
    else{
        printf("FIFO created successfully\n");
    }
    // printf("%d %d \n", *start, t);
    // Reading
    int fd = open(PIPE_NAME, O_RDONLY);
    read(fd, temp->myIdx, ((*start-1)<10?2:3));
    read(fd, temp->myStr, l);
    // printf("%s %s\n", temp->myIdx, temp->myStr);
    printf("%s\n", temp->myIdx);
    close(fd);
    return char_to_int(&temp->myIdx);
}

int main(int argc, const char* argv[]){
    struct myStruct* myData;
    int num_of_rand_str = 50;
    int len_of_rand_str = 6;
    generate_n_rand_str(&myData, num_of_rand_str, len_of_rand_str) ;
    // printf("ok\n");
    // for(int i=0; i<num_of_rand_str; i++){
    //     printf("%s %s\n", myData[i].myIdx, myData[i].myStr);
    // }
    // printf("ok\n");

    int start = 0;
    while(start<num_of_rand_str){
        printf("Sent data:\n");
        send_t_rand_str(myData, num_of_rand_str, len_of_rand_str, 5, &start);
        printf("----------------\n");
        sleep(1);
        printf("Received data:\n");
        start = receive_last_rand_str(&myData, num_of_rand_str, len_of_rand_str, &start)+1;
        printf("----------------\n");
        sleep(0.5);
    }

    return 0;
}