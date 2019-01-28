#include"functions.h"
#include"structs.h"

#define SHMKEY (key_t)7777
#define SEMKEY (key_t)4444
#define PERMS 0600




int main(int argc,char*argv[]){
	shared*shm;
	int semshared,indsempty,indsfull,outdsempty,outdsfull; 		//semaphores
	int semread; 
	
	srand(time(NULL));
	int lines = 0;
	int producers,steps;
	int i,status,k;
	int mainpid = getpid();
	
	//////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////COUNT THE LINES OF FILE/////////////////////////////////////////
	FILE*fp;
	char line[128];
		
	fp=fopen("ListOfHarryPotterSpells.txt","r");
	if(fp==NULL){
		fprintf(stderr,"Can't open file!\n");
		exit(1);
	}
	while(fgets(line,128,fp)){
		lines++;
	}
	
	////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////SHARED MEMORY CREATION,INITIALIZATION AND ATTACHMENT///////////
	
	int shmid;
	
	shmid=shmget(SHMKEY,sizeof(shared*),PERMS|IPC_CREAT); //create the shared memory.
    	if(shmid <0){
		printf("shmget: shmget failed\n"); //if get fails, end the program.
	  	return -1;
   	}
   	
   	shm = (shared*)shmat(shmid,(shared*)0,0); //attach on the shared memory
	if(shm == (shared*)-1){printf("error: shmat failed...\n");return -1;}
	
	for(k=0;k<10;k++){ //initialize the shared memory
		(shm->inds)[k].useful=0;
		(shm->outds)[k].useful=0;
	}
	shm->pidmatch=0;
	
	printf("Loading...\n");
	sleep(1);
	////////////////////////////////////////////////////////////////////////////////////////
	////////////////SEMAPHORE CREATION,INITIALIZATION AND ATTACHMENT////////////////////////
	
	//attachment
	semshared=semget(SEMKEY+1,1,PERMS|IPC_CREAT); //create the semaphores.
   	if(semshared == -1){
		printf("semget: semget failed\n"); //if get fails, end the program.
		return -1;
   	}

   	semread=semget(SEMKEY+2,1,PERMS|IPC_CREAT);
   	if(semread == -1){
		printf("semget: semget failed\n"); //if get fails, end the program.
		return -1;
  	 }
  	 
  	indsempty=semget(SEMKEY+3,1,PERMS|IPC_CREAT); //create the semaphores.
   	if(indsempty == -1){
		printf("semget: semget failed\n"); //if get fails, end the program.
		return -1;
   	}

   	indsfull=semget(SEMKEY+4,1,PERMS|IPC_CREAT);
   	if(indsfull == -1){
		printf("semget: semget failed\n"); //if get fails, end the program.
		return -1;
  	 }
  	 
  	 outdsempty=semget(SEMKEY+5,1,PERMS|IPC_CREAT); //create the semaphores.
   	if(outdsempty == -1){
		printf("semget: semget failed\n"); //if get fails, end the program.
		return -1;
   	}

   	outdsfull=semget(SEMKEY+6,1,PERMS|IPC_CREAT);
   	if(outdsfull == -1){
		printf("semget: semget failed\n"); //if get fails, end the program.
		return -1;
  	 }
	
	//initialization
	union semun{
		int val;
		struct semid_ds *buf;
		ushort *array;
   	}arg;
   	arg.val=1;

   	int u = semctl(semshared, 0, SETVAL, arg.val); 
   	if (u == -1) { 
		printf("semctl: semctl failed\n"); 
  	 return -1; 
   	}
   
   	u = semctl(semread, 0, SETVAL, arg.val); 
   	if (u == -1) { 
		printf("semctl: semctl failed\n"); 
   	return -1; 
   	}
   	
   	arg.val = 10;
   	
   	u = semctl(indsempty, 0, SETVAL, arg.val); 
   	if (u == -1) { 
		printf("semctl: semctl failed\n"); 
  	 return -1; 
   	}
   	
   	u = semctl(outdsempty, 0, SETVAL, arg.val); 
   	if (u == -1) { 
		printf("semctl: semctl failed\n"); 
  	 return -1; 
   	}
   
  	arg.val = 0;
   
   	u = semctl(indsfull, 0, SETVAL, arg.val); 
   	if (u == -1) { 
		printf("semctl: semctl failed\n"); 
   	return -1; 
   	}
   	
   	u = semctl(outdsfull, 0, SETVAL, arg.val); 
   	if (u == -1) { 
		printf("semctl: semctl failed\n"); 
   	return -1; 
   	}
   	
   	
   	
	///////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////READ FROM COMMAND LINE////////////////////////////////////
	if(argc!=3){
		printf("You have to give 2 arguments: Number of producers and number of steps.\n");
		return 1;
	}
	else{
	
		producers = atoi(argv[1]);
		steps = atoi(argv[2]);
		
		if(producers<=0){
			printf("You gave a negative or 0 amount of producers.\n");
			return 1;
		}
		if(steps<=0){
			printf("You gave a negative or 0 amount of steps.\n");
			return 1;
		}
		
	}
	///////////////////////////////////////////////////////////////////////////////////////
	//////////////////// CREATION OF PROCESSES/////////////////////////////////////////////
	
	int c;
	int * pids = malloc((producers+1)*sizeof(int));
	
	for(i=0;i<producers+1;i++){
		c=fork();
		if(c==0){
			pids[i]=getpid();
			break;
		}
		else{
			pids[i]=c;
			
		}
	}

	
	//////////////////////////////////////////////////////////////////////////////////////
	//////////////////////SORT THE PROCESSES//////////////////////////////////////////////
	
	int pr = getpid();
	
	if(pr == pids[0]){ 		//CONSUMER PROCESS
		
		int k =0;
		int termcount=0;
		
		while(1){
			
			////////////////////READ PHASE
			//////////////////////////////
			down(&indsfull);
			down(&semshared);
			for(i=0;i<10;i++){
				if(shm->inds[i].useful==1){
				
					break;
				}
			}
		
			protasi p = shm->inds[i];
			
			
		
			for(i=1;i<10;i++){
				shm->inds[i-1]=shm->inds[i];
		
			}
			shm->inds[9].useful=0;
			k =0;
			
			while(p.line[k]){
      			p.line[k]=toupper(p.line[k]);
      			k++;
   			}
			
			
			up(&semshared);
			up(&indsempty);
			
			if(strcmp(p.line,"TERM\0")==0){
				termcount++;
				if(termcount==producers){
					break;
				}
				continue;
			}
			
		
			/////////////////////////WRITEPHASE
			///////////////////////////////////
		
			if(steps!=0){
			
				down(&outdsempty);
				down(&semshared);
				
				for(i=0;i<10;i++){
					if(shm->outds[i].useful==0){
						break;
					}
				}
			
				shm->outds[i] = p;
			
		
				up(&semshared);
				up(&outdsfull);
				steps--;
			
			}else{
		
				down(&outdsempty);
				down(&semshared);
				for(i=0;i<10;i++){
					if(shm->outds[i].useful==0){
						break;
					}
				}
				strcpy(p.line,"TERM\0");
				shm->outds[i] = p;
			
		
				up(&semshared);
				up(&outdsfull);
		
			}
			
		
		}
		
		
		
		
	}
	else if (pr==mainpid){		//MAIN PROCESS
	
		for(i=0;i<producers+1;i++){
			waitpid(pids[i],&status,0);
			if(WIFEXITED(status)){
				printf("%d : Exited normally\n",pids[i]);
				
			}
		}
		
		printf("Number of Producers : %d\n",producers);
		printf("Number of Steps     : %d\n",steps);
		printf("Pid Matches         : %d\n",shm->pidmatch);
		
	}
	
	else{		//PRODUCER PROCESS
		int flag=0;
		int j;
		
		while(1){
			/////////////////////////WRITE PHASE
			
			fseek(fp, 0, SEEK_SET);		
			int r = rand()%lines+0;
			
			down(&semread);
			for(i=0;i<r;i++){
				fgets(line,128,fp);
			}
			fgets(line,128,fp);
			
			removeallchars(line,'\n');
			
			up(&semread);				//

			protasi p;
			p.useful = 1;
			p.procid = getpid();
			strcpy(p.line,line);
			
			
			down(&indsempty);
			down(&semshared);
			for(i=0;i<10;i++){
				if(shm->inds[i].useful==0){
					break;
				}
			}
			
			shm->inds[i] = p;
			
			up(&semshared);
			up(&indsfull);
			
			
			////////////////////////////////////////
			//////////////////////////////READ PHASE
			
			
			down(&outdsfull);
			down(&semshared);
			
			for(i=0;i<10;i++){
				if(shm->outds[i].useful==1){
					break;
				}
			}
			p = shm->outds[i];
			
			
			for(i=1;i<10;i++){
				shm->outds[i-1]=shm->outds[i];
			}
			shm->outds[9].useful=0;
			
			up(&semshared);
			up(&outdsempty);
			
			if(strcmp(p.line,"TERM")==0){
				down(&indsempty);
				down(&semshared);
				for(i=0;i<10;i++){
					if(shm->inds[i].useful==0){
						break;
					}
				}
				
				shm->inds[i] = p;
			
				up(&semshared);
				up(&indsfull);
				break;
			}
			
			if(p.procid == getpid()){
				down(&semshared);
				shm->pidmatch++;
				up(&semshared);
				printf("%s\n", p.line);
			}
			
			
			
		}
		
	}
	

	
	return 0;
}
