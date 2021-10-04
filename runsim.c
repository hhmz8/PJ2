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
#include <signal.h>
#include <ctype.h> //isprint
#include <unistd.h> //sleep

// Reference: https://www.tutorialspoint.com/inter_process_communication/inter_process_communication_shared_memory.htm
// Reference: https://stackoverflow.com/questions/19461744/how-to-make-parent-wait-for-all-child-processes-to-finish
#define BUF_SIZE 1024
#define SHM_KEY 806040
#define MAX_PRO 20

extern int errno;

struct shmseg {
   int nlicenses;
   char buf[BUF_SIZE];
};

int main(int argc, char** argv) {
	
	int pid;
	int status = 0;
	int option = 0;
	int licenseInput = 0;

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
		licenseInput = atoi(argv[optind]);
	}
	else {
		licenseInput = 1;
	}
	
	// Fork
	pid = fork();
	switch ( pid )
    {
	case -1:
	    perror("Error: fork");
	    return -1;

	case 0:
	    child();
	    break;

	default:
	    parent(pid, status);
	    break;
    }

	return 0;
}

void sigint(int sig){
	printf("Child exited.\n");
	exit(0);
}

void parent(int pid, int status){
	// Shared memory
	struct shmseg *shmp;
	int shmid = shmget(SHM_KEY, BUF_SIZE, 0666|IPC_CREAT);
	if (shmid == -1) {
		perror("Error: shmget");
		return -1;
	}
	
	shmp = shmat(shmid, 0, 0);
	if (shmp == (void *) -1){
		perror("Error: shmat");
		return -1;
	}
	
	shmp->nlicenses = 1;
	
	// Wait for child processes to terminate
	while ((pid = wait(&status)) > 0);
	
	//Deallocation
	if (shmdt(shmp) == -1) {
		perror("Error: shmdt");
		return -1;
	}
	if (shmctl(shmid, IPC_RMID, 0) == -1) {
		perror("Error: shmctl");
		return -1;
	}
	
	
	printf("End of parent.\n");
}

// Reference: http://www.cs.umsl.edu/~sanjiv/classes/cs4760/src/shm.c
// Reference: https://www.geeksforgeeks.org/signals-c-set-2/
void child(){
	signal(SIGINT, sigint);
	sleep(1);
	
	// Shared memory
	struct shmseg *shmp;
    int shmid = shmget(SHM_KEY, BUF_SIZE, 0666|IPC_CREAT);
	if (shmid == -1) {
		perror("Error: shmget");
		exit(-1);
	}

	shmp = shmat(shmid, 0, 0);
	if (shmp == (void *) -1){
		perror("Error: shmat");
		exit(-1);
	}
	
	printf("nlicenses: %d\n", shmp->nlicenses);

	printf("End of child.\n");
	exit(0);
}