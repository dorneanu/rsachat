/*
 * RSAChat.h
 *
 *  Created on: Nov 17, 2011
 *      Author: victor
 */
#ifndef RSACHAT_H_
#define RSACHAT_H_

#include <string.h>

#define D_CONFIG_VERSION    "0.1"
#define D_CONFIG_AUTHOR     "Victor Dorneanu & Simon Gene"
#define BUFSIZE				1024

// Useful macros
#define M_ERROR fprintf(stderr, ">> %s:%d Error: %s\n", \
						__FILE__, __LINE__, strerror(errno))
#define M_PRINT_ERROR(msg) fprintf(stderr, "%s:%d >> Error: %s\n", \
						__FILE__, __LINE__, msg)
#define M_EXIT_ON_ERROR(msg) {M_PRINT_ERROR(msg); exit(EXIT_FAILURE);}
#define M_PRINT_STATUS(...) fprintf(stdout, "\033[41m[--]\033[0m " __VA_ARGS__ )


#define SQLITE3_DEFAULT_DB			"~/.chatstore"


// Structures
struct thread_info {    		/* Used as argument to thread_start() */
    pthread_t thread_id;        /* ID returned by pthread_create() */
    int       thread_num;       /* Application-defined thread # */
    char     *thread_name;      /* From command-line argument */
    int 	  socket_fd;		/* socket fd */
};


#endif /* RSACHAT_H_ */
