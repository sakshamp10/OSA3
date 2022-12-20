#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<fcntl.h>
#include<errno.h>
#include<unistd.h>

#define PIPE_NAME "./fifo"

struct myStruct{
    char* myIdx;
    char* myStr;
};

void receive_t_rand_str(struct myStruct** myData, int n, int l, int* start){
	// Creating the pipe
	umask(0);
	if(mknod(PIPE_NAME, S_IFIFO | 0660, 0) == -1){
		if(errno != EEXIST){
			perror("Cannot create the pipe");
			exit(EXIT_FAILURE);
		}
		// else
		// 	printf("Using existing file\n");
	}    
	else{
        printf("FIFO created successfully\n");
    }

	// Reading
	int fd = open(PIPE_NAME, O_RDONLY);
    while(read(fd, (*myData)[*start].myIdx, ((*start)<10?2:3))>0){
		read(fd, (*myData)[*start].myStr, l); 
		printf("%s %s\n", (*myData)[*start].myIdx, (*myData)[*start].myStr);
		*start=*start+1;
		if(*start>=n){
			break;
		}
    }
	close(fd);
}

void send_last_rand_str(struct myStruct* myData, int n, int l, int *start){
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
	write(fd, myData[*start-1].myIdx, ((*start-1)<10?2:3));
	write(fd, myData[*start-1].myStr, l);
	printf("%s\n", myData[*start-1].myIdx);
	// printf("%s %s\n", myData[*start-1].myIdx, myData[*start-1].myStr);
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
		receive_t_rand_str(&myData, num_of_rand_str, len_of_rand_str, &start);
        printf("----------------\n");
		sleep(0.5);
		printf("Sent data:\n");
		send_last_rand_str(myData, num_of_rand_str, len_of_rand_str, &start);
        printf("----------------\n");
		sleep(1);
	}

    return 0;
}
