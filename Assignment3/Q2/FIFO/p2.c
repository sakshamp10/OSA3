#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<errno.h>
#include<stdlib.h>
#include<time.h>

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
    char* idx;
    char* str;
};

void receive(struct myStruct** myData, int* start){
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
    while(read(fd, (*myData)[*start].idx, (idxsize(*start)))>0){
		read(fd, (*myData)[*start].str, len);
		printf("p2 got:%s %s\n", (*myData)[*start].idx, (*myData)[*start].str);
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
	write(f1, myData[*start-1].idx, (idxsize(*start-1)));
	write(f1, myData[*start-1].str, len);
	printf("p2 sent max index: %s\n", myData[*start-1].idx);
	close(f1);
}

int main(int argc, const char* argv[]){

    struct myStruct *myData = (struct myStruct*) malloc(num*sizeof(struct myStruct));
	
	for (int i = 0; i < num; i++){
        myData[i].str = (char*) malloc((len)*sizeof(char));
        myData[i].idx = (char*) malloc((idxsize(i))*sizeof(char));
	}
	
	int start=0;
	while(start<num){
		receive(&myData, &start);
		sleep(0.5);
		send_last_rand_str(myData, &start);
        printf("\n\n");
		sleep(0.5);
	}

    return 0;
}
