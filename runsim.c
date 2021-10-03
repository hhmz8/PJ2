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
#include <ctype.h> //isprint
#include <unistd.h> //sleep

// Reference: https://www.tutorialspoint.com/inter_process_communication/inter_process_communication_shared_memory.htm
#define BUF_SIZE 1024
#define SHM_KEY 806040

extern int errno;

int main(int argc, char** argv) {

	int option = 0;
	char* filename = malloc(200);

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
		filename = argv[optind];
	}
	else {
		printf("Enter file name: \n");
		scanf("%s",filename);
	}

	printf("End of program.\n");
	return 0;

}