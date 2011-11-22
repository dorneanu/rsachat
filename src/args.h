/*
 * args.h
 *
 *  Created on: Nov 22, 2011
 *      Author: victor
 */

#ifndef ARGS_H_
#define ARGS_H_


// Arguments structure
typedef struct {
	char *nick;				// nick name
	char *new_nick; 		// new nick name
	char *host;				// servers hostname
	int port;				// servers port number
	int f_listen; 			// listen flag; if 1 act as server
} prog_args;


// Function declarations
prog_args*  check_args(int, char **);

#endif /* ARGS_H_ */
