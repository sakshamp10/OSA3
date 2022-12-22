#include<stdio.h>
#include<string.h>
#include<time.h>
#include<fcntl.h>
#include<stdlib.h>
#include<errno.h>
#include<sys/stat.h>
#include <math.h>
#include<sys/types.h>
#include<unistd.h>

#define len 6
#define num 50
char* FIFO="./fifo";


struct myStruct{
    char* idx;
    char* msg;
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

void generate_strings(struct myStruct** myMsg){
    srand(time(NULL));
    *myMsg = (struct myStruct*) malloc(num*sizeof(struct myStruct));
    int curr = 0;
    for(;curr<num;curr++)
    {
        (*myMsg)[curr].msg = (char*) malloc((len)*sizeof(char));
        (*myMsg)[curr].idx = (char*) malloc(idxsize(curr)*sizeof(char));
        int_to_char(curr, &(*myMsg)[curr].idx);

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
        while( (i) <= len-2){
            (*myMsg)[curr].msg[i] = 65 + rand()%26;
            i++;
        }
        (*myMsg)[curr].msg[len-1] = '\0';
    }
}

void sender(struct myStruct* myMsg, int t, int* start){
    // Creating FIFO
    umask(0);
    if(mknod(FIFO, S_IFIFO | 0660, 0) == 0){
        printf("FIFO created successfully\n");
    }
    else{
        if(errno != EEXIST){
            perror("Cannot create FIFO");
            exit(EXIT_FAILURE);
        }
    }
    // Writing
    int f1 = open(FIFO, O_WRONLY);
    for(int i=*start; i<min(*start+t,num); i++){
        printf("p1 sent:%s %s\n", myMsg[i].idx, myMsg[i].msg);
        write(f1, myMsg[i].idx, idxsize(i));
        write(f1, myMsg[i].msg, len);
    }
    close(f1);

}

int receive(struct myStruct** myMsg, int *start){
    struct myStruct* temp = (struct myStruct*) malloc(sizeof(struct myStruct));
    temp->idx = (char*) malloc((idxsize(*start-1))*sizeof(char));
    temp->msg = (char*) malloc(len*sizeof(char));
    // Creating FIFO
    umask(0);
    if(mknod(FIFO, S_IFIFO | 0660, 0) != 0){
        printf("FIFO created successfully\n");

    }
    else{
        if(errno != EEXIST){
            printf("ERROR CREATING FIFO");
            exit(EXIT_FAILURE);
        }
    }

    // Reading
    int fd = open(FIFO, O_RDONLY);
    read(fd, temp->idx, (idxsize(*start-1)));
    read(fd, temp->msg, len);
    printf("p1 received max index: %s\n", temp->idx);
    close(fd);

    int ans=0;
    int i = 0;
    while(temp->idx[i]!='\0'){
        ans *= 10;
        ans += (int)((temp->idx[i]-'0'));
        i++;
    }
    return ans;

}

int main(int argc, const char* argv[]){
    struct myStruct* myMsg;
    generate_strings(&myMsg) ;

    int start = 0;
    struct timespec t1,t2;
    clock_gettime(CLOCK_REALTIME,&t1);
    while(start<num){
        sender(myMsg, 5, &start);
        sleep(0.5);
        start = receive(&myMsg, &start);
        start++;
        printf("\n\n");
        sleep(0.5);
    }
    clock_gettime(CLOCK_REALTIME,&t2);
    printf("time taken in FIFO mode= %lf\n", fabs(((t2.tv_sec-t1.tv_sec)+(t2.tv_nsec-t1.tv_nsec)/1e9)));

    return 0;
}
