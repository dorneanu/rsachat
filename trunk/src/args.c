/*
 * args.c
 *
 *  Created on: Nov 17, 2011
 *      Author: victor
 */
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include "RSAChat.h"
#include "args.h"
#include "db.h"


void print_banner(char *prog) {
    printf("%s- Secure RSA based chat (c) by %s\n", prog, D_CONFIG_AUTHOR);
}


static void print_syntax(char *prog) {
	print_banner(prog);

	// Print usage syntax
    printf("Usage: %s [arguments]\n", prog);
    printf(" -h 	\t\t print this message\n");
    printf(" -a     \t\t print all available keys\n");
    printf(" -i <nick> \t\t Create a fresh RSA key pair and store it in\n");
    printf("           \t\t $HOME/.chatstore under the given nickname\n\n");
    printf(" -c <host> <port> \t Open secure connection to <host> on <port>\n");
    printf("                  \t Example: -c 'localhost 3300' \n\n");
    printf(" -l <port>        \t Listen on all local interfaces on the given\n");
    printf("               \t\t port and wait for secure connections.\n\n");
    printf(" -r <nick>  \t\t Change user's nick to the given one\n");
    printf("             \t\t without changing the public key\n\n");
    printf(" -v          \t\t Print version\n");
}


/**
 * Parse arguments and return own arguments structure
 *
 * @param argc number or arguments
 * @param **argv pointers to arguments
 * @return pointer to prog_args structure
 */
prog_args *parse_args(int argc, char **argv) {
	int opt;
	prog_args *args;
	char *saveptr1;

	// Check if any arguments ...
	if (argc == 1) {
		print_syntax(argv[0]);
		exit(EXIT_FAILURE);
	}

	// Allocate memory for arguments structure
	if ((args = (prog_args*)malloc(sizeof(prog_args))) == NULL)
		fprintf(stderr, "Error: Allocate memory!");

	// Parse arguments
	while ((opt = getopt(argc, argv, "ahi:c:l:r:v")) != -1) {
		switch (opt) {
			case 'h':
				print_syntax(argv[0]);
				exit(EXIT_SUCCESS);

			case 'v':
				print_banner(argv[0]);
				printf("Version: %s\n", D_CONFIG_VERSION);
				exit(EXIT_SUCCESS);

			case 'a':
				args->f_list_keys = 1;
				break;

			case 'i':
				args->new_nick = optarg;
				break;

			case 'c':
				if (optarg) {
					args->host = strtok_r(optarg, " ", &saveptr1);
					args->port = atoi(strtok_r(NULL, " ", &saveptr1));
				}
				break;

			case 'l':
				args->f_listen = 1;
				args->port = atoi(optarg);
				break;

			case 'r':
				if (optarg)
					args->new_nick = optarg;
				break;

			case '?':
				fprintf(stderr, "Error - No such option: `%c'\n\n", optopt);
				print_syntax(argv[0]);
				exit(EXIT_FAILURE);
		}
	}

	return args;
}



