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


int idxsize(int i){
    if(i>=10){
        return 3;
    }
    else return 2;
}

void generate_n_rand_str(struct myStruct** myData){
    srand(time(NULL));
    *myData = (struct myStruct*) malloc(num*sizeof(struct myStruct));
    int curr = 0;
    for(;curr<num;curr++)
    {
        (*myData)[curr].myStr = (char*) malloc((len)*sizeof(char));
        (*myData)[curr].myIdx = (char*) malloc(idxsize(curr)*sizeof(char));
        int_to_char(curr, &(*myData)[curr].myIdx);

        for(int i = 0; i <= len-2; i++){
            (*myData)[curr].myStr[i] = 33 + rand()%62;
        }
        (*myData)[curr].myStr[len-1] = '\0';
    }
}

void send_t_rand_str(struct myStruct* myData, int t, int* start){
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
    int f1 = open(FIFO, O_WRONLY);
    for(int i=*start; i<min(*start+t,num); i++){
        printf("%s %s\n", myData[i].myIdx, myData[i].myStr);
        write(f1, myData[i].myIdx, idxsize(i));
        write(f1, myData[i].myStr, len);
    }
    close(f1);

}

int receive_last_rand_str(struct myStruct** myData, int *start){
    struct myStruct* temp = (struct myStruct*) malloc(sizeof(struct myStruct));
    temp->myIdx = (char*) malloc((idxsize(*start-1))*sizeof(char));
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
    read(fd, temp->myIdx, (idxsize(*start-1)));
    read(fd, temp->myStr, len);
    printf("%s\n", temp->myIdx);
    close(fd);

    int ans=0;
    int i = 0;
    while(temp->myIdx[i]!='\0'){
        ans *= 10;
        ans += (int)((temp->myIdx[i]-'0'));
        i++;
    }
    return ans;

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
