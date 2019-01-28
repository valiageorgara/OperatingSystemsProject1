#include<stdio.h>
#include<stdlib.h>
#include<sys/shm.h>
#include<sys/types.h>
#include<sys/sem.h>
#include<sys/ipc.h>
#include<unistd.h>
#include <time.h>
#include<string.h>


#define SHMKEY (key_t)7777
#define SEMKEY (key_t)4444
#define PERMS 0600

int up(int*semaphore){

   struct sembuf up[1]={0,1,0};

   if(semop(*semaphore,&up[0],1)==-1){  //up the semaphore.
		printf("semop: semop failed\n"); //if semop fails, end the program.
		return -1;
   }
   return 0;
}

int down(int*semaphore){
   
   struct sembuf down[1]={0,-1,0}; //down operation.

   if(semop(*semaphore,&down[0],1)==-1){  //down the semaphore.
		printf("semop: semop failed\n"); //if semop fails, end the program.
		return -1;
   }
   return 0;

}

void removeallchars(char* str, char c) {
    char *pr = str, *pw = str;
    while (*pr) {
        *pw = *pr++;
        pw += (*pw != c);
    }
    *pw = '\0';
}
