#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<fcntl.h>
#include<errno.h>
#include<unistd.h>

#define len 6
#define num 50
char* FIFO="./fifo";


struct myStruct{
    char* myIdx;
    char* myStr;
};

int min(int a, int b){
    if(a>b){
        return b;
    }
    else{
        return a;
    }
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

void generate_n_rand_str(struct myStruct** myData){
    srand(time(NULL));
    *myData = (struct myStruct*) malloc(num*sizeof(struct myStruct));
    int curr = 0;
    for(;curr<num;curr++)
    {
        (*myData)[curr].myStr = (char*) malloc((len)*sizeof(char));
        (*myData)[curr].myIdx = (char*) malloc((curr<10?2:3)*sizeof(char));
        int_to_char(curr, &(*myData)[curr].myIdx);

        for(int i = 0; i <= len-2; i++){
            (*myData)[curr].myStr[i] = 33 + rand()%62;
        }
        (*myData)[curr].myStr[len-1] = '\0';
    }
}

void send_t_rand_str(struct myStruct* myData, int t, int* start){
    printf("%d\n",1);
    // Creating FIFO
    umask(0);
    if(mknod(FIFO, S_IFIFO | 0660, 0) == -1){
        if(errno != EEXIST){
            perror("Cannot create FIFO");
            exit(EXIT_FAILURE);
        }

    }
    else{
        printf("FIFO created successfully\n");
    }
    // Writing
    int fd = open(FIFO, O_WRONLY);
    printf("%d\n",2);
    for(int i=*start; i<min(*start+t,num); i++){
        printf("%s %s\n", myData[i].myIdx, myData[i].myStr);
        write(fd, myData[i].myIdx, (i<10?2:3));
        write(fd, myData[i].myStr, len);
    }
    close(fd);
    printf("%d\n",3);
}

int receive_last_rand_str(struct myStruct** myData, int *start){
    struct myStruct* temp = (struct myStruct*) malloc(sizeof(struct myStruct));
    temp->myIdx = (char*) malloc(((*start-1)<10?2:3)*sizeof(char));
    temp->myStr = (char*) malloc(len*sizeof(char));
    // Creating FIFO
    umask(0);
    if(mknod(FIFO, S_IFIFO | 0660, 0) == -1){
        if(errno != EEXIST){
            printf("ERROR CREATING FIFO");
            exit(EXIT_FAILURE);
        }
    }
    else{
        printf("FIFO created successfully\n");
    }

    // Reading
    int fd = open(FIFO, O_RDONLY);
    read(fd, temp->myIdx, ((*start-1)<10?2:3));
    read(fd, temp->myStr, len);
    printf("%s\n", temp->myIdx);
    close(fd);
    return char_to_int(&temp->myIdx);
}

int main(int argc, const char* argv[]){
    struct myStruct* myData;
    generate_n_rand_str(&myData) ;

    int start = 0;
    while(start<num){
        printf("Sent data(1):\n");
        send_t_rand_str(myData, 5, &start);
        printf("----------------\n");
        sleep(1);
        printf("Received data(1):\n");
        start = receive_last_rand_str(&myData, &start);
        start++;
        printf("----------------\n");
        sleep(0.5);
    }

    return 0;
}
