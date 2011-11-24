/*
 * client.c
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
#include <time.h>
#include "RSAChat.h"
#include "client.h"



/**
 * Connect to given host and port number
 *
 * @param host hostname of server
 * @param port port number
 * @return server socket fd
 */
int chat_client_connect(char *host, int port) {
	struct sockaddr_in sock;
	struct hostent *server;
	int server_socket_fd, ret;

	if ((server_socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
		M_EXIT_ON_ERROR("Can't create socket");

	// Zero sock structure
	memset((char *) &sock, 0, sizeof(sock));

	// Get host by name
	if ((server = gethostbyname(host)) == NULL)
		M_EXIT_ON_ERROR("No such host");

	// Connect to server
	sock.sin_family = AF_INET;
	sock.sin_port   = ntohs(port);
	bcopy((char *)server->h_addr, (char *)&sock.sin_addr.s_addr, server->h_length);

	if ((ret = connect(server_socket_fd,
					  (struct sockaddr *) &sock, sizeof(sock))) < 0 )
		M_EXIT_ON_ERROR("Error connecting to server");

	M_PRINT_STATUS("Connected to server!\n");
	return server_socket_fd;
}



/**
 * Reads data from server
 * @param socket_fd servers socket
 * @param buf data buffer
 * @param buflen length of data buffer
 *
 */
int chat_client_read(int socket_fd, char buf[], int buflen) {
	int ret;

	if ((ret = read(socket_fd, buf, buflen)) < 0) {
		M_PRINT_ERROR("Error reading from socket");
		return -1;
	} else {
		return ret;
	}
}


/**
 * Sends data to server
 *
 * @param socket_fd servers socket
 * @param buf data buffer
 * @param buflen length of data buffer
 * @return -1 if error, 0 otherwise
 */
int chat_client_write(int socket_fd, char buf[], int buflen) {
	int ret;

	if ((ret = write(socket_fd, buf, buflen)) != buflen)
		return -1;
	else
		return 0;

}


/*
 * Same effect as fgets but we overwrite the '\n' by '\0'
 */
char *mygetline(char *line, int size)
{
	if (fgets(line, size, stdin)) {
		char *newline = strchr(line, '\n'); /* check for trailing '\n' */
		if (newline) {
			*newline = '\0'; /* overwrite the '\n' with a terminating null */
		}
	}
	return line;
}


/**
 * Reads from stdin and sends data to server
 *
 * @param server_socket_fd socket to server
 */
void chat_client_read_and_send(int server_socket_fd) {
	char buf[BUFSIZE];

	// Read input and send it to server
	while (1) {
		memset(buf, 0, BUFSIZE);

		if(!(mygetline(buf, BUFSIZE)))
			M_PRINT_ERROR("fgets error");

		// Send to server
		chat_client_write(server_socket_fd, buf, BUFSIZE);
	}
}



/**
 * Thread to receive from server
 */
void *chat_client_receive(void *arg) {
	struct thread_info *tinfo = (struct thread_info *) arg;
	int server_fd = tinfo->socket_fd;
	char buf[BUFSIZE];
	int num_bytes = 0;

	while (1) {
		memset(buf, 0, BUFSIZE);
		num_bytes = chat_client_read(server_fd, buf, BUFSIZE);

		// Check if server closed the connection
		if (num_bytes == 0) {
			M_PRINT_ERROR("Lost server connection");
			close(server_fd);
			exit(EXIT_FAILURE);
		} else {
			printf("%s\n", buf);
		}
	}

	return NULL;
}


/**
 * Start a new client
 *
 * @param host hostname of server
 * @param port port number
 */
void chat_client_start(char *host, int port) {
	int server_socket_fd = -1;
	struct thread_info *tinfo;
	pthread_t threadID;

	// Allocate mem for struct thread_info
	if ((tinfo = malloc(sizeof(struct thread_info))) == NULL)
		M_EXIT_ON_ERROR("Can't allocate memory");

	server_socket_fd = chat_client_connect(host, port);
	tinfo->socket_fd = server_socket_fd;


	// 2 Threads: One for receiving data from server, one for sending data
	pthread_create(&threadID, NULL, chat_client_receive, tinfo);
	chat_client_read_and_send(server_socket_fd);

	pthread_exit(NULL);
}

