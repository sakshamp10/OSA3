#include<stdio.h>
#include<stdlib.h>
#include<time.h>
int main(){
    char * a= (char*)malloc(4*sizeof(char));
    for(int i=0;i<3;i++){
        a[i]=(char)(48+1);
    }
    a[3]='\0';
    printf("string : %s",a);
    return 0;
}
