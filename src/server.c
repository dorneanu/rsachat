/*
 * server.c
 *
 *  Created on: Nov 22, 2011
 *      Author: victor
 */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <unistd.h>
#include <syslog.h>
#include <fcntl.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include "RSAChat.h"


// FIXME: 20 might be to much
#define	MAXFD 20


volatile fd_set the_state;
pthread_mutex_t mutex_state = PTHREAD_MUTEX_INITIALIZER;


// Declarations
int   chat_server_init(int);
void  chat_server_start();
int   chat_server_client_handler(int);
void  chat_server_loop(int);
void* chat_server_read(void *);
int   chat_server_write(int, char[], int);
void  chat_server_write_to_clients(int, char[]);




/**
 * Initialize chat server to listen on port <port>
 *
 * @param port port number to listen on
 * @return socket fd
 */
int chat_server_init(int port) {
	int listen_fd = -1;
	int ret 	  = -1;
	int yes 	  = 1;
	struct sockaddr_in sock;

	if ((listen_fd = socket(PF_INET, SOCK_STREAM, 0)) < 0 )
		M_EXIT_ON_ERROR("Can't create listening socket");

	// Get rid of "Address already in use" error
	if ((ret = setsockopt (listen_fd,SOL_SOCKET, SO_REUSEADDR,
			&yes, sizeof(int))) < 0 )
		M_EXIT_ON_ERROR("Can't reuse port");


	// Fill sock structure with NULL
	memset((char *) &sock, 0, sizeof(sock));

	// Bind address to created socket
	sock.sin_family = AF_INET;
	sock.sin_addr.s_addr = htonl(INADDR_ANY);
	sock.sin_port = ntohs(port);

	if ((ret = bind(listen_fd, (struct sockaddr *) &sock, sizeof(sock))) < 0)
		M_EXIT_ON_ERROR("Can't bind to port");


	// Listen for connections on the socket
	// 5: maximum length to which the queue of pending connections
	// TODO: Change 5 to some generic value
	if ((ret = listen(listen_fd, 5)) < 0 )
		M_EXIT_ON_ERROR("Can't listen to connections");


	return listen_fd;
}


/**
 * Handles new client connection
 *
 * @param socket_fd listening socket
 * @return socket fd to communicate to the client
 */
int chat_server_client_handler(int socket_fd) {
	int fd = -1;
	struct sockaddr_in sock;
	socklen_t socklen;

	socklen = sizeof(socklen);
	if ((fd = accept(socket_fd, (struct sockaddr *) &sock, &socklen)) < 0 )
		M_EXIT_ON_ERROR("Error on new connection");

	return fd;
}


/**
 * Simple loop for accepting connnections
 * @param socket_fd listening socket
 */
void chat_server_loop(int socket_fd) {
	pthread_t threads[MAXFD];
	struct thread_info *tinfo;
	char chat_line[BUFSIZE];
	int num_threads = 0;

	FD_ZERO(&the_state);

	/* Allocate memory for pthread_create() arguments */
	if ((tinfo = calloc(MAXFD, sizeof(struct thread_info))) == NULL)
		M_EXIT_ON_ERROR("Can't allocate memory");

	// The loop
	while (1) {
		int client_fd;
		client_fd = chat_server_client_handler(socket_fd);

		// If maximum number of connections has been reached, just ignore
		// the connection
		if (client_fd >= 0) {
			if (client_fd >= MAXFD) {
				close(client_fd);
				continue;
			}
		}

		// Mutual exclusion to client
		pthread_mutex_lock(&mutex_state);

		// Add new client
		FD_SET(client_fd, &the_state);
		num_threads++;

		// Create new thread
		tinfo[num_threads-1].socket_fd = client_fd;
		tinfo[num_threads-1].thread_num = num_threads-1;

		// Unlock mutex
		pthread_mutex_unlock(&mutex_state);

		// Notify clients about new connection
		sprintf(chat_line, "!!! thread_%d has entered chat", tinfo[num_threads-1].thread_num);
		chat_server_write_to_clients(client_fd, chat_line);

		// Pass argument client_fd to start routine chat_server_read
		pthread_create(&threads[client_fd], NULL, chat_server_read, &tinfo[num_threads-1]);
	}

}


/**
 * Thread which communicates to the clients through sockets
 *
 * @param arg arguments given to the thread
 */
void *chat_server_read(void *arg) {
	struct thread_info *tinfo = (struct thread_info *) arg;
	int client_fd = tinfo->socket_fd;
	char buf[BUFSIZE];
	char chat_line[BUFSIZE];
	int buflen = 0;

	// Read from client
	while(1) {

		if ((buflen = read(client_fd, buf, sizeof(buf))) <= 0) {
			// Client is no longer available
			pthread_mutex_lock(&mutex_state);
			FD_CLR(client_fd, &the_state);	     // delete unavailable client fd
			pthread_mutex_unlock(&mutex_state);

			// Send notification to remaining clients
			sprintf(chat_line,"!!! thread_%d has left the chat", tinfo->thread_num);
			chat_server_write_to_clients(client_fd, chat_line);

			// Close thread
			close(client_fd);
			pthread_exit(NULL);
		}

		// Concatenate new chat line
		sprintf(chat_line, "> thread_%d: %s", tinfo->thread_num, buf);

		// TODO: Delete me
		printf("chat_line: <%s>\n", chat_line);
		fflush(stdout);
		chat_server_write_to_clients(client_fd, chat_line);
	}

	// Ever reached?
	return NULL;
}


/**
 * Write buf of size buflen to socket fd
 *
 * @param fd socket fd to write to
 * @param buf data buffer
 * @param buflen length of data buffer
 * @return -1 if error, otherwise 0
 */
int chat_server_write(int fd, char buf[], int buflen) {
	int ret;

	if ((ret = write(fd, buf, buflen)) != buflen)
		return -1;
	else
		return ret;
}


/**
 * Send data to all clients except one
 *
 * @param exclude_fd socket fd which should be ignored
 * @param buf buffer to be sent
 */
void chat_server_write_to_clients(int exclude_fd, char buf[]) {
	int fd;

	// Send data/message to all other clients
	// Ignore standard file descriptors 0,1,2
	pthread_mutex_lock(&mutex_state);

	for (fd = 3; fd < MAXFD; ++fd) {
		// Write data (but not to current fd)
		if (FD_ISSET(fd, &the_state) && (exclude_fd != fd))
			chat_server_write(fd, buf, strlen(buf));
	}

	pthread_mutex_unlock(&mutex_state);
}



/**
 * Starts chat server
 *
 * @param port port number
 */
void chat_server_start(int port) {
	int socket;

	socket = chat_server_init(port);
	chat_server_loop(socket);
}


