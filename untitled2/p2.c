#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<fcntl.h>
#include<errno.h>
#include<unistd.h>

char* FIFO = "./fifo";
#define len 6
#define num 50

int idxsize(int i){
    if(i>=10){
        return 3;
    }
    else return 2;
}

struct myStruct{
    char* myIdx;
    char* myStr;
};

void receive_t_rand_str(struct myStruct** myData, int* start){
	// Creating the pipe
	umask(0);
	if(mknod(FIFO, S_IFIFO | 0660, 0) == 0){
        printf("FIFO created successfully\n");
	}    
	else{
        if(errno != EEXIST){
            printf("Cannot create the pipe");
            exit(EXIT_FAILURE);
        }
    }

	// Reading
	int fd = open(FIFO, O_RDONLY);
    while(read(fd, (*myData)[*start].myIdx, (idxsize(*start)))>0){
		read(fd, (*myData)[*start].myStr, len);
		printf("p2 received: %s %s\n", (*myData)[*start].myIdx, (*myData)[*start].myStr);
		*start=*start+1;
		if(*start<num){
			continue;
		}else{
            break;
        }
    }
	close(fd);
}

void send_last_rand_str(struct myStruct* myData, int *start){
	// Creating FIFO
	umask(0);
	if(mknod(FIFO, S_IFIFO | 0660, 0) == 0){
        printf("FIFO created successfully\n");
    }
    else{
        if(errno != EEXIST){
            printf("Error creating FIFO");
            exit(EXIT_FAILURE);
        }
    }
	// Writing
	int f1 = open(FIFO, O_WRONLY);
	write(f1, myData[*start-1].myIdx, (idxsize(*start-1)));
	write(f1, myData[*start-1].myStr, len);
	printf("p2 sent: %s\n", myData[*start-1].myIdx);
	close(f1);
}

int main(int argc, const char* argv[]){

    struct myStruct *myData = (struct myStruct*) malloc(num*sizeof(struct myStruct));
	
	for (int i = 0; i < num; i++){
        myData[i].myStr = (char*) malloc((len)*sizeof(char));
        myData[i].myIdx = (char*) malloc((idxsize(i))*sizeof(char));
	}
	
	int start=0;
	while(start<num){
        printf("Received data:\n");
		receive_t_rand_str(&myData, &start);
        printf("\n");
		sleep(0.5);
		printf("Sent data:\n");
		send_last_rand_str(myData, &start);
        printf("\n\n");
		sleep(1);
	}

    return 0;
}
