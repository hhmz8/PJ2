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
#define BUF_SIZE 1024
#define SHM_KEY 806040

extern int errno;

int main(int argc, char** argv) {
	
	int option = 0;
	int nlicenses = 0;

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
		nlicenses = atoi(argv[optind]);
	}
	else {
		nlicenses = 1;
	}
	
	// Shared memory
	int shmid = shmget(SHM_KEY, BUF_SIZE, 0666|IPC_CREAT);
	if (shmid == -1) {
		perror("Error: shmget");
		return -1;
	}
	
	char* str = (char*) shmat(shmid, 0, 0);
	if (shmdt(str) == -1) {
		perror("Error: shmdt");
		return -1;
	}
	if (shmctl(shmid, IPC_RMID, 0) == -1) {
		perror("Error: shmctl");
		return -1;
	}
	
	// Fork
	int pid = fork();
	switch ( pid )
    {
	case -1:
	    perror("Error: fork");
	    return -1;

	case 0:
	    child();
	    break;

	default:
	    parent();
	    break;
    }

	return 0;
}

void sigint(int sig){
	printf("Child exited.\n");
	exit(0);
}

void parent(){
	printf("End of parent.\n");
}

// Reference: http://www.cs.umsl.edu/~sanjiv/classes/cs4760/src/shm.c
// Reference: https://www.geeksforgeeks.org/signals-c-set-2/
void child(){
	signal(SIGINT, sigint);
	sleep ( 2 );
    int shmid = shmget(SHM_KEY, BUF_SIZE, 0666|IPC_CREAT);
	if (shmid == -1) {
		perror("Error: shmget");
		exit(-1);
	}

	char* str = (char*) shmat(shmid, 0, 0);

	printf("End of child.\n");
}