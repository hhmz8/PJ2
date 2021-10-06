/*
Hanzhe Huang
10/3/2021
runsim.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <time.h> //time
#include <sys/wait.h> //wait
#include <signal.h>
#include <ctype.h> //isprint
#include <unistd.h> //sleep, alarm

#include "runsim.h"

// Reference: https://www.tutorialspoint.com/inter_process_communication/inter_process_communication_shared_memory.htm
// Reference: https://stackoverflow.com/questions/19461744/how-to-make-parent-wait-for-all-child-processes-to-finish
#define BUF_SIZE 1024
#define SHM_KEY 806040
#define MAX_PRO 20
#define MAX_TIME 60
#define OUT_FILE "logfile"

extern int errno;

struct shmseg {
   int nlicenses;
   int choosing[MAX_PRO]; //Boolean
   int numbers[MAX_PRO]; //Turn #
   char buf[BUF_SIZE];
};

int main(int argc, char** argv) {
	
	FILE* fptr;
	int i;
	int id = -1;
	int pid;
	int option = 0;
	int licenseLimit = 0;
	char arg1[20];
	char arg2[20];
	char arg3[20];
	fscanf(stdin, "%s", arg1);
	fscanf(stdin, "%s", arg2);
	fscanf(stdin, "%s", arg3);
	
	// Clear log file
	fptr = fopen(OUT_FILE, "w");
	fclose(fptr);

	// Reference: https://www.gnu.org/software/libc/manual/html_node/Example-of-Getopt.html
	while ((option = getopt(argc, argv, "ht:")) != -1) {
		switch (option) {

		case '?':
			if (isprint(optopt))
				perror("Unknown option");
			else
				perror("Unknown option character");
			return -1;
			break;
		}
	}
	
	if (argc > 2) {
		perror("Too many parameters entered");
		return -1;
	}
	if (optind < argc) {
		licenseLimit = atoi(argv[optind]);
	}
	else {
		printf("Defaulting license # to 1.\n");
		licenseLimit = 1;
	}
	
	// Fork
	for (i = 0; i < licenseLimit; i++){
		pid = fork();
		id++; // Temporary process number
		fscanf(stdin, "%s", arg1);
		fscanf(stdin, "%s", arg2);
		fscanf(stdin, "%s", arg3);
		switch ( pid )
		{
		case -1:
			perror("Error: fork");
			return -1;

		case 0:
			child(id, arg1, arg2, arg3);
			break;

		default:
			break;
		}
	}
	
	parent();
	
	return 0;
}

void sigint(int sig){
	printf("Parent process %d exiting...\n",getpid());
	deallocate();
	kill(0, SIGINT);
	exit(0);
}

void sigalrm(int sig){
	printf("Program timed out.\n");
	deallocate();
	kill(0, SIGINT);
	exit(0);
}

void parent(){
	signal(SIGINT, sigint);
	signal(SIGALRM, sigalrm);
	alarm(MAX_TIME);
	
	initlicense(license());
	
	// Reference: Example 3.13 in the textbook
	// Wait for child processes to terminate
	sleep(1);
	
	printf("Waiting for child...\n");
	int childpid;
	while ((childpid = (wait(NULL))) > 0);
	printf("Stopped waiting for child.\n");
	
	//Deallocation
	deallocate();
	
	printf("End of parent.\n");
}

// Reference: http://www.cs.umsl.edu/~sanjiv/classes/cs4760/src/shm.c
// Reference: https://www.geeksforgeeks.org/signals-c-set-2/
void child(int id, char* arg1, char* arg2, char* arg3){
	printf("Child %d with id # %d forked from parent %d.\n",getpid(), id, getppid());
	
	// Shared memory
	struct shmseg *shmp;
    int shmid = shmget(SHM_KEY, BUF_SIZE, 0666|IPC_CREAT);
	if (shmid == -1) {
		perror("Error: shmget");
		exit(-1);
	}

	shmp = shmat(shmid, 0, 0);
	returnlicense(license());
	
	printf("nlicenses: %d\n", shmp->nlicenses);

	// Reference: Lecture video, https://www.geeksforgeeks.org/bakery-algorithm-in-process-synchronization/
	// Bakery algorithm 
	int i;
	int j;
	int max_number = shmp->numbers[0];
	shmp->choosing[id] = 1;
	for (i = 0; i < MAX_PRO; i++) {
		if (shmp->numbers[i] > max_number){
			max_number = shmp->numbers[i];
		}
	}
	shmp->numbers[id] = max_number + 1;
	shmp->choosing[id] = 0;
	for (j = 0; j < MAX_PRO; j++) {
		//printf("Child %d with # of %d, testing process %d with # of %d. While: %d \n",getpid(), shmp->numbers[id], j, shmp->numbers[j], (shmp->numbers[j] < shmp->numbers[id]));
		while (shmp->choosing[j] == 1);
		while ((shmp->numbers[j] != 0) && (shmp->numbers[j] < shmp->numbers[id] || (shmp->numbers[j] == shmp->numbers[id] && j < id)));
	}
	
	// Critical Section
	docommand(arg1, arg2, arg3);
	// End 
	shmp->numbers[id] = 0;
	
	printf("Child %d finished.\n", getpid());
	exit(0);
}

void deallocate(){
	struct shmseg *shmp;
    int shmid = shmget(SHM_KEY, BUF_SIZE, 0666|IPC_CREAT);
	if (shmid == -1) {
		perror("Error: shmget");
		exit(-1);
	}
	shmp = shmat(shmid, 0, 0);
	
	if (shmdt(shmp) == -1) {
		perror("Error: shmdt");
		exit(-1);
	}
	if (shmctl(shmid, IPC_RMID, 0) == -1) {
		perror("Error: shmctl");
		exit(-1);
	}
	printf("Shared memory deallocated.\n");
}

struct shmseg* license(){
	struct shmseg *shmp;
    int shmid = shmget(SHM_KEY, BUF_SIZE, 0666|IPC_CREAT);
	if (shmid == -1) {
		perror("Error: shmget");
		exit(-1);
	}
	shmp = shmat(shmid, 0, 0);
	return shmp;
}

void getlicense(struct shmseg* shmp){
	while(shmp->nlicenses < 1);
}

void returnlicense(struct shmseg* shmp){
	shmp->nlicenses++;
}

void initlicense(struct shmseg* shmp){
	shmp->nlicenses = 0;
	int i;
	for (i = 0; i < MAX_PRO; i++) {
		shmp->choosing[i] = 0;
		shmp->numbers[i] = 0;
	}
}

void addtolicenses(struct shmseg* shmp, int n){
	shmp->nlicenses += n;
}

void removelicenses(struct shmseg* shmp, int n){
	shmp->nlicenses -= n;
}

void docommand(char* arg1, char* arg2, char* arg3){
	
	int pid;
	int childpid;
	
	pid = fork();
	switch ( pid )
	{
	case -1:
		perror("Error: fork");
		exit(-1);

	case 0:
		if ((execl(arg1, "docommand", arg2, arg3, (char*)NULL)) == -1){
			perror("Error: execl");
		}
		exit(-1);
		break;

	default:
		break;
	}
	
	while ((childpid = (wait(NULL))) > 0);
}