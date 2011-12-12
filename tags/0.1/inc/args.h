#ifndef ARGS_H_
#define ARGS_H_
#include <sqlite3.h>


// Arguments structure
typedef struct {
	char *nick;				// nick name
	char *new_nick; 		// new nick name
	char *host;				// servers hostname
	int port;				// servers port number
	int f_listen; 			// listen flag; if 1 act as server
	int f_list_keys;		// print available keys
	sqlite3 *db;			// DB connection;
} prog_args;


// Function declarations
prog_args*  parse_args(int, char **);

#endif /* ARGS_H_ */
